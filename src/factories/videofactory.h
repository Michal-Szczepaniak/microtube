#ifndef VIDEOFACTORY_H
#define VIDEOFACTORY_H

#include "../entities/video.h"
#include <QJsonObject>
#include <memory>

class VideoFactory
{
public:
    static std::unique_ptr<Video> fromJson(QJsonObject video);
    static std::unique_ptr<Video> fromTrendingJson(QJsonObject video);
    static std::unique_ptr<Video> fromRecommendedJson(QJsonObject video);
};

#endif // VIDEOFACTORY_H
