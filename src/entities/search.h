#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <vector>
#include <QJsonArray>
#include <memory>
#include "video.h"
#include "parsers/videosparser.h"

class Search : public QObject {
    Q_OBJECT
public:
    QString query{};
    quint8 pages = 1;
    bool safeSearch = false;
    QJsonArray continuation{};
    SearchResults items;
};

#endif // SEARCH_H
