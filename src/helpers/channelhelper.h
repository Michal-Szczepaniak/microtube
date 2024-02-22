#ifndef CHANNELHELPER_H
#define CHANNELHELPER_H

#include <QObject>
#include <memory>
#include "src/repositories/authorrepository.h"
#include "src/services/subscriptionsaggregator.h"
#include "src/managers/jsprocessmanager.h"

class ChannelHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Author channelInfo READ getChannelInfo NOTIFY channelInfoChanged)
public:
    explicit ChannelHelper(QObject *parent = nullptr);

    Q_INVOKABLE void loadChannelInfo(QString channelId);
    Q_INVOKABLE void subscribe(QString channelId);
    Q_INVOKABLE void unsubscribe(QString channelId);
    Q_INVOKABLE void unsubscribeId(int id);
    Q_INVOKABLE bool isSubscribed(QString channelId);
    Author getChannelInfo();
    void setSubscribe(QString channelId, bool subscribed);

signals:
    void channelInfoChanged();

public slots:
    void gotChannelInfo();

private:
    JSProcessManager _jsProcessHelper;
    Author _channelInfo;
    AuthorRepository _authorRepository;
};

#endif // CHANNELHELPER_H
