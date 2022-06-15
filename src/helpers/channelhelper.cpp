#include "channelhelper.h"

#include <QQmlEngine>

ChannelHelper::ChannelHelper(QObject *parent)
{
    connect(&_jsProcessHelper, &JSProcessHelper::gotChannelInfo, this, &ChannelHelper::gotChannelInfo);
}

void ChannelHelper::loadChannelInfo(QString channelId)
{
    _jsProcessHelper.asyncGetChannelInfo(channelId);
}

void ChannelHelper::subscribe(QString channelId)
{
    setSubscribe(channelId, true);
}

void ChannelHelper::unsubscribe(QString channelId)
{
    setSubscribe(channelId, false);
}

bool ChannelHelper::isSubscribed(QString channelId)
{
    Author a = _authorRepository.getOneByChannelId(channelId);
    if (a.id != -1) {
        return a.subscribed;
    } else {
        return false;
    }
}

Author ChannelHelper::getChannelInfo()
{
    return _channelInfo;
}

void ChannelHelper::setSubscribe(QString channelId, bool subscribed)
{
    Author a = _authorRepository.getOneByChannelId(channelId);
    if (a.id == -1) {
        a =_jsProcessHelper.fetchChannelInfo(channelId);
        a.subscribed = subscribed;
        _authorRepository.put(a);
    } else {
        a.subscribed = subscribed;
        _authorRepository.update(a);
    }
}

void ChannelHelper::gotChannelInfo()
{
    Author a = _jsProcessHelper.getChannelInfo();

    _channelInfo = a;

    emit channelInfoChanged();
}
