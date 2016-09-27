#ifndef STARTTHREAD_H
#define STARTTHREAD_H
#include <QtCore>
#include <QString>
#include <QThread>
#include <iostream>
#include <tins/tins.h>
#include <string>
#include <map>
#include <QQueue>
#include <string>

using namespace Tins;
using namespace std;

class StartThread : public QThread
{
    Q_OBJECT

public:
    explicit StartThread(QObject *parent = 0);
    void run();
    QString interface;
    //int append_to_file(int ack_str, string chunk, bool finished, uint32_t seq_num);
    //long get_length(string payload);
    bool sStop;

signals:
    void addResultLine(Packet pdu_pkt_add);


public slots:

};

#endif // STARTTHREAD_H
