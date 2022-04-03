#ifndef AUTHORFACTORY_H
#define AUTHORFACTORY_H

#include <QJsonObject>
#include "../entities/author.h"

class AuthorFactory
{
public:
    static Author fromJson(QJsonObject json);
    static Author fromTrendingJson(QJsonObject json);
};

#endif // AUTHORFACTORY_H
