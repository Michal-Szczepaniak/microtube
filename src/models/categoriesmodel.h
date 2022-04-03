#ifndef CATEGORIESMODEL_H
#define CATEGORIESMODEL_H

#include <QObject>
#include <QAbstractProxyModel>
#include <QPair>

class CategoriesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CategoriesRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        PhotoRole
    };

    CategoriesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = IdRole) const;
    QHash<int, QByteArray> roleNames() const;

private:
    QStringList _categories;
};

#endif // CATEGORIESMODEL_H
