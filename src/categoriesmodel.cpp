#include "categoriesmodel.h"

CategoriesModel::CategoriesModel()
{
    _ytCategories.loadCategories();
    connect(&_ytCategories, SIGNAL(categoriesLoaded(const QVector<YTCategory> &)), this, SLOT(onCategoriesLoaded(const QVector<YTCategory> &)));
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
    switch (role) {
    case CategoriesRoles::IdRole:
        return category.term;
    case CategoriesRoles::NameRole:
        return category.label;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> CategoriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole]   = "id";
    roles[NameRole] = "name";
    roles[PhotoRole]   = "photo";
    return roles;
}

void CategoriesModel::onCategoriesLoaded(const QVector<YTCategory> &categories)
{
    beginInsertRows(QModelIndex(), 0, categories.length()-1);
    _categories = categories;
    endInsertRows();
}
