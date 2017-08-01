#ifndef SONGPARSER_H
#define SONGPARSER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <QRegularExpression>

#include "song.h"

enum class SongMeta {
  Id,
  File,
  Artist,
  ArtistUnicode,
  Title,
  TitleUnicode,
  Image
};

const QStringList songPatterns {
  QString("BeatmapID:\\s*(.+?)\\s*\\r*\\n"),
  QString("AudioFilename:\\s*(.+?)\\s*\\r*\\n"),
  QString("Title:\\s*(.+?)\\s*\\r*\\n"),
  QString("TitleUnicode:\\s*(.+?)\\s*\\r*\\n"),
  QString("Artist:\\s*(.+?)\\s*\\r*\\n"),
  QString("ArtistUnicode:\\s*(.+?)\\s*\\r*\\n"),
  QString("Background and Video events\\r\\n.+\"(.+?)\"")
};

/**
 * @brief The ConfigParser class
 */
class SongParser : public QObject
{
    Q_OBJECT

  public:
    /**
     * @brief ConfigParser
     * Deleted default construcor
     */
    SongParser() = delete;
    /**
     * @brief ConfigParser
     * Deleted copy constructor
     */
    SongParser(SongParser&) = delete;
    /**
     * @brief ConfigParser
     * Deleted move constructor
     */
    SongParser(SongParser&&) = delete;
    /**
     * @brief ConfigParser
     * @param songPath
     *
     * Constructor which takes a song directory path as parameter.
     * The only way to set a path to the ConfigParser object.
     */
    SongParser(const QString&);

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

  public slots:
    /**
     * @brief Start
     *
     * Starts processing the directory containing the songs
     */
    void Start();


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
    void advanced(uint, uint);
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
    void advanced(uint, uint);

  protected:
    virtual void run();

  private:
    QVector<Song>* _songs;
    const QStringList _songDirList;
};


#endif // SONGPARSER_H
