#include "startthread.h"
#include <QtCore>
#include <unistd.h>
#include <QObject>
#include <QDebug>
#include <tins/tins.h>
#include <string>
#include <map>
#include <fstream>
#include <cstdio>
#include <algorithm>

#include <QQueue>

using namespace Tins;
using namespace std;

StartThread::StartThread(QObject *parent) : QThread(parent)
{
    sStop = false;
}

void StartThread::run() {
    string interface_string = this->interface.toStdString();
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    Sniffer sniffer(interface_string);
    // loop through packets
    for(;;){
       if(sStop) {break;}
       Packet packet = sniffer.next_packet();
       emit addResultLine(packet);
    } // For Loop End
}
