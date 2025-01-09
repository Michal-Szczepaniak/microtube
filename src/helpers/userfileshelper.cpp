#include "userfileshelper.h"
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QDebug>

UserFilesHelper::UserFilesHelper()
{

}

void UserFilesHelper::copyJsFiles()
{
    QStringList files = { "basicVideoInfo.js", "channelInfo.js", "commentReplies.js", "comments.js", "package.json", "videoInfo.js", "unified.js", "subscriptionsAggregator.js" };

    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";

    if (!QDir(appDataLocation + "js").exists())
    {
        QDir().mkpath(appDataLocation + "js");
    }

    for (QString file : files) {
        if (QFile::exists(appDataLocation + "js/" + file))
        {
            QFile::remove(appDataLocation + "js/" + file);
        }
        QFile::copy("/usr/share/microtube/js/" + file, appDataLocation + "js/" + file);
    }

    QProcess* process = new QProcess();
    process->setWorkingDirectory(appDataLocation + "js/");
    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), [process, this](int exitStatus) { process->deleteLater(); emit updateFinished(); });

    if (!QDir(appDataLocation + "js/node_modules").exists()) {
        process->start("npm18", QStringList() << "install");
    } else {
        process->start("npm18", QStringList() << "update");
    }
}

void UserFilesHelper::copyDesktopFile()
{
    QString applicationsPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/";
    QStringList files = { "open-url.desktop", "microtube-url.desktop" };


    for (QString file : files) {
        if (QFile::exists(applicationsPath + file))
        {
            QFile::remove(applicationsPath + file);
        }
        QFile::copy("/usr/share/applications/" + file, applicationsPath + file);
    }

    QProcess* process = new QProcess();
    process->start("update-desktop-database", QStringList() << applicationsPath);
    process->waitForFinished();
}
