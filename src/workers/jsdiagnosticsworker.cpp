#include "jsdiagnosticsworker.h"

#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QDebug>

JsDiagnosticsWorker::JsDiagnosticsWorker(QObject *parent) : QThread(parent)
{

}

void JsDiagnosticsWorker::run()
{
    QString appPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/js/";
    QFileInfo appDir(appPath);

    emit statusChanged(1);
    if (!appDir.exists()) {
        emit statusChanged(-1);
        return;
    }

    emit step(2);
    if (!appDir.isDir()) {
        emit statusChanged(-1);
        return;
    }

    emit step(3);
    if (!appDir.isReadable()) {
        emit statusChanged(-1);
        return;
    }

    emit step(4);
    if (!appDir.isWritable()) {
        emit statusChanged(-1);
        return;
    }

    emit step(5);
    if (!appDir.isExecutable()) {
        emit statusChanged(-1);
        return;
    }

    emit step(6);
    QProcess checkNodeProcess;
    checkNodeProcess.start("node18", QStringList() << "-v", QIODevice::OpenModeFlag::ReadWrite);
    checkNodeProcess.waitForFinished();
    QString output = QString::fromLatin1(checkNodeProcess.readAllStandardOutput());
    if (output != "v18.2.0\n") {
        qDebug() << output;
        emit statusChanged(-1);
        return;
    }

    emit step(7);
    emit statusChanged(1);
}
