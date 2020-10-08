/* $BEGIN_LICENSE

This file is part of Minitube.
Copyright 2009, Flavio Tordini <flavio.tordini@gmail.com>
Copyright 2018, Michał Szczepaniak <m.szczepaniak.000@gmail.com>

Minitube is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Minitube is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Minitube.  If not, see <http://www.gnu.org/licenses/>.

$END_LICENSE */

#ifndef YTCHANNEL_H
#define YTCHANNEL_H

//#include <QtWidgets>
#include <QtNetwork>
#include <QPixmap>

class YTChannel : public QObject {

    Q_OBJECT
    Q_PROPERTY(int notifyCount READ getNotifyCount NOTIFY notifyCountChanged)
    Q_PROPERTY(QString channelId READ getChannelId NOTIFY channelIdChanged)
    Q_PROPERTY(QString userName READ getUserName NOTIFY userNameChanged)
    Q_PROPERTY(QString displayName READ getDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString description READ getDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString countryCode READ getCountryCode NOTIFY countryCodeChanged)
    Q_PROPERTY(bool isSubscribed READ getIsSubscribed NOTIFY isSubscribedChanged)
    Q_PROPERTY(QString viewCount READ getFormattedViewCount NOTIFY viewCountChanged)
    Q_PROPERTY(QString subscriberCount READ getFormattedSubscriberCount NOTIFY subscriberCountChanged)
    Q_PROPERTY(QString bannerMobileImageUrl READ getBannerMobileImageUrl NOTIFY bannerMobileImageUrlChanged)
    Q_PROPERTY(QString thumbnail READ getThumbnailLocation NOTIFY thumbnailLocationChanged)
    Q_PROPERTY(QString thumbnailUrl READ getThumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString webpage READ getWebpage NOTIFY webpageChanged)
    Q_PROPERTY(QDateTime publishedAt READ getPublishedAt NOTIFY publishedAtChanged)

public:
    static YTChannel* forId(const QString &channelId);
    static YTChannel* fromId(const QString &channelId);
    static void subscribe(const QString &channelId);
    static void unsubscribe(const QString &channelId);
    static bool isSubscribed(const QString &channelId);

    int getId() { return id; }
    void setId(int id) { this->id = id; }

    uint getChecked() { return checked; }
    void updateChecked();

    uint getWatched() const { return watched; }
    void setWatched(uint watched) { this->watched = watched; }

    int getNotifyCount() const { return notifyCount; }
    void setNotifyCount(int count) { notifyCount = count; emit this->notifyCountChanged(); }
    void storeNotifyCount(int count);
    bool updateNotifyCount();

    QString getChannelId() const { return channelId; }
    QString getUserName() const { return userName; }
    QString getDisplayName() const { return displayName; }
    QString getDescription() const { return description; }
    QString getCountryCode() const { return countryCode; }

    void loadThumbnail();
    const QString & getThumbnailDir();
    QString getThumbnailLocation();
    const QPixmap & getThumbnail() { return thumbnail; }
    const QString getThumbnailUrl() { return thumbnailUrl; }
    QString & getWebpage();

    QString latestVideoId();
    bool getIsSubscribed();
    Q_INVOKABLE void subscribe();

    static const QHash<QString, YTChannel*> &getCachedChannels() { return cache; }

    QString getViewCount() const;
    QString getFormattedViewCount() const;
    QString getSubscriberCount() const;
    QString getFormattedSubscriberCount() const;
    QString getBannerMobileImageUrl() const;
    QDateTime getPublishedAt() const;

public slots:
    void updateWatched();
    Q_INVOKABLE void unsubscribe();

signals:
    void infoLoaded();
    void thumbnailLoaded();
    void error(QString message);
    void notifyCountChanged();
    void channelIdChanged();
    void userNameChanged();
    void displayNameChanged();
    void descriptionChanged();
    void countryCodeChanged();
    void viewCountChanged();
    void subscriberCountChanged();
    void isSubscribedChanged();
    void bannerMobileImageUrlChanged();
    void thumbnailUrlChanged();
    void thumbnailLocationChanged();
    void webpageChanged();
    void publishedAtChanged();

private slots:
    void parseResponse(const QByteArray &bytes);
    void requestError(const QString &message);
    void storeThumbnail(const QByteArray &bytes);

private:
    YTChannel(const QString &channelId, QObject *parent = nullptr);
    void maybeLoadfromAPI();
    void storeInfo();

    static QHash<QString, YTChannel*> cache;

    int id;
    QString channelId;
    QString userName;
    QString displayName;
    QString description;
    QString countryCode;
    QString viewCount;
    QString subscriberCount;
    QString bannerMobileImageUrl;
    QString webpage;
    QDateTime publishedAt;

    QString thumbnailUrl;
    QPixmap thumbnail;
    bool loadingThumbnail;

    int notifyCount;
    uint checked;
    uint watched;
    uint loaded;
    bool loading;
};

// This is required in order to use QPointer<YTUser> as a QVariant
typedef QPointer<YTChannel> YTChannelPointer;
Q_DECLARE_METATYPE(YTChannelPointer)

#endif // YTCHANNEL_H
