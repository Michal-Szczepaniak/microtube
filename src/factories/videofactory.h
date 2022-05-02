#ifndef VIDEOFACTORY_H
#define VIDEOFACTORY_H

#include "../entities/video.h"
#include <QJsonObject>
#include <memory>
#include <QtSql/QSqlRecord>

class VideoFactory
{
public:
    static std::unique_ptr<Video> fromJson(QJsonObject video);
    static std::unique_ptr<Video> fromTrendingJson(QJsonObject video);
    static std::unique_ptr<Video> fromRecommendedJson(QJsonObject video);
    static std::unique_ptr<Video> fromVideoInfoJson(QJsonObject video);
    static Video* fromSqlRecord(QSqlRecord record);
};

#endif // VIDEOFACTORY_H
