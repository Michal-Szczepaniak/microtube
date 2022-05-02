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
    Author *a = _authorRepository.getOneByChannelId(channelId);
    if (a != nullptr) {
        return a->subscribed;
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
    Author *a = _authorRepository.getOneByChannelId(channelId);
    if (a == nullptr) {
        _jsProcessHelper.asyncGetChannelInfo(channelId);

        _pendingAction.channelId = channelId;
        _pendingAction.subscribe = subscribed;
    } else {
        a->subscribed = subscribed;
        _authorRepository.update(a->id);
    }
}

void ChannelHelper::gotChannelInfo()
{
    Author a = _jsProcessHelper.getChannelInfo();

    if (_pendingAction.channelId != "") {
        a.subscribed = _pendingAction.subscribe;
        _authorRepository.put(&a);

        _pendingAction.channelId = "";
    } else {
        _channelInfo = a;

        emit channelInfoChanged();
    }
}
