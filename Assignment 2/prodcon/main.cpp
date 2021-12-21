#include "main.h"

#include <pthread.h>
#include <unistd.h>

#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

deque<string> jobs_queue;
ofstream LogFile;
chrono::high_resolution_clock::time_point start_time;
pthread_mutex_t jobs_queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t jobs_queue_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t jobs_queue_empty = PTHREAD_COND_INITIALIZER;
map<string, int> summary = {{"Work", 0},
                            {"Ask", 0},
                            {"Receive", 0},
                            {"Complete", 0},
                            {"Sleep", 0}};

int main(int argc, char** argv) {
    // argv is [./prodcon, nthreads, log_id]
    string log_filename;     // log file id
    string command;          // command from input file
    deque<string> all_jobs;  // all jobs from std input

    if (argc == 1) {
        cout << "nthreads must be provided" << endl
             << "./prodcon <nthreads>" << endl;
        exit(0);
    }
    // (1) Get number of threads
    int nthreads = atoi(argv[1]);  // number of threads

    // (2) Name the log file
    if (argc <= 2 || atoi(argv[2]) == 0) {
        // If no log id provided or id is 0, then use `prodcon.log`
        log_filename = "prodcon.log";
    } else {
        log_filename = "prodcon." + string(argv[2]) + ".log";
    }

    // (3) Open log file
    LogFile.open(log_filename, ofstream::out | ofstream::trunc);

    // (4) Load in all jobs into a queue
    while (!cin.eof() && getline(cin, command)) {
        all_jobs.push_back(command);
        // Count summary
        if (command[0] == 'T')
            summary["Work"]++;  // Producer: # of ‘T’ commands
        else if (command[0] == 'S')
            summary["Sleep"]++;  // Producer: # of ‘S’ commands
    }

    // (5) Start timer
    start_time = chrono::high_resolution_clock::now();

    // (6) Create `nthreads` threads
    int i;
    pthread_t thread_id;
    vector<pthread_t> thread_ids;
    for (i = 0; i < nthreads; ++i) {
        thread_id = consumer_thread(i + 1);         // create the thread
        thread_ids.push_back(thread_id);            // add to `thread_ids` vector
        summary["Thread " + to_string(i + 1)] = 0;  // initialize in `summary` map
    }

    // (7) Keep executing all jobs in `all_jobs` until no jobs left
    string next_job;
    int input_num;
    while (!all_jobs.empty()) {
        pthread_mutex_lock(&jobs_queue_lock);
        if (jobs_queue.size() == 2 * nthreads - 1)                  // wait if jobs_queue is full
            pthread_cond_wait(&jobs_queue_full, &jobs_queue_lock);  // Producers use condition variable to wait for the queue to have spaces
        pthread_mutex_unlock(&jobs_queue_lock);

        // Get next job from `all_jobs`
        next_job = all_jobs.front();  // get next job
        all_jobs.pop_front();         // remove from queue

        input_num = atoi(next_job.substr(1).c_str());
        if (next_job[0] == 'T') {
            pthread_mutex_lock(&jobs_queue_lock);
            // Add new transaction 'T' from `all_jobs` to `jobs_queue`
            jobs_queue.push_back(next_job);  // add to jobs_queue

            // Signal that queue is not empty. Producers notify consumers if there is a new item
            pthread_cond_signal(&jobs_queue_empty);
            pthread_mutex_unlock(&jobs_queue_lock);

            // [LOG]: Work
            write_log_line(get_time(), 0, jobs_queue.size(), "Work", input_num);
        } else if (next_job[0] == 'S') {
            // [LOG]: Sleep
            write_log_line(get_time(), 0, -1, "Sleep", input_num);
            // S<n> producer waits
            Sleep(input_num);

        } else {
            // Skip on invalid new_job
            fprintf(stderr, "%s is not valid", next_job.c_str());
        }
    };

    // (8) Signal end of new jobs to all threads
    // Notify all threads that no more jobs will be added to `jobs_queue`
    write_log_line(get_time(), 0, -1, "End", -1);  // End of input for producer

    // Signal to all threads that if `jobs_queue` if empty to exit because no new jobs will be added
    pthread_mutex_lock(&jobs_queue_lock);
    // Add END job to jobs queue
    jobs_queue.push_back("END");
    pthread_mutex_unlock(&jobs_queue_lock);

    // (9) Wait for all threads to finish
    // When the producer reaches the end of file, then the program will end once all the consumers have completed their work.
    for (pthread_t t_id : thread_ids) {
        jobs_queue.push_back("END");
        // Release all `jobs_queue_empty` condition locks
        pthread_cond_signal(&jobs_queue_empty);
        pthread_join(t_id, NULL);
    }

    // (10) Write out summary
    write_summary();

    // (11) Exiting program
    LogFile.close();  // close file
    return 0;
}