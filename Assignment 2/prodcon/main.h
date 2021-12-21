#include <chrono>
#include <deque>
#include <fstream>
#include <map>
#include <string>

#ifndef MAIN_H  // Guard against multiple delcarations
#define MAIN_H

using namespace std;

// Global variables
extern deque<string> jobs_queue;                              // queue of all jobs
extern ofstream LogFile;                                      // Log file
extern chrono::high_resolution_clock::time_point start_time;  // start of execution time
extern pthread_mutex_t jobs_queue_lock;                       // lock jobs_queue
extern pthread_cond_t jobs_queue_full;                        // condition if jobs_queue is full
extern pthread_cond_t jobs_queue_empty;                       // condition if jobs_queue is empty
extern map<string, int> summary;                              // summary of the work

// threads.cpp
/**
 * Generates one consumer thread
 */
pthread_t consumer_thread(int i);

// tands.cpp
void Sleep(int n);
void Trans(int n);

// utils.cpp
/**
 * Write summary to LogFile
 */
void write_summary();
/**
 * Write a single log line to LogFile
 */
void write_log_line(double time, int thread_id, int Q, string event, int n);
/**
 * Get number of seconds since the start_time
 */
double get_time();

#endif