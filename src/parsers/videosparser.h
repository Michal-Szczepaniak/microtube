#ifndef VIDEOSPARSER_H
#define VIDEOSPARSER_H

#include <memory>
#include <vector>
#include <entities/video.h>
#include <QJsonArray>
#include <variant>
#include <entities/playlist.h>

typedef std::vector<std::variant<Author, Playlist, std::unique_ptr<Video>>> SearchResults;
typedef std::variant<Author, Playlist, std::unique_ptr<Video>> SearchResult;

class VideosParser
{
public:

    static SearchResults parse(const QJsonArray videos);
    static SearchResults parseTrending(const QJsonArray videos);
    static SearchResults parseRecommended(const QJsonArray videos);
    static SearchResults parseChanelVideos(const QJsonArray videos);
    static SearchResults parsePlaylist(const QJsonArray videos);
};

#endif // VIDEOSPARSER_H
