#ifndef JSDIAGNOSTICSWORKER_H
#define JSDIAGNOSTICSWORKER_H

#include <QThread>
#include <QObject>

class JsDiagnosticsWorker : public QThread
{
    Q_OBJECT
public:
    explicit JsDiagnosticsWorker(QObject *parent = nullptr);

    void run();

signals:
    void step(int step);
    void statusChanged(int status);

};

#endif // JSDIAGNOSTICSWORKER_H
