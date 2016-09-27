#include "sniffing.h"
#include "mainwindow.h"

using namespace std;
using namespace Tins;

Sniffing::Sniffing()
{
    cout << "accessed Sniffing Class" << endl;
}

// function for list iavilable interfaces
string *Sniffing::interfaces() {
    int interface_index = 0;
    string *arr = new string[100];
    vector<NetworkInterface> interfaces = NetworkInterface::all();
    for(const NetworkInterface& iface : interfaces) {
        arr[interface_index] = iface.name();
        interface_index++;
    }
    return arr;
}

