#include "playlistmodel.h"
#include <QDebug>
#include <QSettings>

PlaylistModel::PlaylistModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(&_jsProcessHelper, &JSProcessHelper::searchFinished, this, &PlaylistModel::searchDone);
    connect(&_jsProcessHelper, &JSProcessHelper::gotTrendingVideos, this, &PlaylistModel::gotTrendingVideos);
    connect(&_jsProcessHelper, &JSProcessHelper::gotRecommendedVideos, this, &PlaylistModel::gotRecommendedVideos);
    connect(&_jsProcessHelper, &JSProcessHelper::gotChannelVideos, this, &PlaylistModel::gotChannelVideos);
}


int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return _items.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (rowCount() <= 0 || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    Video* video = _items.at(index.row()).get();
    switch (role) {
    case IdRole:
        return video->videoId;
    case TitleRole:
        return video->title;
    case DurationRole:
        return video->duration;
    case ThumbnailRole:
        if (video->thumbnails.contains(Thumbnail::HD))
            return video->thumbnails[Thumbnail::HD].url;
        else
            return video->thumbnails[Thumbnail::SD].url;
    case AlternativeThumbnailRole:
        return video->thumbnails[Thumbnail::SD].url;
    case DescriptionRole:
        return video->description;
    case AuthorRole:
        return QVariant::fromValue(video->author);
    case IsUpcomingRole:
        return video->isUpcoming;
    case IsLiveRole:
        return video->isLive;
    case ViewsRole:
        return video->views;
    case PublishedRole:
        return video->uploadedAt;
    case UrlRole:
        return video->url;
    default:
        return QVariant();
    }
}

void PlaylistModel::search(QString query)
{
    std::unique_ptr<Search> search(new Search());
    search->query = query;
    search->safeSearch = QSettings().value("safeSearch", false).toBool();
    _lastSearch = move(search);
    _jsProcessHelper.asyncSearch(_lastSearch.get());
}

void PlaylistModel::loadRecommendedVideos(QString query)
{
    _jsProcessHelper.asyncLoadRecommendedVideos(query);
}

void PlaylistModel::loadCategory(QString category, QString country)
{
    _jsProcessHelper.asyncScrapeTrending(category, country);
}

void PlaylistModel::loadChannelVideos(QString channelId)
{
    _jsProcessHelper.asyncLoadChannelVideos(channelId);
}

QString PlaylistModel::getIdAt(int index)
{
    Q_ASSERT(_items.at(index));

    return _items.at(index)->videoId;
}

void PlaylistModel::loadSubscriptions()
{
    beginResetModel();
    _items = _videoRepository.getSubscriptions();
    endResetModel();
}

void PlaylistModel::loadUnwatchedSubscriptions()
{
    beginResetModel();
    _items = _videoRepository.getUnwatchedSubscriptions();
    endResetModel();
}

bool PlaylistModel::getSafeSearch() const
{
    return QSettings().value("safeSearch", false).toBool();
}

void PlaylistModel::setSafeSearch(bool safeSearch)
{
    QSettings().setValue("safeSearch", safeSearch);

    emit safeSearchChanged();
}

void PlaylistModel::searchDone(bool continuation)
{
    if (continuation) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + _lastSearch->items.size()-1);
        std::move(_lastSearch->items.begin(), _lastSearch->items.end(), std::back_inserter(_items));
        endInsertRows();
    } else {
        beginResetModel();
        _items = move(_lastSearch->items);
        endResetModel();
    }
}

void PlaylistModel::gotTrendingVideos()
{
    beginResetModel();
    _items = _jsProcessHelper.getTrendingVideos();
    endResetModel();
}

void PlaylistModel::gotRecommendedVideos()
{
    beginResetModel();
    _items = _jsProcessHelper.getRecommendedVideos();
    endResetModel();
}

void PlaylistModel::gotChannelVideos(bool continuation)
{
    std::vector<std::unique_ptr<Video>> videos = _jsProcessHelper.getChannelVideos();
    if (continuation) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + videos.size()-1);
        std::move(videos.begin(), videos.end(), std::back_inserter(videos));
        endInsertRows();
    } else {
        beginResetModel();
        _items = move(videos);
        endResetModel();
    }
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[DurationRole] = "duration";
    roles[ThumbnailRole] = "thumbnail";
    roles[AlternativeThumbnailRole] = "altThumbnail";
    roles[DescriptionRole] = "description";
    roles[AuthorRole] = "author";
    roles[IsUpcomingRole] = "isUpcoming";
    roles[IsLiveRole] = "isLive";
    roles[ViewsRole] = "views";
    roles[PublishedRole] = "published";
    roles[UrlRole] = "url";
    return roles;
}

bool PlaylistModel::canFetchMore(const QModelIndex &parent) const
{
    return _lastSearch != nullptr && !_lastSearch->continuation.isEmpty();
}

void PlaylistModel::fetchMore(const QModelIndex &parent)
{
    _jsProcessHelper.asyncContinueSearch(_lastSearch.get());
}
