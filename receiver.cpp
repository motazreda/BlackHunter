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
map<string, bool> finished_flag;
map<string, uint32_t> acks;
map<string, uint32_t> seqs;
map<string, vector<uint32_t>> all_seqs;
map<string, long> all_length;

long content_length=0;

receiver::receiver(QObject *parent) : QThread(parent)
{
//    map<string, bool> finished;
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
                   finished_flag[to_string(tcp.ack_seq())] = false;
                   RawPDU &rawdata = pdu_pkt.pdu()->rfind_pdu<RawPDU>();
                   RawPDU::payload_type &payload = rawdata.payload();
                   string raw_data(payload.begin(), payload.end());
                   //finished[to_string(tcp.ack_seq())] = false;
                   // begin chunk
                   cout << "debug 2" << endl;
                   if(detected[to_string(tcp.ack_seq())]) {

                       ack_number = tcp.ack_seq();
                       string ack_str = to_string(ack_number);
                       if((tcp.flags() == TCP::ACK) || (tcp.flags() == (TCP::ACK | TCP::PSH))){
                           if(acks[ack_str] != 0 || acks[ack_str] == tcp.ack_seq()) {
                               if(find(all_seqs[to_string(tcp.ack_seq())].begin(),
                                       all_seqs[to_string(tcp.ack_seq())].end(), tcp.seq())
                                       !=
                                       all_seqs[to_string(tcp.ack_seq())].end()
                               ) {
                                   cout << "Got Duplicate ACK " << endl;
                               }else{
                                   if(content_length == (tcp.seq() - seqs[to_string(tcp.ack_seq())])){
                                          finished_flag[to_string(tcp.ack_seq())] = true;
                                   }
                                   all_length[ack_str] +=
                                           receiver::append_to_file(
                                                                    acks[ack_str],
                                                                    raw_data.substr(0, raw_data.size()-1),
                                                                    finished_flag[to_string(tcp.ack_seq())],
                                                                    (tcp.seq() - seqs[to_string(tcp.ack_seq())])
                                           );
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
                   if(type_of_resp(raw_data) == 200) {
                       cout << "Response is " << 200 << endl;
                   }else if(type_of_resp(raw_data) == 206) {
                       cout << "Response is " << 206 << endl;
                   }
                   size_t check_content_type = raw_data.find("Content-Type: application/octet-stream");
                   if(check_content_type != string::npos) {
                       finished_flag[to_string(tcp.ack_seq())] = false;
                       seqs[to_string(tcp.ack_seq())] = tcp.seq();
                       content_length = receiver::get_length(raw_data);
                       vector<string> chunk;
                       chunk = receiver::get_data_without_content_header(raw_data);
                       raw_data = chunk[0];
                       int content_h_size = std::stoi(chunk[1]);
                       cout << "Content Header Size is " << content_h_size << endl;
                       cout << "the content length is: " << content_length << endl;
                       detected[to_string(tcp.ack_seq())] = true;
                       ack_number = tcp.ack_seq();
                       string ack_str = to_string(ack_number);
                       acks[ack_str] = tcp.ack_seq();
                       if((tcp.flags() == TCP::ACK)) {
                            if(find(all_seqs[to_string(tcp.ack_seq())].begin(),
                                    all_seqs[to_string(tcp.ack_seq())].end(),
                                    tcp.seq())
                                    !=
                                    all_seqs[to_string(tcp.ack_seq())].end()
                            ) {
                                cout << "Got Duplicate Ack" << endl;
                            }else{
                                if(content_length == (tcp.seq() - seqs[to_string(tcp.ack_seq())])){
                                       finished_flag[to_string(tcp.ack_seq())] = true;
                                }
                                all_length[to_string(tcp.ack_seq())] +=
                                        receiver::append_to_file(
                                            acks[ack_str],
                                            raw_data.substr(0, raw_data.size()-1),
                                            finished_flag[to_string(tcp.ack_seq())],
                                            (tcp.seq() - seqs[to_string(tcp.ack_seq())] - content_h_size)
                                        );
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
    vector<char> data_bytes(file_chunk.begin(), file_chunk.end());
    f.open(to_string(acks_ptr), ios::app);
    f.seekp(seq_num);
    for(unsigned int i=0;i!=data_bytes.size()+1;i++)
        f.write(&data_bytes[i], 1);
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


vector<string> receiver::get_data_without_content_header(string full_payload) {
    size_t check_for_header = full_payload.find("\r\n\r\n");

    //cout << full_payload << endl;
    check_for_header = (check_for_header + 4);
    cout << "got position is number " << check_for_header << endl;
    vector<char> data_without_header(full_payload.begin(), full_payload.end());
    int n=0;
    vector<char> len(n);
    cout << "Now Begin show u data" << endl;
    for(unsigned int i=check_for_header; i != (full_payload.size()); i++) {
        len.push_back(data_without_header[i]);
        cout << data_without_header[i];
        n++;
    }
    cout << "\n$$$$$$$$$" << endl;
    string payload_without_header(len.begin(), len.end());
    vector<string> chunk_info;
    chunk_info.push_back(payload_without_header);
    chunk_info.push_back(std::to_string(check_for_header));
    return chunk_info;
}

//check response
int receiver::type_of_resp(string data){
    if((data.find("HTTP/1.1 200 OK")) != string::npos)
        return 200;
    else if ((data.find("HTTP/1.1 206 Partial Content")) != string::npos)
        return 206;
    else
        return -1;
}
