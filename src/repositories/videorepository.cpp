#include "authorrepository.h"
#include "videorepository.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlResult>
#include <QVariant>
#include <QDebug>
#include <QSqlError>
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
        Author a = ar.getOneByChannelId(entity->author.authorId);
        if (a.id == -1) {
            ar.put(entity->author);
        } else {
            entity->author = a;
        }
    }

    QSqlQuery q;
    q.prepare("INSERT INTO video(author, duration, videoId, title, url, views, thumbnail, bigThumbnail, watched, uploadedAt, timestamp, isLive, isUpcoming, upcoming) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    q.addBindValue(QVariant::fromValue(entity->author.id));
    q.addBindValue(QVariant::fromValue(entity->duration));
    q.addBindValue(QVariant::fromValue(entity->videoId));
    q.addBindValue(QVariant::fromValue(entity->title));
    q.addBindValue(QVariant::fromValue(entity->url));
    q.addBindValue(QVariant::fromValue(entity->views));
    q.addBindValue(QVariant::fromValue(entity->thumbnails[Thumbnail::SD].url));
    q.addBindValue(QVariant::fromValue(entity->thumbnails[Thumbnail::HD].url));
    q.addBindValue(QVariant::fromValue(entity->watched));
    q.addBindValue(QVariant::fromValue(entity->uploadedAt));
    q.addBindValue(QVariant::fromValue(entity->timestamp));
    q.addBindValue(QVariant::fromValue(entity->isLive));
    q.addBindValue(QVariant::fromValue(entity->isUpcoming));
    q.addBindValue(QVariant::fromValue(entity->upcoming));
    q.exec();

    QVariant v = q.lastInsertId();
    Q_ASSERT_X(v.isValid(), "VideoRepository::put", q.lastError().text().toLatin1());

    entity->id = v.toInt();
}

void VideoRepository::update(int id)
{
    QSqlQuery q;
    q.prepare("UPDATE video set author = ?, duration = ?, videoId = ?, title = ?, url = ?, views = ?, thumbnail = ?, bigThumbnail = ?, watched = ?, uploadedAt = ?, timestamp = ?, isLive = ?, isUpcoming = ?, upcoming = ? WHERE id = ?");

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
    q.addBindValue(QVariant::fromValue(entity->uploadedAt));
    q.addBindValue(QVariant::fromValue(entity->timestamp));
    q.addBindValue(QVariant::fromValue(entity->isLive));
    q.addBindValue(QVariant::fromValue(entity->isUpcoming));
    q.addBindValue(QVariant::fromValue(entity->upcoming));
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

Video *VideoRepository::getOneByVideoId(QString videoId)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM video WHERE videoId = ?");
    q.addBindValue(QVariant::fromValue(videoId));
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

std::vector<std::unique_ptr<Video>> VideoRepository::getSubscriptions()
{
    QSqlQuery q;
    q.prepare("SELECT video.* FROM video JOIN author ON author.id = video.author WHERE subscribed = true ORDER BY timestamp desc");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::getSubscriptions", q.lastError().text().toLatin1());

    std::vector<std::unique_ptr<Video>> result;
    while (q.next()) {
        result.push_back(std::unique_ptr<Video>(VideoFactory::fromSqlRecord(q.record())));
    }

    return result;
}

std::vector<std::unique_ptr<Video> > VideoRepository::getUnwatchedSubscriptions()
{
    QSqlQuery q;
    q.prepare("SELECT video.* FROM video JOIN author ON author.id = video.author WHERE subscribed = true AND watched = false ORDER BY timestamp desc");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::getSubscriptions", q.lastError().text().toLatin1());

    std::vector<std::unique_ptr<Video>> result;
    while (q.next()) {
        result.push_back(std::unique_ptr<Video>(VideoFactory::fromSqlRecord(q.record())));
    }

    return result;
}

std::vector<std::unique_ptr<Video> > VideoRepository::getChannelVideos(int authorId)
{
    QSqlQuery q;
    q.prepare("SELECT video.* FROM video JOIN author ON author.id = video.author WHERE subscribed = true AND video.author = ? ORDER BY timestamp desc");
    q.addBindValue(QVariant::fromValue(authorId));
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::getChannelVideos", q.lastError().text().toLatin1());

    std::vector<std::unique_ptr<Video>> result;
    while (q.next()) {
        result.push_back(std::unique_ptr<Video>(VideoFactory::fromSqlRecord(q.record())));
    }

    return result;
}

void VideoRepository::setAllWatchedStatus(bool watched)
{
    QSqlQuery q;
    q.prepare("UPDATE video set watched = ? WHERE 1=1");
    q.addBindValue(QVariant::fromValue(watched));
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::setAllWatchedStatus", q.lastError().text().toLatin1());
}

void VideoRepository::initTable()
{
    QSqlQuery q;
    q.prepare("create table if not exists video (id INTEGER PRIMARY KEY AUTOINCREMENT, author INTEGER, duration TEXT, videoId TEXT, title TEXT, url TEXT, views INTEGER, thumbnail TEXT, bigThumbnail TEXT, watched BOOLEAN, uploadedAt TEXT, timestamp INTEGER, isLive BOOLEAN, isUpcoming BOOLEAN, upcoming INTEGER)");
    q.exec();
}
