#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "worlds.h"
#include "view.h"
#include "qscene.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void createWorld();

public slots:
    void update();

private:
    Ui::MainWindow *ui;
    QScene         *scene;
    QTimer         *timer;
    QBox2DWorld    *world;
    QBox2DView     *view;
};

#endif // MAINWINDOW_H
