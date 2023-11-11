#include "playlistmodel.h"
#include <QDebug>
#include <QSettings>
#include <repositories/authorrepository.h>

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

    const SearchResult &result = _items.at(index.row());
    if (std::holds_alternative<std::unique_ptr<Video>>(result)) {
        Video *video = std::get<std::unique_ptr<Video>>(result).get();
        switch (role) {
        case IdRole:
            return video->videoId;
        case TypeRole:
            return VideoType;
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
            return video->timestamp;
        case UrlRole:
            return video->url;
        default:
            return QVariant();
        }
    } else if (std::holds_alternative<Author>(result)) {
        Author author = std::get<Author>(result);
        switch (role) {
        case IdRole:
            return author.authorId;
        case TypeRole:
            return ChannelType;
        case TitleRole:
            return author.name;
        case ThumbnailRole:
            return author.bestAvatar.url;
        case DescriptionRole:
            return author.description;
        case UrlRole:
            return author.url;
        case AuthorRole:
            return QVariant::fromValue(author);
        default:
            return QVariant();
        }
    } else {
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
    if (category == "Subscriptions") {
        loadSubscriptions();
    } else {
        _jsProcessHelper.asyncScrapeTrending(category, country);
    }
}

void PlaylistModel::loadChannelVideos(QString channelId)
{
    _jsProcessHelper.asyncLoadChannelVideos(channelId);
}

void PlaylistModel::loadSubscriberVideos(QString channelId)
{
    beginResetModel();
    AuthorRepository authorRepository;
    Author author = authorRepository.getOneByChannelId(channelId);
    auto videos = _videoRepository.getChannelVideos(author.id);
    _items.clear();
    std::move(videos.begin(), videos.end(), std::back_inserter(_items));
    endResetModel();
}

void PlaylistModel::continueChannelVideos()
{
    _jsProcessHelper.asyncContinueChannelVideos();
}

QString PlaylistModel::getIdAt(int index)
{
    Q_ASSERT(std::holds_alternative<std::unique_ptr<Video>>(_items.at(index)));
    Q_ASSERT(std::get<std::unique_ptr<Video>>(_items.at(index)));

    return std::get<std::unique_ptr<Video>>(_items.at(index))->videoId;
}

void PlaylistModel::loadSubscriptions()
{
    beginResetModel();
    _items.clear();
    auto subscriptions = _videoRepository.getSubscriptions();
    std::move(subscriptions.begin(), subscriptions.end(), std::back_inserter(_items));
    endResetModel();
}

void PlaylistModel::loadUnwatchedSubscriptions()
{
    beginResetModel();
    _items.clear();
    auto subscriptions = _videoRepository.getUnwatchedSubscriptions();
    std::move(subscriptions.begin(), subscriptions.end(), std::back_inserter(_items));
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
    SearchResults videos = _jsProcessHelper.getChannelVideos();
    if (continuation) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + videos.size()-1);
        std::move(videos.begin(), videos.end(), std::back_inserter(_items));
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
    roles[TypeRole] = "elementType";
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
