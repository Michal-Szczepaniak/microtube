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

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QtWidgets>
#include "searchparams.h"

class Video;
class VideoSource;

enum DataRoles {
    ItemTypeRole = Qt::UserRole,
    VideoRole,
    ActiveTrackRole,
    DownloadItemRole,
    HoveredItemRole,
    DownloadButtonHoveredRole,
    DownloadButtonPressedRole,
    AuthorHoveredRole,
    AuthorPressedRole,
    AuthorRole,
    ViewCountRole,
    PublishedRole,
    ThumbnailRole,
    DescriptionRole
};

enum ItemTypes { ItemTypeVideo = 1, ItemTypeShowMore };

class PlaylistModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QVariant searchParams READ searchParams NOTIFY searchParamsChanged)

public:
    PlaylistModel(QWidget *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const {
        Q_UNUSED(parent);
        return 4;
    }
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    bool removeRows(int position, int rows, const QModelIndex &parent);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    Qt::DropActions supportedDragActions() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex &parent);

    Q_INVOKABLE void setActiveRow(int row, bool notify = true);
    Q_INVOKABLE bool rowExists(int row) const { return ((row >= 0) && (row < videos.size())); }
    Q_INVOKABLE int activeRow() const { return m_activeRow; } // returns -1 if there is no active row
    Q_INVOKABLE int nextRow() const;
    Q_INVOKABLE int previousRow() const;
    Q_INVOKABLE bool nextRowExists();
    Q_INVOKABLE bool previousRowExists() const;
    Q_INVOKABLE Video *qmlVideoAt(int row) const;
    Q_INVOKABLE void findRecommended(Video *video);
    Q_INVOKABLE void loadChannelVideos(QString channelId);
    Q_INVOKABLE void loadAllSubscribedChannelsVideos();
    Q_INVOKABLE void loadUnwatchedVideos();
    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void searchAgain();
    void watch(SearchParams *searchParams);
    Q_INVOKABLE void loadCategory(QString id, QString label);
    Q_INVOKABLE void watchChannel(const QString &channelId);
    bool safeSearch();

    void removeIndexes(QModelIndexList &indexes);
    int rowForVideo(Video *video);
    QModelIndex indexForVideo(Video *video);
    void move(QModelIndexList &indexes, bool up);

    Video *videoAt(int row) const;
    Video *activeVideo() const;
    int rowForCloneVideo(const QString &videoId) const;

    VideoSource *getVideoSource() { return videoSource; }
    void setVideoSource(VideoSource *videoSource);
    void abortSearch();


public slots:
    Q_INVOKABLE void searchMore();
    void searchNeeded();
    void addVideos(const QVector<Video *> &newVideos);
    void searchFinished(int total);
    void searchError(const QString &message);
    void updateVideoSender();
    void emitDataChanged();

    void setHoveredRow(int row);
    void clearHover();
    void updateHoveredRow();

    void enterAuthorHover();
    void exitAuthorHover();
    void enterAuthorPressed();
    void exitAuthorPressed();

signals:
    void activeVideoChanged(Video *video, Video *previousVideo);
    void needSelectionFor(const QVector<Video *> &videos);
    void haveSuggestions(const QStringList &suggestions);
    void searchParamsChanged();

protected:
    QVariant searchParams();

private:
    void handleFirstVideo(Video *video);
    void searchMore(int max);

    VideoSource *videoSource;
    bool searching;
    bool canSearchMore;
    bool firstSearch;

    QVector<Video *> videos;
    QVector<Video *> deletedVideos;
    int startIndex;
    int max;

    int m_activeRow;
    Video *m_activeVideo;

    QString errorMessage;

    int hoveredRow;
    bool authorHovered;
    bool authorPressed;

    QMutex mutex;
};

#endif
