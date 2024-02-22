#include "channelhelper.h"

#include <QQmlEngine>
#include <QSettings>

ChannelHelper::ChannelHelper(QObject *parent)
{
    connect(&_jsProcessHelper, &JSProcessManager::gotChannelInfo, this, &ChannelHelper::gotChannelInfo);
}

void ChannelHelper::loadChannelInfo(QString channelId)
{
    Search search;
    search.country = QSettings().value("country", false).toString();
    search.safeSearch = QSettings().value("safeSearch", false).toBool();
    search.query = channelId;

    _jsProcessHelper.asyncGetChannelInfo(&search);
}

void ChannelHelper::subscribe(QString channelId)
{
    setSubscribe(channelId, true);
}

void ChannelHelper::unsubscribe(QString channelId)
{
    setSubscribe(channelId, false);
}

void ChannelHelper::unsubscribeId(int id)
{
    Author a = _authorRepository.get(id);
    if (a.id != -1) {
        a.subscribed = false;
        _authorRepository.update(a);
    }
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
        Search search;
        search.country = QSettings().value("country", false).toString();
        search.safeSearch = QSettings().value("safeSearch", false).toBool();
        search.query = channelId;
        a =_jsProcessHelper.fetchChannelInfo(&search);
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
