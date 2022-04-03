#ifndef INVIDIOUS_H
#define INVIDIOUS_H

#include <QAbstractListModel>
#include <QObject>

class Invidious : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit Invidious(QObject *parent = nullptr);
};

#endif // INVIDIOUS_H
