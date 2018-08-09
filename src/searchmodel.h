#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractListModel>

class SearchModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SearchModel(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SEARCHMODEL_H
