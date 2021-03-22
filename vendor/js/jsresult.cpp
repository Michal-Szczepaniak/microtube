#include "jsresult.h"

QJSValue JSResult::setData(QJSValue value) {
    qDebug() << "Success" << value.toString();

    if (isSignalConnected(QMetaMethod::fromSignal(&JSResult::json))) {
        qDebug() << "json";
        if (value.isObject()) {
            auto doc = QJsonDocument::fromVariant(value.toVariant());
            if (doc.isEmpty()) {
                qDebug() << value.toString();
                emit error("Cannot parse JSON");
                return QJSValue();
            }
            emit json(doc);
        }
    }

    if (isSignalConnected(QMetaMethod::fromSignal(&JSResult::string))) {
        qDebug() << "Emitting string signal";
        if (value.isUndefined() || value.isNull())
            emit string(QString());
        else
            emit string(value.toString());
    }

    // This object has JavascriptOwnership
    // But it is deleted very late by the JS engine
    deleteLater();

    return QJSValue();
}

QJSValue JSResult::setError(QJSValue value) {
    QString message = value.toString();
    qWarning() << "Error" << message;
    qDebug() << value.property("stack").toString().splitRef('\n');
    emit error(message);
    return QJSValue();
}
