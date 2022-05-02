#ifndef VIDEOREPOSITORY_H
#define VIDEOREPOSITORY_H

#include "baserepository.h"
#include "src/entities/video.h"

class VideoRepository : public BaseRepository<Video>
{
public:
    Video *get(int id);
    void put(Video *entity);
    void update(int id);
    void remove(int id);

    static void initTable();
};

#endif // VIDEOREPOSITORY_H
