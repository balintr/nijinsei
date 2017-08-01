#include "song.h"

Song::Song()
  :_id(-1),
   _title(QString()),
   _titleUnicode(QString()),
   _artist(QString()),
   _artistUnicode(QString()),
   _path(QString()),
   _dir(QString()),
   _images(QStringList())
{

}

Song::~Song()
{

}

uint Song::id() const
{
  return _id;
}

QString Song::artist() const
{
  return _artist;
}

QString Song::artistUnicode() const
{
  return _artist;
}

QString Song::title() const
{
  return _title;
}

QString Song::titleUnicode() const
{
  return _title;
}

QString Song::path() const
{
  return _path;
}

QString Song::dir() const
{
  return _dir;
}

QStringList Song::images() const
{
  return _images;
}

void Song::setId(uint id)
{
  _id = id;
}

void Song::setArtist(const QString artist)
{
  _artist = artist;
}

void Song::setArtistUnicode(const QString artistUnicode)
{
  _artistUnicode = artistUnicode;
}

void Song::setTitle(const QString title)
{
  _title = title;
}

void Song::setTitleUnicode(const QString titleUnicode)
{
  _titleUnicode = titleUnicode;
}

void Song::setPath(const QString path)
{
  _path = path;
}

void Song::setDir(const QString dir)
{
  _dir = dir;
}

void Song::addImage(const QString image)
{
  bool imageExists = false;
  for (auto it = _images.cbegin(); it != _images.cend(); ++it)
  {
    if (*it == image)
    {
      imageExists = true;
      break;
    }
  }

  if (!imageExists)
    _images.push_back(image);
}

QString Song::debugString() const
{
  QString debugStr;

  debugStr.append(QString::number(_id) + "_");
  debugStr.append(_artist + "_");
  debugStr.append(_artistUnicode + "_");
  debugStr.append(_title + "_");
  debugStr.append(_artistUnicode + "_");
  debugStr.append(_path + "_");
  debugStr.append(_dir + "_");

  for (auto it = _images.cbegin(); it != _images.cend(); ++it)
    debugStr.append(*it + "_");

  return debugStr;
}
