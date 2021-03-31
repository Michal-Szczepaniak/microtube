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
    QStringList files = { "channelInfo.js", "channelVideos.js", "package.json", "search.js", "videoInfo.js" };

    QString appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";

    if (!QDir(appDataLocation + "js").exists())
    {
        QDir().mkdir(appDataLocation + "js");
    }

    for (QString file : files) {
        if (QFile::exists(appDataLocation + "js/" + file))
        {
            QFile::remove(appDataLocation + "js/" + file);
        }
        QFile::copy("/usr/share/microtube/js/" + file, appDataLocation + "js/" + file);
    }

    if (!QDir(appDataLocation + "js/node_modules").exists()) {
        QProcess* process = new QProcess();
        process->setWorkingDirectory(appDataLocation + "js/");
        process->start("npm", QStringList() << "install");
        process->waitForFinished();
    } else {
        QProcess* process = new QProcess();
        process->setWorkingDirectory(appDataLocation + "js/");
        process->start("npm", QStringList() << "update");
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
