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

#include "channelmodel.h"
#include "ytchannel.h"
#include "database.h"

static const int channelOffset = 2;

ChannelModel::ChannelModel(QObject *parent) :
    QAbstractListModel(parent),
    hoveredRow(-1) { }


int ChannelModel::rowCount(const QModelIndex &) const {
    return channels.isEmpty() ? 0 : channelOffset + channels.size();
}

QVariant ChannelModel::data(const QModelIndex &index, int role) const {
    switch (role) {

    case ChannelModel::ItemTypeRole:
        return typeForIndex(index);

    case ChannelModel::DataObjectRole:
        if (typeForIndex(index) == ChannelModel::ItemChannel)
            return QVariant::fromValue(channelForIndex(index));
        break;

    case ChannelModel::NotifyCountRole:
        if (typeForIndex(index) == ChannelModel::ItemChannel)
            return channelForIndex(index)->getNotifyCount();
        else if (index.row() == 0 || index.row() == 1)
            return INT_MAX;
        break;

    case ChannelModel::HoveredItemRole:
        return hoveredRow == index.row();

    case Qt::StatusTipRole:
        if (typeForIndex(index) == ChannelModel::ItemChannel)
            return channelForIndex(index)->getDescription();
        break;

    case ChannelModel::UsernameRole:
        if (typeForIndex(index) == ChannelModel::ItemChannel)
            return channelForIndex(index)->getDisplayName();
        else if (index.row() == 0)
            return "All videos";
        else if (index.row() == 1)
            return "Unwatched videos";
        break;

    case ChannelModel::ThumbnailRole:
        if (typeForIndex(index) == ChannelModel::ItemChannel)
            return channelForIndex(index)->getThumbnailLocation();
        else if (index.row() == 0)
            return "/usr/share/microtube/qml/resources/images/channels.png";
        else if (index.row() == 1)
            return "/usr/share/microtube/qml/resources/images/unwatched.png";
        break;

    }

    return QVariant();
}

YTChannel* ChannelModel::channelForIndex(const QModelIndex &index) const {
    const int row = index.row();
    if (row < channelOffset) return nullptr;
    return channels.at(index.row() - channelOffset);
}

YTChannel* ChannelModel::channelForIndex(int index) const {
    if (index < channelOffset) return nullptr;
    return channels.at(index - channelOffset);
}

ChannelModel::ItemTypes ChannelModel::typeForIndex(const QModelIndex &index) const {
    switch (index.row()) {
    case 0:
        return ChannelModel::ItemAggregate;
    case 1:
        return ChannelModel::ItemUnwatched;
    default:
        return ChannelModel::ItemChannel;
    }
}

ChannelModel::ItemTypes ChannelModel::typeForIndex(int index) const {
    switch (index) {
    case 0:
        return ChannelModel::ItemAggregate;
    case 1:
        return ChannelModel::ItemUnwatched;
    default:
        return ChannelModel::ItemChannel;
    }
}

void ChannelModel::setQuery(const QString &query, const QSqlDatabase &db) {
    beginResetModel();
    channels.clear();
    sqlError = QSqlError();

    QSqlQuery q(db);
    q.prepare(query);
    bool success = q.exec();
    if (!success) {
        qWarning() << q.lastQuery() << q.lastError().text();
        sqlError = q.lastError();
    }
    channels.reserve(q.size());
    while (q.next()) {
        YTChannel *channel = YTChannel::forId(q.value(0).toString());
        connect(channel, SIGNAL(thumbnailLoaded()), SLOT(updateSender()), Qt::UniqueConnection);
        connect(channel, SIGNAL(notifyCountChanged()), SLOT(updateSender()), Qt::UniqueConnection);
        connect(channel, SIGNAL(destroyed(QObject *)), SLOT(removeChannel(QObject *)), Qt::UniqueConnection);
        channels << channel;
    }
    channels.squeeze();
    endResetModel();
}

QSqlError ChannelModel::lastError() const {
    return sqlError;
}

void ChannelModel::updateSender() {
    YTChannel *channel = static_cast<YTChannel*>(sender());
    if (!channel) {
        qWarning() << "Cannot get sender" << __PRETTY_FUNCTION__;
        return;
    }
    updateChannel(channel);
}

void ChannelModel::updateChannel(YTChannel *channel) {
    int row = channels.indexOf(channel);
    if (row == -1) return;
    row += channelOffset;
    QModelIndex i = createIndex(row, 0);
    emit dataChanged(i, i);
}

void ChannelModel::updateUnwatched() {
    QModelIndex i = createIndex(1, 0);
    emit dataChanged(i, i);
}

void ChannelModel::removeChannel(QObject *obj) {
    YTChannel *channel = static_cast<YTChannel*>(obj);
    // qWarning() << "channel" << channel << obj << obj->metaObject()->className();
    if (!channel) return;

    int row = channels.indexOf(channel);
    if (row == -1) return;

    int position = row + channelOffset;
    beginRemoveRows(QModelIndex(), position, position+1);
    channels.removeAt(row);
    endRemoveRows();
}

void ChannelModel::setHoveredRow(int row) {
    int oldRow = hoveredRow;
    hoveredRow = row;
    emit dataChanged( createIndex( oldRow, 0 ), createIndex( oldRow, 0 ) );
    emit dataChanged( createIndex( hoveredRow, 0 ), createIndex( hoveredRow, 0 ) );
}

void ChannelModel::clearHover() {
    emit dataChanged( createIndex( hoveredRow, 0 ), createIndex( hoveredRow, 0 ) );
    hoveredRow = -1;
}

QHash<int, QByteArray> ChannelModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ItemTypeRole] = "itemType";
    roles[DataObjectRole] = "channel";
    roles[NotifyCountRole] = "notifyCount";
    roles[Qt::StatusTipRole] = "description";
    roles[UsernameRole] = "username";
    roles[ThumbnailRole] = "thumbnail";
    return roles;
}

void ChannelModel::unsubscribe(int index) {
    YTChannel::unsubscribe(channelForIndex(index)->getChannelId());
    removeChannel(channelForIndex(index));
}

void ChannelModel::updateQuery() {
    QString sql = "select user_id from subscriptions";

    sql += " order by added desc";

    setQuery(sql, Database::instance().getConnection());
}
