#ifndef VIDEOSPARSER_H
#define VIDEOSPARSER_H

#include <QVector>
#include <memory>
#include <vector>
#include "../entities/video.h"
#include <QJsonArray>

class VideosParser
{
public:
    static std::vector<std::unique_ptr<Video>> parse(const QJsonArray videos);
    static std::vector<std::unique_ptr<Video>> parseTrending(const QJsonArray videos);
    static std::vector<std::unique_ptr<Video>> parseRecommended(const QJsonArray videos);
};

#endif // VIDEOSPARSER_H
