#ifndef THUMBNAILFACTORY_H
#define THUMBNAILFACTORY_H

#include <QJsonObject>
#include "../entities/thumbnail.h"

class ThumbnailFactory
{
public:
    static Thumbnail fromJson(QJsonObject json);
    static Thumbnail fromTrendingJson(QJsonObject json);
    static Thumbnail fromRecommendedJson(QJsonObject json);
};

#endif // THUMBNAILFACTORY_H
