#ifndef AUTHORFACTORY_H
#define AUTHORFACTORY_H

#include <QJsonObject>
#include <src/entities/author.h>
#include <QSqlRecord>

class AuthorFactory
{
public:
    static Author fromJson(QJsonObject json);
    static Author fromTrendingJson(QJsonObject json);
    static Author* fromSqlRecord(QSqlRecord record);
};

#endif // AUTHORFACTORY_H
