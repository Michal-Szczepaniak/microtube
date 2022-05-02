#ifndef CHANNELPARSER_H
#define CHANNELPARSER_H

#include <memory>
#include <QJsonObject>
#include "src/entities/author.h"

class ChannelParser
{
public:
    static Author parseAuthorInfo(const QJsonObject authorInfo);
};

#endif // CHANNELPARSER_H
