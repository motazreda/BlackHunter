#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "sniffing.h"
#include "startthread.h"
#include "receiver.h"
#include <QQueue>
#include <QMetaType>
#include <tins/tins.h>

using namespace std;
using namespace Tins;

Q_DECLARE_METATYPE (string);
Q_DECLARE_METATYPE (Packet);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qRegisterMetaType<string>("string");
    qRegisterMetaType<Packet>("Packet");

    ui->setupUi(this);
    Sniffing sniff;
    string *interfaces_list = sniff.interfaces();
    for (int i=0;interfaces_list[i] != ""; i++){
        ui->comboBox->addItem(QString::fromStdString(interfaces_list[i]));
    }
    ui->textBrowser->setStyleSheet("background-color: #2a2a2a;");
    th = new StartThread();
    th2 = new receiver();
    QObject::connect(th, SIGNAL(addResultLine(Packet)), th2, SLOT(takeResultLine(Packet)), Qt::QueuedConnection);
    QObject::connect(ui->pushButton, SIGNAL(clicked()), SLOT(start_sniffing()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), SLOT(stop_sniffing()));
}


void MainWindow::start_sniffing() {
      ui->textBrowser->append("<h5><font color=white>Sniffing Started ......</font></h5>");
      th->interface = ui->comboBox->currentText();
      th->start();
      th2->start();
}

void MainWindow::stop_sniffing() {
    ui->textBrowser->append("<h5><font color=white>Sniffing Stopped ......</font></h5>");
    th->sStop = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}
