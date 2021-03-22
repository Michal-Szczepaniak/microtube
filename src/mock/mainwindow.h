#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QAction>

class MainWindow : public QObject
{
    Q_OBJECT
public:
    static MainWindow* instance() {
        static MainWindow instance;
        return &instance;
    }

    QAction* getAction(QString s);

private:
    MainWindow()= default;
    ~MainWindow()= default;
    MainWindow(const MainWindow&)= delete;
    MainWindow& operator=(const MainWindow&)= delete;
};

#endif // MAINWINDOW_H
