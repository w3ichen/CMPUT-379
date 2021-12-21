#include <stdlib.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "main.h"

using namespace std;

int get_port(int argc, char *argv[]) {
    int port;
    if (argc < 2) {
        cout << "<port> not provided - fallback to port 8888" << endl;
        port = 8888;
    } else {
        port = atoi(argv[1]);
    }

    if (port <= 5000 || port >= 64000) {
        cout << "<port> must be in the range of 5,000 to 64,000" << endl;
        exit(0);
    }
    cout << "Using port " << port << endl;

    return port;
}

void get_port_and_ip_address(int argc, char *argv[], int &port, string &ip_address, ofstream &LogFile) {
    port = get_port(argc, argv);
    LogFile << "Using port " << port << endl;

    // Get ip address
    if (argc < 3) {
        cout << "<ip_address> not provided - fallback to ip address 127.0.0.1" << endl;
        ip_address = "127.0.0.1";
    } else {
        ip_address = argv[2];
    }
    LogFile << "Using server address " << ip_address << endl;
    cout << "Using server address " << ip_address << endl;
}

void write_epoch_time(ofstream &LogFile) {
    chrono::milliseconds ms = chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch());
    // Print as seconds
    LogFile << fixed << setprecision(2) << (double)ms.count() / 1000 << ":";
}
void write_cmd(string cmd, ofstream &LogFile) {
    // (T 8)
    LogFile << "(" << left << cmd[0] << setw(3) << right << cmd.substr(1) << ")  ";
}

// Parse string up to \t delimiter
void clean_string(string &str) {
    string delim = "\t";
    size_t pos = str.find(delim);
    str = str.substr(0, pos);
}

void write_server_log(int trans_id, string cmd, string machine_name, ofstream &LogFile) {
    clean_string(cmd);

    write_epoch_time(LogFile);                                 // 1583256161.99:
    LogFile << "  #" << setw(3) << right << trans_id << "  ";  // # 1
    write_cmd(cmd, LogFile);                                   // (T 1)
    LogFile << "from " << machine_name;                        // from ug11.20295

    LogFile << endl;
}
void write_client_log(string type, string cmd, ofstream &LogFile) {
    clean_string(cmd);

    write_epoch_time(LogFile);        // 1583256161.99:
    LogFile << "  " << type << "  ";  // Send
    write_cmd(cmd, LogFile);          // (T 1)
    LogFile << endl;
}

void split_message(string str, string &machine_name, string &cmd) {
    // Message is `<machine_name>\t<cmd>`
    string delim = "\t";
    size_t pos = str.find(delim);
    machine_name = str.substr(0, pos);
    str.erase(0, pos + delim.length());

    pos = str.find(delim);
    cmd = str.substr(0, pos);
}

void write_summary(map<string, int> trans_sent_map, chrono::high_resolution_clock::time_point start_time, ofstream &LogFile) {
    int total_trans = 0;
    LogFile << "\nSUMMARY\n";
    for (const auto &summary_item : trans_sent_map) {
        // 14 transactions from ug11.20295
        LogFile << setw(5) << right << summary_item.second << " transactions from " << summary_item.first << endl;
        total_trans += summary_item.second;
    }

    // 21.3 transactions/sec (29/1.36)
    auto now = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_seconds = now - start_time;

    double secs = elapsed_seconds.count() - (double)TIMEOUT;  // subtract idle timeout
    LogFile << fixed << setprecision(1) << setw(5) << right << ((double)total_trans / secs) << " transactions/sec  "
            << "(" << total_trans << "/" << fixed << setprecision(2) << secs << ")" << endl;
}