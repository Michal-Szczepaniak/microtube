#ifndef VIDEOREPOSITORY_H
#define VIDEOREPOSITORY_H

#include "baserepository.h"
#include "src/entities/video.h"
#include <memory>

class VideoRepository : public BaseRepository<Video*>
{
public:
    ~VideoRepository();

    Video *get(int id);
    void put(Video *entity);
    void update(int id);
    void remove(int id);
    bool has(int id) const;
    bool has(QString videoId) const;
    bool isWatched(int id) const;
    bool isWatched(QString videoId) const;

    void deleteAll();

    Video *getOneByVideoId(QString videoId);
    static std::vector<std::unique_ptr<Video>> getSubscriptions();
    static std::vector<std::unique_ptr<Video>> getUnwatchedSubscriptions();
    static std::vector<std::unique_ptr<Video>> getChannelVideos(int authorId);
    void setAllWatchedStatus(bool watched);

    static void initTable();
};

#endif // VIDEOREPOSITORY_H
