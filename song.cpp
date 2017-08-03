#include "song.h"
#include <algorithm>

Song::Song()
  :_id(-1),
   _title(),
   _titleUnicode(),
   _artist(),
   _artistUnicode(),
   _path(),
   _dir(),
   _images()
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
  return _artistUnicode;
}

QString Song::title() const
{
  return _title;
}

QString Song::titleUnicode() const
{
  return _titleUnicode;
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

QString Song::video() const
{
  return _video;
}

void Song::setId(uint id)
{
  _id = id;
}

void Song::setArtist(const QString& artist)
{
  _artist = artist;
}

void Song::setArtistUnicode(const QString& artistUnicode)
{
  _artistUnicode = artistUnicode;
}

void Song::setTitle(const QString& title)
{
  _title = title;
}

void Song::setTitleUnicode(const QString& titleUnicode)
{
  _titleUnicode = titleUnicode;
}

void Song::setPath(const QString& path)
{
  _path = path;
}

void Song::setDir(const QString& dir)
{
  _dir = dir;
}

void Song::addImage(const QString& image)
{
  if (std::find(_images.cbegin(), _images.cend(), image) == _images.cend())
    _images.push_back(image);
}

void Song::setVideo(const QString& video)
{
  _video = video;
}

QString Song::debugString() const
{
  QString debugStr;

  debugStr.append("Id:" + QString::number(_id) + " ");
  debugStr.append("Artist:" + _artist + " ");
  debugStr.append("ArtistUnicode:" + _artistUnicode + " ");
  debugStr.append("Title:" + _title + " ");
  debugStr.append("TitleUnicode:" + _titleUnicode + " ");
  debugStr.append("Path:" + _path + " ");
  debugStr.append("Dir:" + _dir + " ");
  debugStr.append("Video:" + _video + " ");
  debugStr.append("Images:");
  for (const auto& img : _images)
    debugStr.append(img + "#");

  return debugStr;
}
