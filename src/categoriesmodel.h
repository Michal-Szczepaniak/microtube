#ifndef CATEGORIESMODEL_H
#define CATEGORIESMODEL_H

#include <QObject>
#include <QAbstractProxyModel>
#include <QPair>
#include "ivvideolist.h"

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

public slots:
//    void onCategoriesLoaded(const QVector<YTCategory> &categories);

private:
    QStringList _categories;
};

#endif // CATEGORIESMODEL_H
