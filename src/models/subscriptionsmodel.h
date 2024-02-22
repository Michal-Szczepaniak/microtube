#ifndef SUBSCRIPTIONSMODEL_H
#define SUBSCRIPTIONSMODEL_H

#include <QAbstractListModel>
#include "src/repositories/videorepository.h"
#include "src/repositories/authorrepository.h"
#include "src/entities/author.h"

class SubscriptionsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum SubscriptionRoles {
        IdRole = Qt::UserRole + 1,
        DatabaseIdRole,
        AuthorIdRole,
        NameRole,
        AvatarRole,
        UnwatchedRole
    };

    explicit SubscriptionsModel(QObject *parent = nullptr);
    Q_INVOKABLE void loadSubscriptionsList();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void markAllAsWatched();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = IdRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<Author> _subscriptions;
    AuthorRepository _authorRepository;
    VideoRepository _videoRepository;
};

#endif // SUBSCRIPTIONSMODEL_H
