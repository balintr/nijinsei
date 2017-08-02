#ifndef SONG_H
#define SONG_H

#include <QObject>

class Song
{
  public:
    Song();
    Song(Song&) = default;
    Song(Song&&) = default;
    ~Song();

    uint id() const;
    QString artist() const;
    QString artistUnicode() const;
    QString title() const;
    QString titleUnicode() const;
    QString path() const;
    QString dir() const;
    QStringList images() const;
    QString video() const;

    void setId(const uint);
    void setArtist(const QString);
    void setArtistUnicode(const QString);
    void setTitle(const QString);
    void setTitleUnicode(const QString);
    void setPath(const QString);
    void setDir(const QString);
    void addImage(const QString);
    void setVideo(const QString);

    QString debugString() const;

  private:
    uint _id;
    QString _title;
    QString _titleUnicode;
    QString _artist;
    QString _artistUnicode;
    QString _path;
    QString _dir;
    QStringList _images;
    QString _video;
};

#endif // SONG_H
