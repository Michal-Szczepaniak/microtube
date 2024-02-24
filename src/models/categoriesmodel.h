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
        CodeRole,
        PhotoRole
    };

    CategoriesModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = IdRole) const override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QStringList _categories;
    QStringList _codes;
};

#endif // CATEGORIESMODEL_H
