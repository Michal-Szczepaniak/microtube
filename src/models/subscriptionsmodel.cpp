#include "subscriptionsmodel.h"

SubscriptionsModel::SubscriptionsModel(QObject *parent) : QAbstractListModel(parent)
{

}

void SubscriptionsModel::loadSubscriptionsList()
{
    beginResetModel();
    _subscriptions = _authorRepository.getSubscriptionsWithUnwatchedCount();
    endResetModel();
}

void SubscriptionsModel::refresh()
{
    _subscriptions = _authorRepository.getSubscriptionsWithUnwatchedCount();

    emit dataChanged(createIndex(0, 0), createIndex(_subscriptions.count()-1, _subscriptions.count()-1));
}

void SubscriptionsModel::markAllAsWatched()
{
    _videoRepository.setAllWatchedStatus(true);

    loadSubscriptionsList();
}

int SubscriptionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _subscriptions.count();
}

QVariant SubscriptionsModel::data(const QModelIndex &index, int role) const
{
    if (rowCount() <= 0 || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    auto author = _subscriptions[index.row()];
    switch (role) {
    case SubscriptionRoles::IdRole:
        return index.row();
    case SubscriptionRoles::DatabaseIdRole:
        return author.id;
    case SubscriptionRoles::AuthorIdRole:
        return author.authorId;
    case SubscriptionRoles::NameRole:
        return author.name;
    case SubscriptionRoles::AvatarRole:
        return author.bestAvatar.url;
    case SubscriptionRoles::IgnoredRole:
        return author.ignored;
    case SubscriptionRoles::UnwatchedRole:
        return author.ignored ? 0 : author.unwatchedVideosCount;
    default:
        return QVariant();
    }
}

bool SubscriptionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (rowCount() <= 0 || index.row() < 0 || index.row() >= rowCount()) return false;

    Author &author = _subscriptions[index.row()];
    switch (role) {
    case SubscriptionRoles::IgnoredRole:
        author.ignored = !author.ignored;
        _authorRepository.update(author);
        emit dataChanged(index, index, {UnwatchedRole});
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> SubscriptionsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]         = "id";
    roles[DatabaseIdRole] = "databaseId";
    roles[AuthorIdRole]   = "authorId";
    roles[NameRole]       = "name";
    roles[AvatarRole]     = "avatar";
    roles[IgnoredRole]    = "ignored";
    roles[UnwatchedRole]  = "unwatchedCount";
    return roles;
}
