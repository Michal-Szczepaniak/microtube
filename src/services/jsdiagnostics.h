#ifndef JSDIAGNOSTICS_H
#define JSDIAGNOSTICS_H

#include <QObject>

#include <workers/jsdiagnosticsworker.h>

class JsDiagnostics : public QObject
{
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(QString stepText READ stepText NOTIFY stepTextChanged)
    Q_OBJECT
public:
    explicit JsDiagnostics(QObject *parent = nullptr);

    Q_INVOKABLE void runDiagnostics();

    bool isRunning() const;
    void setIsRunning(bool isRunning);
    QString statusText() const;
    QString stepText() const;

public slots:
    void onStatusChanged(int status);
    void onStep(int step);

signals:
    void statusTextChanged();
    void stepTextChanged();

private:
    int _status = 0;
    int _step = 0;
    JsDiagnosticsWorker _worker;

};

#endif // JSDIAGNOSTICS_H
