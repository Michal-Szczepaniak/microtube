#ifndef JSRESULT_H
#define JSRESULT_H

#include <QtQml>

class JSResult : public QObject {
    Q_OBJECT

public:
    JSResult(QObject *parent = nullptr) : QObject(parent) {}
    ~JSResult() { qDebug() << "Destroying result"; }

    template <typename Functor> JSResult &onString(Functor lambda) {
        connect(this, &JSResult::string, this, lambda);
        return *this;
    }
    template <typename Functor> JSResult &onJson(Functor lambda) {
        connect(this, &JSResult::json, this, lambda);
        return *this;
    }
    template <typename Functor> JSResult &onError(Functor lambda) {
        connect(this, &JSResult::error, this, lambda);
        return *this;
    }

    Q_INVOKABLE QJSValue setData(QJSValue value);
    Q_INVOKABLE QJSValue setError(QJSValue value);

signals:
    void json(const QJsonDocument &doc);
    void string(const QString &data);
    void error(const QString &message);
};

#endif // JSRESULT_H
