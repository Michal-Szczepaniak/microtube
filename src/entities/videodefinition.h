#ifndef VIDEODEFINITION_H
#define VIDEODEFINITION_H

#include <QString>
#include <QMap>
#include <QVector>

class VideoDefinition {
public:
    VideoDefinition() = delete;

    static const QMap<quint32, QVector<int>> videoDefinitions;
    static const QMap<quint32, QVector<int>> videoDefinitionsAvc1;
    static const QMap<quint32, QVector<int>> videoDefinitionsCombined;
    static const QMap<quint32, QVector<int>> liveVideoDefinitions;
    static const QVector<int> audioDefinitions;
};

const QMap<quint32, QVector<int>> VideoDefinition::videoDefinitions = {
    { 2160, { 315, 313 } },
    { 1440, { 308, 271 } },
    { 1080, { 303, 248 } },
    { 720,  { 302, 247, 22 } },
    { 480,  {      244 } },
    { 360,  {      243, 18 } },
    { 240,  {      242 } }
};

const QMap<quint32, QVector<int>> VideoDefinition::videoDefinitionsAvc1 = {
    { 1080, { 299, 137 } },
    { 720,  { 298, 136 } },
    { 480,  { 135 } },
    { 360,  { 134 } },
    { 240,  { 133 } },
};

const QMap<quint32, QVector<int>> VideoDefinition::videoDefinitionsCombined = {
    { 720,  { 22 } },
    { 360,  { 18 } }
};

const QMap<quint32, QVector<int>> VideoDefinition::liveVideoDefinitions = {
/*    { 2160, { 315 } },
    { 1440, { 308 } },
    { 1080, { 303, 299, 248, 137, 96 } },
    { 720,  { 302, 298, 247, 136, 95 } },
    { 480,  {           244, 135, 94 } },
    { 360,  {           243, 134, 93 } },
    { 240,  {           242, 133, 92 } }
    Sadly only combined streams work fine and I don't want to debug why
    but it looks so nice I'm gonna leave it for future reference
    */

    { 1080, { 301, 96 } },
    { 720,  { 300, 95 } },
    { 480,  {      94 } },
    { 360,  {      93 } },
    { 240,  {      92 } }
};

const QVector<int> VideoDefinition::audioDefinitions = {
    251, 250, 249
};

#endif // VIDEODEFINITION_H
