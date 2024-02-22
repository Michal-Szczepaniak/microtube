#ifndef PLAYLISTFACTORY_H
#define PLAYLISTFACTORY_H

#include <entities/playlist.h>
#include <QJsonObject>
#include <memory>

class PlaylistFactory
{
public:
    static Playlist fromJson(QJsonObject json);
    static Playlist fromGridJson(QJsonObject json);
};

#endif // PLAYLISTFACTORY_H
