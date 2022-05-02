#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <memory>
#include <vector>
#include "../helpers/jsprocesshelper.h"

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool safeSearch READ getSafeSearch WRITE setSafeSearch NOTIFY safeSearchChanged)
public:
    explicit PlaylistModel(QObject *parent = nullptr);

    enum DataRoles {
        IdRole = Qt::UserRole,
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
        UrlRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE void search(QString query);
    Q_INVOKABLE void loadRecommendedVideos(QString query);
    Q_INVOKABLE void loadCategory(QString category, QString country);
    Q_INVOKABLE QString getIdAt(int index);
    bool getSafeSearch() const;
    void setSafeSearch(bool safeSearch);

signals:
    void currentVideoStreamUrlChanged();
    void currentAudioStreamUrlChanged();
    void currentVideoChanged();
    void safeSearchChanged();

public slots:
    void searchDone(bool continuation);
    void gotTrendingVideos();
    void gotRecommendedVideos();

protected:
    QHash<int, QByteArray> roleNames() const;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    std::unique_ptr<Search> _lastSearch;
    std::vector<std::unique_ptr<Video>> _items;
    JSProcessHelper _jsProcessHelper;
    quint32 _maxDefinition;
};

#endif // PLAYLISTMODEL_H
