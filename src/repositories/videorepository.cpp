#include "authorrepository.h"
#include "videorepository.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlResult>
#include <QVariant>
#include <QDebug>
#include <src/factories/videofactory.h>

Video *VideoRepository::get(int id)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM video WHERE id = ?");
    q.addBindValue(QVariant::fromValue(id));
    q.exec();

    bool ret = q.next();
    if (ret) {
        Video* v = VideoFactory::fromSqlRecord(q.record());
        _trackedObjects[v->id] = v;
        return _trackedObjects[v->id];
    } else {
        qWarning() << "Couldn't fetch";
    }

    return nullptr;
}

void VideoRepository::put(Video *entity)
{
    if (entity->author.id == -1) {
        AuthorRepository ar;
        ar.put(&entity->author);
    }

    QSqlQuery q;
    q.prepare("INSERT INTO video(author, duration, videoId, title, url, views, thumbnail, bigThumbnail, watched) VALUES (?,?,?,?,?,?,?,?,?)");
    q.addBindValue(QVariant::fromValue(entity->author.id));
    q.addBindValue(QVariant::fromValue(entity->duration));
    q.addBindValue(QVariant::fromValue(entity->videoId));
    q.addBindValue(QVariant::fromValue(entity->title));
    q.addBindValue(QVariant::fromValue(entity->url));
    q.addBindValue(QVariant::fromValue(entity->views));
    q.addBindValue(QVariant::fromValue(entity->thumbnails[Thumbnail::SD].url));
    q.addBindValue(QVariant::fromValue(entity->thumbnails[Thumbnail::HD].url));
    q.addBindValue(QVariant::fromValue(entity->watched));
    q.exec();

    QVariant v = q.lastInsertId();
    Q_ASSERT(v.isValid());

    entity->id = v.toInt();
}

void VideoRepository::update(int id)
{
    QSqlQuery q;
    q.prepare("UPDATE video set author = ?, duration = ?, videoId = ?, title = ?, url = ?, views = ?, thumbnail = ?, bigThumbnail = ?, watched = ? WHERE id = ?");

    Video* entity = _trackedObjects[id];
    q.addBindValue(QVariant::fromValue(entity->author.id));
    q.addBindValue(QVariant::fromValue(entity->duration));
    q.addBindValue(QVariant::fromValue(entity->videoId));
    q.addBindValue(QVariant::fromValue(entity->title));
    q.addBindValue(QVariant::fromValue(entity->url));
    q.addBindValue(QVariant::fromValue(entity->views));
    q.addBindValue(QVariant::fromValue(entity->thumbnails[Thumbnail::SD].url));
    q.addBindValue(QVariant::fromValue(entity->thumbnails[Thumbnail::HD].url));
    q.addBindValue(QVariant::fromValue(entity->watched));
    q.addBindValue(QVariant::fromValue(entity->id));
    q.exec();
}

void VideoRepository::remove(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM video WHERE id = ?");
    q.addBindValue(id);
    q.exec();

    delete _trackedObjects[id];
    _trackedObjects.remove(id);
}

void VideoRepository::initTable()
{
    QSqlQuery q;
    q.prepare("create table if not exists video (id INTEGER PRIMARY KEY AUTOINCREMENT, author INTEGER, duration TEXT, videoId TEXT, title TEXT, url TEXT, views INTEGER, thumbnail TEXT, bigThumbnail TEXT, watched BOOLEAN)");
    q.exec();
}
