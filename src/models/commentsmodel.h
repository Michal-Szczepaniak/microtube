#ifndef COMMENTSMODEL_H
#define COMMENTSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "entities/comment.h"
#include "managers/jsprocessmanager.h"

class CommentsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)
public:
    enum CommentsRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        PhotoRole,
        TextRole,
        PublishedRole,
        RepliesCountRole,
        RepliesContinuationRole
    };

    explicit CommentsModel(QObject *parent = nullptr);
    Q_INVOKABLE void loadCommentsForVideo(QString videoId);
    Q_INVOKABLE void loadRepliesForComment(QJsonObject continuation);
    bool isBusy() const;
    void setBusy(bool busy);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

signals:
    void busyChanged();

public slots:
    void gotComments(bool canContinue, bool isContinuation);
    void gotCommentReplies(QJsonObject continuation);

protected:
    QHash<int, QByteArray> roleNames() const override;
    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    std::vector<Comment> _comments;
    bool _canContinue;
    bool _busy;
    QString _videoId;
    QJsonObject _repliesContinuation;
    JSProcessManager _jsProcessHelper;
};

#endif // COMMENTSMODEL_H
