#ifndef THREADMODEL_H
#define THREADMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QPointer>
#include "comment.h"

class ThreadModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ThreadModel();

    enum DataRoles {
        IdRole = Qt::UserRole,
        TextRole,
        AuthorRole,
        AuthorProfileImageRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    const Comment &getTopLevelComment() const;
    Comment &getTopLevelCommentEditable();
    void loadComments();

public slots:
    void parseComments(QByteArray bytes);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QVector<Comment> _comments;
    Comment _comment;
};

typedef QPointer<ThreadModel> ThreadModelPointer;
Q_DECLARE_METATYPE(ThreadModelPointer)

#endif // THREADMODEL_H
