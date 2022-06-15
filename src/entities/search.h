#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <vector>
#include <QJsonArray>
#include <memory>
#include "video.h"

class Search : public QObject {
    Q_OBJECT
public:
    QString query{};
    quint8 pages = 1;
    bool safeSearch = false;
    QJsonArray continuation{};
    std::vector<std::unique_ptr<Video>> items;
};

#endif // SEARCH_H
