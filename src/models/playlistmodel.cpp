#include "playlistmodel.h"
#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QUrlQuery>
#include <repositories/authorrepository.h>

PlaylistModel::PlaylistModel(QObject *parent) : QAbstractListModel(parent), _currentVideoIndex(0), _busy(false)
{
    connect(&_jsProcessManager, &JSProcessManager::searchFinished, this, &PlaylistModel::searchDone);
    connect(&_jsProcessManager, &JSProcessManager::gotTrendingVideos, this, &PlaylistModel::gotTrendingVideos);
    connect(&_jsProcessManager, &JSProcessManager::gotRecommendedVideos, this, &PlaylistModel::gotRecommendedVideos);
    connect(&_jsProcessManager, &JSProcessManager::gotChannelVideos, this, &PlaylistModel::gotChannelVideos);
    connect(&_jsProcessManager, &JSProcessManager::gotVideoInfo, this, &PlaylistModel::gotVideoInfo);
    connect(&_jsProcessManager, &JSProcessManager::gotPlaylist, this, &PlaylistModel::gotPlaylist);
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
        case IsSubscribedRole:
        {
            if (video->author.authorId.length() == 0) return false;

            AuthorRepository authorRepository;
            return authorRepository.has(video->author.authorId);
        }
        case WatchedRole:
        {
            if (video->videoId.length() == 0) return false;

            return _videoRepository.isWatched(video->videoId);
        }
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
    } else if (std::holds_alternative<Playlist>(result)) {
        Playlist playlist = std::get<Playlist>(result);
        switch (role) {
        case IdRole:
            return playlist.playlistId;
        case TypeRole:
            return PlaylistType;
        case TitleRole:
            return playlist.title;
        case ThumbnailRole:
            return playlist.bestThumbnail.url;
        case UrlRole:
            return playlist.url;
        case DurationRole:
            return playlist.length;
        case AuthorRole:
            return QVariant::fromValue(playlist.author);
        default:
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (rowCount() <= 0 || index.row() >= rowCount()) return false;

    const SearchResult &result = _items.at(index.row());
    if (std::holds_alternative<std::unique_ptr<Video>>(result)) {
        Video *video = std::get<std::unique_ptr<Video>>(result).get();
        AuthorRepository authorRepository;

        switch (role) {
        case WatchedRole:
        {
            if (video->videoId < 0) return false;

            Video* v = _videoRepository.getOneByVideoId(video->videoId);
            if (v == nullptr) {
                if (authorRepository.has(video->author.authorId)) {
                    _videoRepository.put(video);
                    v = _videoRepository.get(video->id);
                } else {
                    return false;
                }
            }

            v->watched = value.toBool();
            _videoRepository.update(v->id);
        }
            break;
        default:
            return false;
        }

        return true;
    }

    return false;
}

void PlaylistModel::search(QString query)
{
    QUrlQuery url(query);
    if (url.hasQueryItem("list")) {
        beginResetModel();
        _items.clear();
        loadPlaylist(url.queryItemValue("list"));
        endResetModel();
        return;
    } else if (url.hasQueryItem("v")) {
        beginResetModel();
        _items.clear();
        addVideo(url.queryItemValue("v"));
        endResetModel();
        return;
    }

    Search search = createNewSearch();
    search.query = query;
    search.type = Search::Query;
    _lastSearch = search;
    emit lastSearchChanged();
    executeSearch();
}

void PlaylistModel::searchAgain()
{
    executeSearch();
}

void PlaylistModel::loadRecommendedVideos(QString query)
{
    Search search = createNewSearch();
    search.query = query;
    search.type = Search::VideoInfo;
    _jsProcessManager.asyncLoadRecommendedVideos(search);
}

void PlaylistModel::loadCategory(QString category)
{
    if (category == "") return;

    if (category == "Subscriptions") {
        loadSubscriptions();
    } else {
        Search search = createNewSearch();
        search.query = category;
        search.type = Search::Category;
        search.language = "en";
        _lastSearch = search;
        emit lastSearchChanged();
        executeSearch();
    }
}

void PlaylistModel::loadChannelVideos(QString channelId, int type)
{
    Search search = createNewSearch();
    search.query = channelId;
    search.type = (Search::SearchType)type;
    _lastSearch = search;
    emit lastSearchChanged();
    executeSearch();
}

void PlaylistModel::loadSubscriberVideos(QString channelId)
{
    Search search = createNewSearch();
    search.query = channelId;
    search.type = Search::Subscriber;
    _lastSearch = search;
    emit lastSearchChanged();

    beginResetModel();
    _items.clear();
    executeSearch();
    endResetModel();
}

void PlaylistModel::continueChannelVideos()
{
    Search search = createNewSearch();
    search.type = _lastSearch->type;
    _lastSearch = search;
    emit lastSearchChanged();

    _jsProcessManager.asyncContinueChannelVideos(&_lastSearch.value());
}

QString PlaylistModel::getIdAt(int index)
{
    Q_ASSERT(std::holds_alternative<std::unique_ptr<Video>>(_items.at(index)));
    Q_ASSERT(std::get<std::unique_ptr<Video>>(_items.at(index)));

    return std::get<std::unique_ptr<Video>>(_items.at(index))->videoId;
}

void PlaylistModel::loadSubscriptions()
{
    Search search = createNewSearch();
    search.type = Search::Subscriptions;
    _lastSearch = search;
    emit lastSearchChanged();

    beginResetModel();
    _items.clear();
    executeSearch();
    endResetModel();
}

void PlaylistModel::loadUnwatchedSubscriptions()
{
    Search search = createNewSearch();
    search.type = Search::UnwatchedSubscriptions;
    _lastSearch = search;
    emit lastSearchChanged();

    beginResetModel();
    _items.clear();
    executeSearch();
    endResetModel();
}

void PlaylistModel::loadPlaylist(QString id)
{
    Search search = createNewSearch();
    search.type = Search::Playlist;
    search.query = id;
    _lastSearch = search;
    emit lastSearchChanged();

    executeSearch();
}

void PlaylistModel::copyOtherModel(PlaylistModel *model)
{
    if (model == nullptr || !model->getSearch().has_value()) return;

    _lastSearch = model->getSearch().value();
    emit lastSearchChanged();

    beginResetModel();
    _items.clear();
    executeSearch();
    endResetModel();
}

void PlaylistModel::addVideo(QString id, quint8 retryCount)
{
    Search search = createNewSearch();
    search.query = id;
    search.type = Search::VideoInfo;
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _items.emplace_back(_jsProcessManager.getBasicVideoInfo(search));
    endInsertRows();
}

void PlaylistModel::removeVideo(QString id)
{
    for (int i = 0; i < _items.size(); i++) {
        if (false == std::holds_alternative<std::unique_ptr<Video>>(_items[i])) continue;

        if (std::get<std::unique_ptr<Video>>(_items[i])->videoId == id) {
            beginRemoveRows(QModelIndex(), i, i);
            _items.erase(_items.begin() + i);
            endRemoveRows();
        }
    }
}

bool PlaylistModel::hasVideo(QString id) const
{
    for (const SearchResult &video : _items) {
        if (false == std::holds_alternative<std::unique_ptr<Video>>(video)) continue;

        if (std::get<std::unique_ptr<Video>>(video)->videoId == id) {
            return true;
        }
    }

    return false;
}

void PlaylistModel::clear()
{
    beginResetModel();
    _items.clear();
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

QString PlaylistModel::getCountry() const
{
    return QSettings().value("country", "US").toString();
}

void PlaylistModel::setCountry(QString country)
{
    QSettings().setValue("country", country);

    emit countryChanged();
}

void PlaylistModel::reverse()
{
    beginResetModel();
    std::reverse(_items.begin(), _items.end());
    endResetModel();
}

bool PlaylistModel::getCanReverse() const
{
    return _lastSearch->type == Search::Subscriber || _lastSearch->type == Search::Subscriptions || _lastSearch->type == Search::UnwatchedSubscriptions;
}

std::optional<Search> PlaylistModel::getSearch()
{
    return _lastSearch;
}

void PlaylistModel::executeSearch()
{
    setBusy(true);

    switch (_lastSearch->type) {
    case Search::Query:
        _jsProcessManager.asyncSearch(&_lastSearch.value());
        break;
    case Search::Category:
        _jsProcessManager.asyncGetTrending(&_lastSearch.value());
        break;
    case Search::Channel:
    case Search::ChannelShorts:
    case Search::ChannelLiveStreams:
    case Search::ChannelPlaylists:
        _jsProcessManager.asyncLoadChannelVideos(&_lastSearch.value());
        break;
    case Search::Subscriber:
    {
        AuthorRepository authorRepository;
        Author author = authorRepository.getOneByChannelId(_lastSearch->query);
        auto videos = _videoRepository.getChannelVideos(author.id);
        std::move(videos.begin(), videos.end(), std::back_inserter(_items));
        setBusy(false);
    }
        break;
    case Search::Subscriptions:
    {
        auto subscriptions = _videoRepository.getSubscriptions();
        std::move(subscriptions.begin(), subscriptions.end(), std::back_inserter(_items));
        setBusy(false);
    }
        break;
    case Search::UnwatchedSubscriptions:
    {
        auto subscriptions = _videoRepository.getUnwatchedSubscriptions();
        std::move(subscriptions.begin(), subscriptions.end(), std::back_inserter(_items));
        setBusy(false);
    }
        break;
    case Search::Playlist:
        _jsProcessManager.asyncLoadPlaylist(&_lastSearch.value());
        break;
    case Search::VideoInfo:
        break;
    }
}

qint32 PlaylistModel::getCurrentVideoIndex() const
{
    return _currentVideoIndex;
}

void PlaylistModel::setCurrentVideoIndex(qint32 currentVideoIndex)
{
    _currentVideoIndex = currentVideoIndex;

    emit currentVideoIndexChanged();
}

bool PlaylistModel::hasLastSearch() const
{
    if (!_lastSearch.has_value()) return false;

    return _lastSearch->type == Search::Query;
}

int PlaylistModel::getSortBy() const
{
    if (!_lastSearch.has_value()) return 0;

    return _lastSearch->sortBy;
}

void PlaylistModel::setSortBy(int value)
{
    if (!_lastSearch.has_value()) return;

    _lastSearch->sortBy = (Search::SortBy)value;

    emit lastSearchChanged();
}

int PlaylistModel::getUploadDateFilter() const
{
    if (!_lastSearch.has_value()) return 0;

    return _lastSearch->uploadDateFilter;
}

void PlaylistModel::setUploadDateFilter(int value)
{
    if (!_lastSearch.has_value()) return;

    _lastSearch->uploadDateFilter = (Search::UploadDateFilter)value;

    emit lastSearchChanged();
}

int PlaylistModel::getTypeFilter() const
{
    if (!_lastSearch.has_value()) return 0;

    return _lastSearch->typeFilter;
}

void PlaylistModel::setTypeFilter(int value)
{
    if (!_lastSearch.has_value()) return;

    _lastSearch->typeFilter = (Search::TypeFilter)value;

    emit lastSearchChanged();
}

int PlaylistModel::getDurationFilter() const
{
    if (!_lastSearch.has_value()) return 0;

    return _lastSearch->durationFilter;
}

void PlaylistModel::setDurationFilter(int value)
{
    if (!_lastSearch.has_value()) return;

    _lastSearch->durationFilter = (Search::DurationFilter)value;

    emit lastSearchChanged();
}

bool PlaylistModel::isBusy() const
{
    return _busy;
}

void PlaylistModel::setBusy(bool busy)
{
    _busy = busy;

    emit busyChanged();
}

void PlaylistModel::searchDone(bool continuation)
{
    SearchResults videos = _jsProcessManager.getSearchVideos();
    if (continuation) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + videos.size()-1);
        std::move(videos.begin(), videos.end(), std::back_inserter(_items));
        endInsertRows();
    } else {
        beginResetModel();
        _items = move(videos);
        endResetModel();
    }

    setBusy(false);
}

void PlaylistModel::gotTrendingVideos()
{
    beginResetModel();
    _items = _jsProcessManager.getTrendingVideos();
    endResetModel();

    setBusy(false);
}

void PlaylistModel::gotRecommendedVideos()
{
    beginResetModel();
    _items = _jsProcessManager.getRecommendedVideos();
    endResetModel();

    setBusy(false);
}

void PlaylistModel::gotChannelVideos(bool continuation)
{
    SearchResults videos = _jsProcessManager.getChannelVideos();
    if (continuation) {
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + videos.size()-1);
        std::move(videos.begin(), videos.end(), std::back_inserter(_items));
        endInsertRows();
    } else {
        beginResetModel();
        _items = move(videos);
        endResetModel();
    }

    setBusy(false);
}

void PlaylistModel::gotVideoInfo(QHash<int, QString> formats)
{
    std::unique_ptr<Video> video = _jsProcessManager.getVideoInfo();

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _items.emplace_back(move(video));
    endInsertRows();

    setBusy(false);
}

void PlaylistModel::gotPlaylist()
{
    SearchResults videos = _jsProcessManager.getPlaylistVideos();

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + videos.size()-1);
    std::move(videos.begin(), videos.end(), std::back_inserter(_items));
    endInsertRows();

    setBusy(false);
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
    roles[IsSubscribedRole] = "isSubscribed";
    roles[WatchedRole] = "watched";
    return roles;
}

bool PlaylistModel::canFetchMore(const QModelIndex &parent) const
{
    qDebug() << "Can fetch more: " << ((_lastSearch.has_value() && (
                                            (_lastSearch->type == Search::Query && _jsProcessManager.hasSearchContinuation()) ||
                                            (_lastSearch->type == Search::Channel && _jsProcessManager.hasVideosContinuation()) ||
                                            (_lastSearch->type == Search::Playlist && _jsProcessManager.hasPlaylistContinuation())
                                            )) ? "yes" : "no");

    return _lastSearch.has_value() && (
               (_lastSearch->type == Search::Query && _jsProcessManager.hasSearchContinuation()) ||
               (_lastSearch->type == Search::Channel && _jsProcessManager.hasVideosContinuation()) ||
               (_lastSearch->type == Search::Playlist && _jsProcessManager.hasPlaylistContinuation())
               );
}

void PlaylistModel::fetchMore(const QModelIndex &parent)
{
    if (!_lastSearch.has_value()) return;

    setBusy(true);

    switch (_lastSearch->type) {
    case Search::Query:
        _jsProcessManager.asyncContinueSearch(&_lastSearch.value());
        break;
    case Search::Channel:
    case Search::ChannelShorts:
    case Search::ChannelLiveStreams:
    case Search::ChannelPlaylists:
        _jsProcessManager.asyncContinueChannelVideos(&_lastSearch.value());
        break;
    case Search::Playlist:
        _jsProcessManager.asyncContinuePlaylist(&_lastSearch.value());
        break;
    default:
        break;
    }
}

Search PlaylistModel::createNewSearch()
{
    Search search;
    search.country = getCountry();
    search.safeSearch = getSafeSearch();
    return search;
}
