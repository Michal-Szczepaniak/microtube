#ifndef TRENDINGSEARCH_H
#define TRENDINGSEARCH_H

#include <QObject>
#include <vector>
#include <QJsonArray>
#include <memory>
#include "video.h"

class TrendingSearch : public QObject {
    Q_OBJECT
public:
    QString query{};
};

#endif // TRENDINGSEARCH_H
