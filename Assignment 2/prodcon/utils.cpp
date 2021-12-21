#include <pthread.h>

#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>

#include "main.h"

using namespace std;

void write_summary_line(string first, int second) {
    LogFile << "    " << setw(15) << left << first << second << endl;
}

void write_summary() {
    map<string, int> summary_copy(summary);
    LogFile << "Summary:" << endl;
    string print_order[5] = {"Work", "Ask", "Receive", "Complete", "Sleep"};
    for (int i = 0; i < 5; i++) {
        write_summary_line(print_order[i], summary_copy[print_order[i]]);  // print summary line
        summary_copy.erase(print_order[i]);                                // remove from map
    }
    // Remaining keys are Thread # keys
    for (const auto& summary_item : summary_copy) {
        write_summary_line(summary_item.first, summary_item.second);
    }
    // Calculate and write transactions per second
    double trans_per_sec = summary["Work"] / get_time();  // (# of 'T')/(# of secs)
    LogFile << "Transactions per second: " << setprecision(2) << trans_per_sec << endl;
}

pthread_mutex_t write_log_file_lock = PTHREAD_MUTEX_INITIALIZER;  // locks summary object
void write_log_line(double time, int thread_id, int Q, string event, int n) {
    pthread_mutex_lock(&write_log_file_lock);

    LogFile << setw(8) << fixed << setprecision(3) << left << time  // 0.000
            << "ID= " << setw(2) << right << thread_id;             // ID= 1
    if (Q != -1)
        LogFile << "  Q= " << setw(2) << right << Q;  // Q= 0
    else
        LogFile << left << setw(7) << " ";
    LogFile << "  " << left << setw(13) << event;  // Ask
    if (n != -1) LogFile << n;                     // 4
    LogFile << endl;                               // \n

    pthread_mutex_unlock(&write_log_file_lock);
}

double get_time() {
    auto now = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_seconds = now - start_time;
    return elapsed_seconds.count();
}
