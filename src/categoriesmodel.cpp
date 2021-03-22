#include "categoriesmodel.h"
#include "ytregions.h"

#include <QQmlEngine>

CategoriesModel::CategoriesModel()
{
    beginInsertRows(QModelIndex(), 0, 5);
    _categories.append(tr("Most Popular"));
    _categories.append(tr("Trending"));
    _categories.append(tr("Music"));
    _categories.append(tr("News"));
    _categories.append(tr("Movies"));
    _categories.append(tr("Gaming"));
    endInsertRows();
    //    connect(&_ytCategories, SIGNAL(categoriesLoaded(const QVector<YTCategory> &)), this, SLOT(onCategoriesLoaded(const QVector<YTCategory> &)));
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
        return index.row();
    case CategoriesRoles::NameRole:
        return category;
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

//void CategoriesModel::onCategoriesLoaded(const QVector<YTCategory> &categories)
//{
//    beginInsertRows(QModelIndex(), 0, categories.length()-1);
//    _categories = categories;
//    endInsertRows();
//}
