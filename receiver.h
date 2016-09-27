#ifndef RECEIVER_H
#define RECEIVER_H
#include <QThread>
#include <QQueue>
#include <QtCore>
#include <string>
#include <iostream>
#include <tins/tins.h>


using namespace std;
using namespace Tins;

class receiver : public QThread
{
    Q_OBJECT

public:
    explicit receiver(QObject *parent = 0);
    QQueue<string> all_packets;
    int counter=0;
    void run();
    int append_to_file(int ack_str, string chunk, bool finished, uint32_t seq_num);
    long get_length(string payload);
    void test(QQueue<string>);

signals:


public slots:
    void takeResultLine(Packet pdu_pkt_take);
};

#endif // RECEIVER_H
