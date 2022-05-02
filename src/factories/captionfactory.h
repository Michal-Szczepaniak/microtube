#ifndef CAPTIONFACTORY_H
#define CAPTIONFACTORY_H

#include "src/entities/caption.h"
#include <QJsonObject>

class CaptionFactory
{
public:
    static Caption fromJson(QJsonObject json);
};

#endif // CAPTIONFACTORY_H
