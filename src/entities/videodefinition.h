#ifndef VIDEODEFINITION_H
#define VIDEODEFINITION_H

#include <QString>
#include <QMap>
#include <QVector>

class VideoDefinition {
public:
    VideoDefinition() = delete;

    static const QMap<quint32, QVector<int>> videoDefinitions;
    static const QVector<int> audioDefinitions;
};

const QMap<quint32, QVector<int>> VideoDefinition::videoDefinitions = {
//    { 2160, { 315, 313 } },
//    { 1440, { 308, 271 } },
//    { 1080, { 303, 299, 137, 248 } },
//    { 720,  { 302, 298, 136, 247 } },
//    { 480,  { 135, 244 } },
//    { 360,  { 134, 243 } },
//    { 240,  { 133, 242 } }
    { 2160, { 315, 313 } },
    { 1440, { 308, 271 } },
    { 1080, { 303, 299, 248 } },
    { 720,  { 302, 298, 247 } },
    { 480,  { 244 } },
    { 360,  { 243 } },
    { 240,  { 242 } }
};

const QVector<int> VideoDefinition::audioDefinitions = {
    251, 250, 249
};

#endif // VIDEODEFINITION_H
