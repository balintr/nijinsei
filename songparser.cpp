#include "songparser.h"

SongParser::SongParser(const QString &osuSongPath)
  :_songsDir(osuSongPath),
   _threadPool(QThreadPool::globalInstance()),
   _totalSongs(0),
   _songsParsed(0)
{}

void SongParser::Start()
{
  QStringList directories = _songsDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  QFuture<void> pathExtending = QtConcurrent::map(directories,
                                                  [this] (QString& data) { data = _songsDir.path() + "/" + data; });
  pathExtending.waitForFinished();

  _totalSongs = directories.size();

  qDebug() << "Active threads: " << _threadPool->activeThreadCount();
  qDebug() << "Maximum threads: " << _threadPool->maxThreadCount();
  qDebug() << "First dir: " << directories.first();

  for (int threads = _threadPool->activeThreadCount(); threads != _threadPool->maxThreadCount(); ++threads)
  {
    int maxSliceSize = directories.size() / (_threadPool->maxThreadCount() - _threadPool->activeThreadCount());

    QStringList newList;
    for (auto shittyIt = directories.begin(); shittyIt != directories.begin() + maxSliceSize; ++shittyIt)
    {
      newList.push_back(*shittyIt);
    }

    qDebug() << "Sub qstringlist last: " << newList.last();

    SongDirectoryParser* newParser = new SongDirectoryParser(newList);
    connect(newParser, SIGNAL(advanced(uint,uint)),
            this, SLOT(ParsingAdvanced(uint, uint)));
    _threadPool->start(newParser);
  }

  emit finished();
}

void SongParser::ParsingAdvanced(uint, uint)
{
  emit advanced(++_songsParsed, _totalSongs);
}

SongDirectoryParser::SongDirectoryParser(const QStringList& songDirList)
  :_songs(nullptr),
   _songDirList(songDirList)
{}

void SongDirectoryParser::run()
{
  uint totalSongNumber = _songDirList.size();
  for (auto dirIt = _songDirList.cbegin(); dirIt != _songDirList.cend(); ++dirIt)
  {
    bool firstFileVisited = false;
    uint parsedSongs = 0;

    QDir songDir(*dirIt);
    songDir.setNameFilters(QStringList("*.osu"));

    Song song;
    song.setDir(songDir.absolutePath());

    QStringList songConfigFiles = songDir.entryList(QDir::Files);
    for (auto songConfigIt = songConfigFiles.cbegin();
             songConfigIt != songConfigFiles.cend();
             ++songConfigIt)
    {
      QFile currentSongConfig(songDir.path() + "/" + *songConfigIt);
      if (currentSongConfig.open(QIODevice::ReadOnly))
      {
        qDebug() << "\tConfig: " << *songConfigIt << "\n";

        QString content(currentSongConfig.readAll());

        QRegularExpression regexp;
        QRegularExpressionMatch match;
        if (!firstFileVisited)
        {
          firstFileVisited = true;
          for (int i = 0; i < songPatterns.size(); ++i)
          {
            regexp.setPattern(songPatterns.at(i));
            match = regexp.match(&content);
            if (match.hasMatch())
            {
              switch((SongMeta) i)
              {
                case SongMeta::Id:
                  song.setId(match.captured(1).toUInt());
                  break;
                case SongMeta::File:
                  song.setPath(match.captured(1));
                  break;
                case SongMeta::Artist:
                  song.setArtist(match.captured(1));
                  break;
                case SongMeta::ArtistUnicode:
                  song.setArtistUnicode(match.captured(1));
                  break;
                case SongMeta::Title:
                  song.setTitle(match.captured(1));
                  break;
                case SongMeta::TitleUnicode:
                  song.setTitleUnicode(match.captured(1));
                  break;
                case SongMeta::Image:
                  song.addImage(match.captured(1));
              }
            }
          }
        }
        else
        {
          regexp.setPattern(songPatterns.at((int) SongMeta::Image));
          match = regexp.match(&content);
          if (match.hasMatch())
          {
            song.addImage(match.captured(1));
          }
        }

      }
      else
        qDebug() << "\tCouldn't open " << *songConfigIt << "\n";

      emit advanced(parsedSongs, totalSongNumber);
    }
  }
}
