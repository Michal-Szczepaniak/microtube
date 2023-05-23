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
    static std::unique_ptr<Video> fromChannelVideosJson(QJsonObject video);
    static int parseAmount(QString amount);

private:
    static uint parseTimestamp(QString timestamp);
    static QString formatDuration(QTime duration);
};

#endif // VIDEOFACTORY_H
