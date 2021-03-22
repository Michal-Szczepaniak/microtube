#include "mainwindow.h"


QAction *MainWindow::getAction(QString s)
{
    Q_UNUSED(s)
    return new QAction(this);
}
