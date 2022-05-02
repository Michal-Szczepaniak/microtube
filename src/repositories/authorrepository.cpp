#include "authorrepository.h"
#include <QSqlQuery>
#include <QVariant>
#include <src/factories/authorfactory.h>
#include <QDebug>

Author *AuthorRepository::get(int id)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE id = ?");
    q.addBindValue(QVariant::fromValue(id));
    q.exec();

    bool ret = q.next();
    if (ret) {
        Author* a = AuthorFactory::fromSqlRecord(q.record());
        _trackedObjects[a->id] = a;
        return _trackedObjects[a->id];
    } else {
        qWarning() << "Couldn't fetch";
    }

    return nullptr;
}

void AuthorRepository::put(Author *entity)
{
    QSqlQuery q;
    q.prepare("INSERT INTO author(authorId, name, url, avatar, subscribed) VALUES (?,?,?,?,?)");
    q.addBindValue(QVariant::fromValue(entity->authorId));
    q.addBindValue(QVariant::fromValue(entity->name));
    q.addBindValue(QVariant::fromValue(entity->url));
    q.addBindValue(QVariant::fromValue(entity->bestAvatar.url));
    q.addBindValue(QVariant::fromValue(entity->subscribed));
    q.exec();

    QVariant v = q.lastInsertId();
    Q_ASSERT(v.isValid());

    entity->id = v.toInt();
}

void AuthorRepository::update(int id)
{
    QSqlQuery q;
    q.prepare("UPDATE author set authorId = ?, name = ?, url = ?, avatar = ?, subscribed = ? WHERE id = ?");

    Author* entity = _trackedObjects[id];
    q.addBindValue(QVariant::fromValue(entity->authorId));
    q.addBindValue(QVariant::fromValue(entity->name));
    q.addBindValue(QVariant::fromValue(entity->url));
    q.addBindValue(QVariant::fromValue(entity->bestAvatar.url));
    q.addBindValue(QVariant::fromValue(entity->subscribed));
    q.addBindValue(QVariant::fromValue(entity->id));
    q.exec();
}

void AuthorRepository::remove(int id)
{
    QSqlQuery q;
    q.prepare("DELETE FROM author WHERE id = ?");
    q.addBindValue(id);
    q.exec();

    delete _trackedObjects[id];
    _trackedObjects.remove(id);
}

Author *AuthorRepository::getOneByChannelId(QString channelId)
{
    QSqlQuery q;
    q.prepare("SELECT * FROM author WHERE authorId = ?");
    q.addBindValue(QVariant::fromValue(channelId));
    q.exec();

    bool ret = q.next();
    if (ret) {
        Author* a = AuthorFactory::fromSqlRecord(q.record());
        _trackedObjects[a->id] = a;
        return _trackedObjects[a->id];
    } else {
        qWarning() << "Couldn't fetch";
    }

    return nullptr;
}

void AuthorRepository::initTable()
{
    QSqlQuery q;
    q.prepare("create table if not exists author (id INTEGER PRIMARY KEY AUTOINCREMENT, authorId TEXT, name TEXT, url TEXT, avatar TEXT, subscribed BOOLEAN)");
    q.exec();
}
