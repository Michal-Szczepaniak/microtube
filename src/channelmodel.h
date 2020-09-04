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

#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QtCore>
#include <QtSql>

class YTChannel;

class ChannelModel : public QAbstractListModel {
    Q_OBJECT
public:
    ChannelModel(QObject *parent = 0);

    enum DataRoles {
        ItemTypeRole = Qt::UserRole,
        DataObjectRole,
        NotifyCountRole,
        HoveredItemRole,
        UsernameRole,
        ThumbnailRole
    };

    enum ItemTypes {
        ItemChannel = 1,
        ItemAggregate,
        ItemUnwatched
    };

    void setQuery(const QString &query, const QSqlDatabase &db);
    QSqlError lastError() const;
    ItemTypes typeForIndex(const QModelIndex &index) const;
    YTChannel* channelForIndex(const QModelIndex &index) const;
    ItemTypes typeForIndex(int index) const;
    YTChannel* channelForIndex(int index) const;
    void setHoveredRow(int row);

    Q_INVOKABLE void unsubscribe(int index);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void clearHover();
    void updateSender();
    void updateChannel(YTChannel *channel);
    void updateUnwatched();
    void removeChannel(QObject *obj);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QVector<YTChannel*> channels;
    int hoveredRow;
    QSqlError sqlError;

};

#endif // CHANNELMODEL_H
