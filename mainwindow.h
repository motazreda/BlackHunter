#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <tins/tins.h>
#include <iostream>
#include <vector>
#include "startthread.h"
#include "receiver.h"
#include <QString>

using namespace Tins;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    StartThread *th;
    Receiver *th2;

private:
    Ui::MainWindow *ui;

public slots:
    void start_sniffing();
    void stop_sniffing();
};


#endif // MAINWINDOW_H
