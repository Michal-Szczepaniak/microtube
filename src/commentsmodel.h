#ifndef COMMENTSMODEL_H
#define COMMENTSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "comment.h"
#include "threadmodel.h"

class CommentsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CommentsModel(QObject *parent = 0);
    ~CommentsModel();

    enum DataRoles {
        IdRole = Qt::UserRole,
        TextRole,
        AuthorRole,
        AuthorProfileImageRole,
        SubCommentsModelRole
    };

    Q_INVOKABLE void loadComments(QString videoId);
    Q_INVOKABLE void loadMoreComments();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void parseCommentThreads(QByteArray bytes);

protected:
    QHash<int, QByteArray> roleNames() const;
    QVariant showMoreButton(int role) const;

private:
    QVector<ThreadModel*> _commentThreads;
    QString _videoId;
    QString _continuation;
};

#endif // COMMENTSMODEL_H
