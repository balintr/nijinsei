#ifndef SONGPARSER_H
#define SONGPARSER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <QRegularExpression>

#include "song.h"

namespace SongParser
{
  enum SongMeta {
    Id,
    File,
    Artist,
    ArtistUnicode,
    Title,
    TitleUnicode,
    Image,
    Video
  };

  const QHash<int, QRegularExpression> songPatterns {
    {SongMeta::Id, QRegularExpression(QString("BeatmapID:\\s*(.+?)\\s*\\r*\\n"))},
    {SongMeta::File, QRegularExpression(QString("AudioFilename:\\s*(.+?)\\s*\\r*\\n"))},
    {SongMeta::Artist, QRegularExpression(QString("Title:\\s*(.+?)\\s*\\r*\\n"))},
    {SongMeta::ArtistUnicode, QRegularExpression(QString("TitleUnicode:\\s*(.+?)\\s*\\r*\\n"))},
    {SongMeta::Title, QRegularExpression(QString("Artist:\\s*(.+?)\\s*\\r*\\n"))},
    {SongMeta::TitleUnicode, QRegularExpression(QString("ArtistUnicode:\\s*(.+?)\\s*\\r*\\n"))},
    {SongMeta::Image, QRegularExpression(QString("(?:\\s*,?0,\\s*){2}\"(.+?)\""))},
    {SongMeta::Video, QRegularExpression(QString("Video\\s*,\\s*0\\s*,\\s*\"(.+?)\""))}
  };

  /**
   * @brief The ConfigParser class
   */
  class Parser : public QObject
  {
      Q_OBJECT

    public:
      /**
       * @brief ConfigParser
       * @param songPath
       *
       * Constructor which takes a song directory path as parameter.
       * The only way to set a path to the ConfigParser object.
       */
      Parser(const QString&);
      /**
       * @brief ConfigParser
       * Deleted default construcor
       */
      Parser() = delete;
      /**
       * @brief ConfigParser
       * Deleted copy constructor
       */
      Parser(Parser&) = delete;
      /**
       * @brief ConfigParser
       * Deleted move constructor
       */
      Parser(Parser&&) = delete;

    private:
      /**
       * @brief _songsDir
       *
       * The root directory where all the song directories can be found.
       */
      QDir _songsDir;
      /**
       * @brief _threadPool
       *
       * Global thread pool used to parse multiple config batches
       * asynchronously.
       */
      QThreadPool* _threadPool;

      uint _totalSongs;
      uint _songsParsed;

    public slots:
      /**
       * @brief Start
       *
       * Starts processing the directory containing the songs
       */
      void start();

    private slots:
      void parsingProgressed(uint, uint);


    signals:
      /**
       * @brief (signal) finished
       *
       * Indicates that the whole root directory has been parsed
       */
      void finished();
      /**
       * @brief (signal) advanced
       * @param parsedSongs
       * @param totalSongs
       *
       * Indicates that song parsing advanced by 1
       * Returns the total number of songs as first and the number of parsed songs so far
       */
      void progressed(uint, uint);
  };

  class SongDirectoryParser : public QObject, public QRunnable
  {
      Q_OBJECT

    public:
      SongDirectoryParser(const QStringList&);
      SongDirectoryParser() = delete;
      SongDirectoryParser(SongDirectoryParser&) = delete;
      SongDirectoryParser(SongDirectoryParser&&) = delete;
      virtual ~SongDirectoryParser() = default;

    signals:
      void finished(QVector<Song>*);
      void progressed(uint, uint);

    protected:
      virtual void run();

    private:
      QVector<Song>* _songs;
      const QStringList _songDirList;
  };

} // namespace Song

#endif // SONGPARSER_H
