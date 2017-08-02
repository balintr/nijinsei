#include "songparser.h"

using namespace SongParser;

Parser::Parser(const QString &osuSongPath)
  :_songsDir(osuSongPath),
   _threadPool(QThreadPool::globalInstance()),
   _totalSongs(0),
   _songsParsed(0)
{}

void Parser::start()
{
  QStringList directories = _songsDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  QFuture<void> pathExtending = QtConcurrent::map(directories,
                                                  [this] (QString& data) { data = _songsDir.path() + "/" + data; });
  pathExtending.waitForFinished();

  int freeThreads = _threadPool->maxThreadCount() - _threadPool->activeThreadCount();
  qDebug() << "Free threads: " << freeThreads;

  _totalSongs = directories.length();
  int maxSliceSize = directories.length() / freeThreads;

  qDebug() << "Max slice: " << maxSliceSize;
  QStringList::iterator begin;
  QStringList::iterator end;
  // TODO: random crash ?
  for (int slice = 0; slice < maxSliceSize; ++slice)
  {
    QStringList newList;

    begin = (directories.begin() + (slice * maxSliceSize));
    if (begin == directories.end())
      break;

    if (slice == maxSliceSize - 1)
      end = directories.end();
    else
      end = (directories.begin() + ((slice + 1) * maxSliceSize));

    for (auto shittyIt = begin; shittyIt != end; ++shittyIt)
    {
      newList.push_back(*shittyIt);
    }

    SongDirectoryParser* newParser = new SongDirectoryParser(newList);
    connect(newParser, SIGNAL(progressed(uint, uint)),
            this, SLOT(parsingProgressed(uint, uint)));
    _threadPool->start(newParser);
  }

  emit finished();
}

void Parser::parsingProgressed(uint, uint)
{
  emit progressed(++_songsParsed, _totalSongs);
}

SongDirectoryParser::SongDirectoryParser(const QStringList& songDirList)
  :_songs(nullptr),
   _songDirList(songDirList)
{}

void SongDirectoryParser::run()
{
  uint totalSongNumber = _songDirList.length();
  qDebug() << "Total songs: " << totalSongNumber;
  for (auto dirIt = _songDirList.cbegin();
       dirIt != _songDirList.cend();
       ++dirIt)
  {
    qDebug() << "Parsing: " << *dirIt;
    bool firstFileVisited = false;
    uint parsedSongs = 0;

    QDir songDir(*dirIt);
    songDir.setNameFilters(QStringList("*.osu"));

    Song song;
    song.setDir(songDir.absolutePath());

    QRegularExpressionMatch match;

    QStringList songConfigFiles = songDir.entryList(QDir::Files);
    for (auto songConfigIt = songConfigFiles.cbegin();
             songConfigIt != songConfigFiles.cend();
             ++songConfigIt)
    {
      QFile currentSongConfig(songDir.path() + "/" + *songConfigIt);
      if (currentSongConfig.open(QIODevice::ReadOnly))
      {
        QString content(currentSongConfig.readAll());

        if (!firstFileVisited)
        {
          firstFileVisited = true;
          for (int songMetaEnum = SongMeta::Id;
               songMetaEnum != SongMeta::Video;
               ++songMetaEnum)
          {
            match = SongPatterns[songMetaEnum].match(&content);
            if (match.hasMatch())
            {
              switch(songMetaEnum)
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
                  break;
                case SongMeta::Video:
                  song.setVideo(match.captured(1));
              }
            }
          }
        }
        else
        {
          match = SongPatterns[SongMeta::Image].match(&content);
          if (match.hasMatch())
          {
            song.addImage(match.captured(1));
          }

          match = SongPatterns[SongMeta::Video].match(&content);
          if (match.hasMatch())
          {
            song.setVideo(match.captured(1));
          }
        }
      }
      else
        qDebug() << "\tCouldn't open " << *songConfigIt << "\n";

    }

    qDebug() << song.debugString();
    emit progressed(parsedSongs, totalSongNumber);
  }
}

