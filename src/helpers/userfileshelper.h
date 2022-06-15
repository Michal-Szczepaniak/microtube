#ifndef USERFILESHELPER_H
#define USERFILESHELPER_H

#include <QObject>

class UserFilesHelper : public QObject
{
    Q_OBJECT
public:
    UserFilesHelper();

    void copyJsFiles();
    void copyDesktopFile();

signals:
    void updateFinished();
};

#endif // USERFILESHELPER_H
