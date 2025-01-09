#include "authorrepository.h"
#include <QSqlQuery>
#include <QVariant>
#include <src/factories/authorfactory.h>
#include <QSqlError>
#include <QDebug>

Author AuthorRepository::get(int id)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE id = ?");
    q.addBindValue(QVariant::fromValue(id));
    q.exec();

    bool ret = q.next();
    if (ret) {
        return AuthorFactory::fromSqlRecord(q.record());
    } else {
        qWarning() << "Couldn't fetch. id: " << id;
    }

    return {};
}

void AuthorRepository::put(Author &entity)
{
    if (entity.authorId == "") return;

    QSqlQuery q;
    q.prepare("INSERT INTO author(authorId, name, url, avatar, subscribed, ignored, latestVideoId) VALUES (?,?,?,?,?,?,?)");
    q.addBindValue(QVariant::fromValue(entity.authorId));
    q.addBindValue(QVariant::fromValue(entity.name));
    q.addBindValue(QVariant::fromValue(entity.url));
    q.addBindValue(QVariant::fromValue(entity.bestAvatar.url));
    q.addBindValue(QVariant::fromValue(entity.subscribed));
    q.addBindValue(QVariant::fromValue(entity.ignored));
    q.addBindValue(QVariant::fromValue(entity.latestVideoId));
    q.exec();

    QVariant v = q.lastInsertId();
    Q_ASSERT(v.isValid());

    entity.id = v.toInt();
}

void AuthorRepository::update(Author entity)
{
    if (entity.id == -1) return;

    QSqlQuery q;
    q.prepare("UPDATE author set authorId = ?, name = ?, url = ?, avatar = ?, subscribed = ?, ignored = ?, latestVideoId = ? WHERE id = ?");

    q.addBindValue(QVariant::fromValue(entity.authorId));
    q.addBindValue(QVariant::fromValue(entity.name));
    q.addBindValue(QVariant::fromValue(entity.url));
    q.addBindValue(QVariant::fromValue(entity.bestAvatar.url));
    q.addBindValue(QVariant::fromValue(entity.subscribed));
    q.addBindValue(QVariant::fromValue(entity.ignored));
    q.addBindValue(QVariant::fromValue(entity.latestVideoId));
    q.addBindValue(QVariant::fromValue(entity.id));
    q.exec();
}

void AuthorRepository::remove(Author entity)
{
    Q_ASSERT(entity.id);

    QSqlQuery q;
    q.prepare("DELETE FROM author WHERE id = ?");
    q.addBindValue(entity.id);
    q.exec();
}

bool AuthorRepository::has(QString channelId) const
{
    QSqlQuery q;
    q.prepare("SELECT id FROM author WHERE authorId = ? AND subscribed = true");
    q.addBindValue(QVariant::fromValue(channelId));
    q.exec();

    return q.next();
}

Author AuthorRepository::getOneByChannelId(QString channelId)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE authorId = ?");
    q.addBindValue(QVariant::fromValue(channelId));
    q.exec();

    bool ret = q.next();
    if (ret) {
        return AuthorFactory::fromSqlRecord(q.record());
    } else {
        qWarning() << "Couldn't fetch. Channel id: " << channelId;
    }

    return {};
}

QVector<Author> AuthorRepository::getSubscriptions()
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE subscribed = true");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "AuthorRepository::getSubscriptions", q.lastError().text().toLatin1());

    QVector<Author> result;
    while(q.next()) {
        result.append(AuthorFactory::fromSqlRecord(q.record()));
    }

    return result;
}

QVector<Author> AuthorRepository::getSubscriptionsWithUnwatchedCount()
{
    QSqlQuery q;
    q.prepare("SELECT author.*, COUNT(video.id) as unwatchedCount FROM author LEFT JOIN video ON video.author = author.id AND video.watched = 0 AND isUpcoming = 0 AND isLive = 0 WHERE subscribed = 1 GROUP BY author.id");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "AuthorRepository::getSubscriptionsWithUnwatchedCount", q.lastError().text().toLatin1());

    QVector<Author> result;
    while(q.next()) {
        result.append(AuthorFactory::fromSqlRecord(q.record()));
    }

    return result;
}

void AuthorRepository::initTable()
{
    QSqlQuery q;
    q.prepare("create table if not exists author (id INTEGER PRIMARY KEY AUTOINCREMENT, authorId TEXT, name TEXT, url TEXT, avatar TEXT, subscribed BOOLEAN)");
    q.exec();

    q.prepare("SELECT COUNT(*) AS CNTREC FROM pragma_table_info('author') WHERE name='ignored'");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::initTable", q.lastError().text().toLatin1());

    Q_ASSERT_X(q.first(), "VideoRepository::initTable", "Could not execute query");

    int count = q.value(0).toInt();
    if (count == 0) {
        q.prepare("ALTER TABLE author ADD COLUMN ignored BOOLEAN;");
        q.exec();

        Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::initTable", q.lastError().text().toLatin1());
    }

    q.prepare("SELECT COUNT(*) AS CNTREC FROM pragma_table_info('author') WHERE name='latestVideoId'");
    q.exec();

    Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::initTable", q.lastError().text().toLatin1());

    Q_ASSERT_X(q.first(), "VideoRepository::initTable", "Could not execute query");

    count = q.value(0).toInt();
    if (count == 0) {
        q.prepare("ALTER TABLE author ADD COLUMN latestVideoId TEXT;");
        q.exec();

        Q_ASSERT_X(!q.lastError().isValid(), "VideoRepository::initTable", q.lastError().text().toLatin1());
    }
}
