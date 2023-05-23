#ifndef COMMENTFACTORY_H
#define COMMENTFACTORY_H

#include <QJsonObject>
#include "entities/comment.h"
#include "authorfactory.h"
#include <QSqlRecord>

class CommentFactory
{
public:
    static Comment fromJson(QJsonObject json);
    static Comment fromReplyJson(QJsonObject json);

private:
    AuthorFactory _authorFactory;
};

#endif // COMMENTFACTORY_H
