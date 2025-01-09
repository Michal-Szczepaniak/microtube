#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

class JsonHelper {
public:
    static QJsonValue find(QJsonArray array, bool(*callback)(QJsonObject)) {
        for (QJsonValue val : array) {
            QJsonObject obj = val.toObject();
            if (callback(obj)) {
                return obj;
            }
        }

        return QJsonValue();
    }
};

#endif // JSONHELPER_H
