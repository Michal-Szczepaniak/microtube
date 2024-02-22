#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <memory>
#include <vector>
#include "src/repositories/videorepository.h"
#include "src/managers/jsprocessmanager.h"

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString country READ getCountry WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(bool safeSearch READ getSafeSearch WRITE setSafeSearch NOTIFY safeSearchChanged)
    Q_PROPERTY(qint32 currentVideoIndex READ getCurrentVideoIndex WRITE setCurrentVideoIndex NOTIFY currentVideoIndexChanged)
    Q_PROPERTY(bool hasLastSearch READ hasLastSearch NOTIFY lastSearchChanged)
    Q_PROPERTY(int uploadDateFilter READ getUploadDateFilter WRITE setUploadDateFilter NOTIFY lastSearchChanged)
    Q_PROPERTY(int typeFilter READ getTypeFilter WRITE setTypeFilter NOTIFY lastSearchChanged)
    Q_PROPERTY(int durationFilter READ getDurationFilter WRITE setDurationFilter NOTIFY lastSearchChanged)
    Q_PROPERTY(int sortBy READ getSortBy WRITE setSortBy NOTIFY lastSearchChanged)
    Q_ENUMS(DataTypes)
public:
    explicit PlaylistModel(QObject *parent = nullptr);

    enum DataRoles {
        IdRole = Qt::UserRole,
        TypeRole,
        TitleRole,
        DurationRole,
        ThumbnailRole,
        AlternativeThumbnailRole,
        DescriptionRole,
        AuthorRole,
        IsUpcomingRole,
        IsLiveRole,
        ViewsRole,
        PublishedRole,
        UrlRole,
        IsSubscribedRole,
        WatchedRole
    };

    enum DataTypes {
        VideoType,
        ChannelType,
        PlaylistType
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void searchAgain();
    Q_INVOKABLE void loadRecommendedVideos(QString query);
    Q_INVOKABLE void loadCategory(QString category);
    Q_INVOKABLE void loadChannelVideos(QString channelId, int type = Search::Channel);
    Q_INVOKABLE void loadSubscriberVideos(QString channelId);
    Q_INVOKABLE void continueChannelVideos();
    Q_INVOKABLE QString getIdAt(int index);
    Q_INVOKABLE void loadSubscriptions();
    Q_INVOKABLE void loadUnwatchedSubscriptions();
    Q_INVOKABLE void loadPlaylist(QString id);
    Q_INVOKABLE void copyOtherModel(PlaylistModel* model);
    Q_INVOKABLE void addVideo(QString id, quint8 retryCount = 0);
    Q_INVOKABLE void removeVideo(QString id);
    Q_INVOKABLE bool hasVideo(QString id) const;
    Q_INVOKABLE void clear();
    Q_INVOKABLE bool getSafeSearch() const;
    Q_INVOKABLE void setSafeSearch(bool safeSearch);
    Q_INVOKABLE QString getCountry() const;
    Q_INVOKABLE void setCountry(QString country);
    std::optional<Search> getSearch();
    void executeSearch();
    qint32 getCurrentVideoIndex() const;
    void setCurrentVideoIndex(qint32 currentVideoIndex);
    bool hasLastSearch() const;
    int getSortBy() const;
    void setSortBy(int value);
    int getUploadDateFilter() const;
    void setUploadDateFilter(int value);
    int getTypeFilter() const;
    void setTypeFilter(int value);
    int getDurationFilter()  const;
    void setDurationFilter(int value);

signals:
    void currentVideoStreamUrlChanged();
    void currentAudioStreamUrlChanged();
    void currentVideoChanged();
    void safeSearchChanged();
    void countryChanged();
    void currentVideoIndexChanged();
    void lastSearchChanged();

public slots:
    void searchDone(bool continuation);
    void gotTrendingVideos();
    void gotRecommendedVideos();
    void gotChannelVideos(bool continuation);
    void gotVideoInfo(QHash<int, QString> formats);
    void gotPlaylist();

protected:
    QHash<int, QByteArray> roleNames() const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    Search createNewSearch();

private:
    std::optional<Search> _lastSearch;
    SearchResults _items;
    JSProcessManager _jsProcessManager;
    quint32 _maxDefinition;
    VideoRepository _videoRepository;
    qint32 _currentVideoIndex;
};

#endif // PLAYLISTMODEL_H
