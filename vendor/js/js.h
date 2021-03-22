#ifndef JS_H
#define JS_H

#include <QtQml>

#include "jsnamfactory.h"
#include "jsresult.h"

class JSTimer : public QTimer {
    Q_OBJECT

public:
    static auto &getTimers() {
        static QHash<uint, JSTimer *> timers;
        return timers;
    }

    auto getId() const { return id; }

    // This should be static but cannot bind static functions to QJSEngine
    Q_INVOKABLE QJSValue clearTimeout(QJSValue id) {
        auto timer = getTimers().take(id.toUInt());
        if (timer) {
            timer->stop();
            timer->deleteLater();
        } else
            qDebug() << "Unknown id" << id.toUInt();
        return QJSValue();
    }
    // This should be static but cannot bind static functions to QJSEngine
    Q_INVOKABLE QJSValue setTimeout(QJSValue callback, QJSValue delayTime, QJSValue args) {
        qDebug() << callback.toString() << delayTime.toInt() << args.toString();

        QJSValueList valueArgs;
        if (args.isArray()) {
            const int argsLength = args.property("length").toInt();
            for (int i = 0; i < argsLength; ++i) {
                auto arg = args.property(i);
                qDebug() << "Adding arg" << arg.toString();
                valueArgs << arg;
            }
        }

        auto timer = new JSTimer();
        timer->setInterval(delayTime.toInt());
        connect(timer, &JSTimer::timeout, this, [callback, valueArgs]() mutable {
            qDebug() << "Calling" << callback.toString();
            if (!callback.isCallable()) qDebug() << callback.toString() << "is not callable";
            auto value = callback.call(valueArgs);
            if (value.isError()) {
                qWarning() << "Error" << value.toString();
                qDebug() << value.property("stack").toString().splitRef('\n');
            }
        });
        timer->start();
        return timer->getId();
    }

    Q_INVOKABLE JSTimer(QObject *parent = nullptr) : QTimer(parent) {
        setTimerType(Qt::CoarseTimer);
        setSingleShot(true);
        // avoid 0
        static uint counter = 1;
        id = counter++;
        connect(this, &JSTimer::destroyed, this, [id = id] { getTimers().remove(id); });
        connect(this, &JSTimer::timeout, this, &QTimer::deleteLater);
        getTimers().insert(id, this);
    }

private:
    uint id;
};

class JS : public QObject {
    Q_OBJECT

public:
    static JS &instance();

    explicit JS(QObject *parent = nullptr);
    JSNAMFactory &getNamFactory() { return namFactory; };

    void initialize(const QUrl &url);
    bool checkError(const QJSValue &value);

    bool isInitialized();
    QQmlEngine &getEngine() { return *engine; }

    QProcess* executeNodeScript(QString name, QStringList args);
    JSResult &callFunction(JSResult *result, const QString &name, const QJSValueList &args);

signals:
    void initialized();
    void initFailed(QString message);

private:
    void initialize();

    QQmlEngine *engine;
    JSNAMFactory namFactory;
    bool initializing = false;
    bool ready = false;
    QUrl url;
};

#endif // JS_H
