#include "receiver.h"
#include <iostream>
#include <QQueue>
#include <QtCore>
#include <unistd.h>
#include <tins/tins.h>
#include <string>
#include <map>
#include <fstream>
#include <cstdio>
#include <algorithm>

using namespace std;
using namespace Tins;

//global
uint32_t ack_number = 0;
map<string, bool> detected;
map<string, bool> finished;
map<string, uint32_t> acks;
map<string, uint32_t> seqs;
map<string, vector<uint32_t>> all_seqs;
map<string, long> all_length;

long content_length=0;

receiver::receiver(QObject *parent) : QThread(parent)
{

}

void receiver::run() {
    cout << "Running thread 2" << endl;
}

void receiver::takeResultLine(Packet pdu_pkt) {
    if(pdu_pkt.pdu()->find_pdu<IP>()) {
           if(pdu_pkt.pdu()->find_pdu<TCP>()) {
               if(pdu_pkt.pdu()->find_pdu<RawPDU>()) {
                   IP &ip = pdu_pkt.pdu()->rfind_pdu<IP>();
                   TCP &tcp = pdu_pkt.pdu()->rfind_pdu<TCP>();
                   RawPDU &rawdata = pdu_pkt.pdu()->rfind_pdu<RawPDU>();
                   RawPDU::payload_type &payload = rawdata.payload();
                   string raw_data(payload.begin(), payload.end());
                   finished[to_string(tcp.ack_seq())] = false;
                   // begin chunk
                   cout << "debug 2" << endl;
                   if(detected[to_string(tcp.ack_seq())]) {

                       ack_number = tcp.ack_seq();
                       string ack_str = to_string(ack_number);
                       if((tcp.flags() == TCP::ACK) || (tcp.flags() == (TCP::ACK | TCP::PSH))){
                           if(acks[ack_str] != 0 || acks[ack_str] == tcp.ack_seq()) {
                               if(find(all_seqs[to_string(tcp.ack_seq())].begin(), all_seqs[to_string(tcp.ack_seq())].end(), tcp.seq()) != all_seqs[to_string(tcp.ack_seq())].end()) {
                                   cout << "Got Duplicate ACK " << endl;
                               }else{
                                   if(content_length == (tcp.seq() - seqs[to_string(tcp.ack_seq())])){
                                          finished[to_string(tcp.ack_seq())] = true;
                                   }
                                   all_length[ack_str] += receiver::append_to_file(acks[ack_str], raw_data.substr(0, raw_data.size()-1), finished[to_string(tcp.ack_seq())], (tcp.seq() - seqs[to_string(tcp.ack_seq())]));
                                   all_seqs[to_string(tcp.ack_seq())].push_back(tcp.seq());
                                   cout << "all_length: " << all_length[ack_str] << endl;
                                   cout << "Relative Seq Number is: " << (tcp.seq() - seqs[to_string(tcp.ack_seq())]) << endl;
                                   cout << "##############################################################" << endl;
                               }

                           }
                       }
                   }
                   // end chunk

                   //begin chunk 2
                   size_t check_content_type = raw_data.find("Content-Type: application/octet-stream");
                   if(check_content_type != string::npos) {
                       finished[to_string(tcp.ack_seq())] = false;
                       seqs[to_string(tcp.ack_seq())] = tcp.seq();
                       content_length = receiver::get_length(raw_data);
                       cout << "the content length is: " << content_length << endl;
                       detected[to_string(tcp.ack_seq())] = true;
                       ack_number = tcp.ack_seq();
                       string ack_str = to_string(ack_number);
                       acks[ack_str] = tcp.ack_seq();
                       if((tcp.flags() == TCP::ACK)) {
                            if(find(all_seqs[to_string(tcp.ack_seq())].begin(), all_seqs[to_string(tcp.ack_seq())].end(), tcp.seq()) != all_seqs[to_string(tcp.ack_seq())].end()) {
                                cout << "Got Duplicate Ack" << endl;
                            }else{
                                if(content_length == (tcp.seq() - seqs[to_string(tcp.ack_seq())])){
                                       finished[to_string(tcp.ack_seq())] = true;
                                }
                                all_length[to_string(tcp.ack_seq())] += receiver::append_to_file(acks[ack_str], raw_data.substr(0, raw_data.size()-1), finished[to_string(tcp.ack_seq())], (tcp.seq() - seqs[to_string(tcp.ack_seq())]));
                                all_seqs[to_string(tcp.ack_seq())].push_back(tcp.seq());
                            }
                       }
//                       string src_ip_address = ip.src_addr().to_string();
//                       string dst_ip_address = ip.dst_addr().to_string();
//                       QString data = "<font color=#00FF00>Detected File coming from " + \
//                               QString::fromStdString(src_ip_address) + \
//                               " -> " + \
//                               QString::fromStdString(dst_ip_address) + \
//                               "with ACK number " + \
//                               QString::number(ack_number) + "</font><br>" + \
//                               QString::fromStdString(raw_data);
                       //emit addResultLine(data);

                   }
                   // end chunk 2

               }
           }
    }

}


// append chunk too the file
int receiver::append_to_file(int acks_ptr, string file_chunk, bool finished_1, uint32_t seq_num) {
    cout << "the ack number is " << acks_ptr << endl;
    ofstream f;
    f.open(to_string(acks_ptr), ios::app);
    f.seekp(seq_num);
    f.write(file_chunk.c_str(), file_chunk.size());
    f.close();
    if(finished_1){
        string final_file = to_string(acks_ptr)+"_123";
        cout << "finished" << endl;
        rename(to_string(acks_ptr).c_str(), final_file.c_str());
    }else{
        cout << "payload size is: " << (file_chunk.size()+1) << endl;
    }
    return (file_chunk.size()+1);
}


// get Content-Length
long receiver::get_length(string payload) {
    size_t check_for_length = payload.find("Content-Length: ");
    vector<char> raw_data_vec(payload.begin(), payload.end());
    int n=0;
    vector<char> length(n);
    cout << raw_data_vec[check_for_length] << endl;
    for(int i=(check_for_length+15) ; raw_data_vec[i] != 0xd ; ++i) {
        length.push_back(raw_data_vec[i]);
        n++;
    }
    string slength(length.begin(), length.end());
    return stol(slength);
}
