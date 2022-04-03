#ifndef SUBSCRIPTIONSHELPER_H
#define SUBSCRIPTIONSHELPER_H

#include <QObject>

class SubscriptionsHelper : public QObject
{
    Q_OBJECT
public:
    explicit SubscriptionsHelper(QObject *parent = nullptr);

    Q_INVOKABLE bool isSubscribed(QString channelId);
    Q_INVOKABLE void subscribe(QString channelId);
    Q_INVOKABLE void unsubscribe(QString channelId);
signals:

};

#endif // SUBSCRIPTIONSHELPER_H
