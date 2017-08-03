#include "songparser.h"
#include <algorithm>
#include <cmath>

namespace SongParser {

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

  _totalSongs = directories.length();

  int freeThreads = _threadPool->maxThreadCount() - _threadPool->activeThreadCount();
  if (freeThreads == 0)
    freeThreads = 1;
  qDebug() << "Free threads: " << freeThreads;

  const int maxSliceSize =
    static_cast<int>(std::round(static_cast<float>(directories.length()) / freeThreads));

  qDebug() << "Max slice: " << maxSliceSize;
  // TODO: random crash ?
  for (auto dirIt = directories.cbegin(), endIt = directories.cend();
       dirIt != endIt;
       dirIt += maxSliceSize)
  {
    QStringList newList;

    auto partEndIt = dirIt + std::min(maxSliceSize, endIt - dirIt);
    for (auto shittyIt = dirIt; shittyIt != partEndIt; ++shittyIt)
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
{
  setAutoDelete(true);
}

void SongDirectoryParser::run()
{
  const uint totalSongNumber = _songDirList.length();
  qDebug() << "Total songs: " << totalSongNumber;
  for (const auto &dirStr : _songDirList)
  {
    qDebug() << "Parsing: " << dirStr;
    bool firstFileVisited = false;
    uint parsedSongs = 0;

    QDir songDir(dirStr);
    Song song;
    song.setDir(songDir.absolutePath());

    QRegularExpressionMatch match;

    const QStringList songConfigFiles =
      songDir.entryList(QStringList("*.osu"), QDir::Files);
    for (const auto &songConfig : songConfigFiles)
    {
      QFile currentSongConfig(songDir.path() + "/" + songConfig);
      if (currentSongConfig.open(QIODevice::ReadOnly))
      {
        const QString content(currentSongConfig.readAll());

        if (!firstFileVisited)
        {
          firstFileVisited = true;
          for (int songMetaEnum = SongMeta::Id;
               songMetaEnum != SongMeta::Video;
               ++songMetaEnum)
          {
            match = songPatterns[songMetaEnum].match(&content);
            if (match.hasMatch())
            {
              switch (songMetaEnum)
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
                  break;
                default:
                  Q_ASSERT(0);
              }
            }
          }
        }
        else
        {
          match = songPatterns[SongMeta::Image].match(&content);
          if (match.hasMatch())
          {
            song.addImage(match.captured(1));
          }

          match = songPatterns[SongMeta::Video].match(&content);
          if (match.hasMatch())
          {
            song.setVideo(match.captured(1));
          }
        }
      }
      else
        qDebug() << "\tCouldn't open " << songConfig << "\n";

    }
    ++parsedSongs;

    qDebug() << song.debugString();
    emit progressed(parsedSongs, totalSongNumber);
  }
}

} // namespace SongParser
