#include "subscriptionshelper.h"
#include <QSettings>

SubscriptionsHelper::SubscriptionsHelper(QObject *parent) : QObject(parent)
{

}

bool SubscriptionsHelper::isSubscribed(QString channelId)
{
    return QSettings().value(channelId, false).toBool();
}

void SubscriptionsHelper::subscribe(QString channelId)
{
    QSettings settings;
    QStringList subscribedChannels = settings.value("subscriptions").toStringList();
    settings.setValue(channelId, true);
    if (!subscribedChannels.contains(channelId)) subscribedChannels.append(channelId);
    settings.setValue("subscriptions", subscribedChannels);
}

void SubscriptionsHelper::unsubscribe(QString channelId)
{
    QSettings settings;
    QStringList subscribedChannels = settings.value("subscriptions").toStringList();
    settings.setValue(channelId, false);
    if (subscribedChannels.contains(channelId)) subscribedChannels.removeOne(channelId);
    settings.setValue("subscriptions", subscribedChannels);
}
