#include "categoriesmodel.h"

#include <QQmlEngine>

CategoriesModel::CategoriesModel()
{
    beginInsertRows(QModelIndex(), 0, 5);
    _categories.append(tr("Now"));
    _codes.append("Now");
    _categories.append(tr("Music"));
    _codes.append("Music");
    _categories.append(tr("Gaming"));
    _codes.append("Gaming");
    _categories.append(tr("Movies"));
    _codes.append("Movies");
    endInsertRows();
}

int CategoriesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _categories.length();
}

QVariant CategoriesModel::data(const QModelIndex &index, int role) const
{
    if (rowCount() <= 0 || index.row() < 0 || index.row() >= rowCount()) return QVariant();

    auto category = _categories[index.row()];
    auto code = _codes[index.row()];
    switch (role) {
    case CategoriesRoles::IdRole:
        return index.row();
    case CategoriesRoles::NameRole:
        return category;
    case CategoriesRoles::CodeRole:
        return code;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CategoriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]   = "id";
    roles[NameRole] = "name";
    roles[CodeRole] = "code";
    roles[PhotoRole]   = "photo";
    return roles;
}
