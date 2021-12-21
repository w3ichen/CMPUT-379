#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

#include "main.h"

using namespace std;

pthread_mutex_t summary_lock = PTHREAD_MUTEX_INITIALIZER;  // locks summary object

// Increments value in `summary` map
void summary_increment(string map_key) {
    pthread_mutex_lock(&summary_lock);
    summary[map_key]++;
    pthread_mutex_unlock(&summary_lock);
}

// Function executed by each thread
void *consumer_execution(void *thread_id) {
    string new_job;
    int input_num;

    while (true) {
        // [LOG]: Ask
        write_log_line(get_time(), (intptr_t)thread_id, -1, "Ask", -1);
        summary_increment("Ask");  //  Consumer: # of asks for work

        // If the next job is the `END`, then exit this thread
        if (jobs_queue.front() == "END") break;

        // (1) Obtain new job from `jobs_queue`
        pthread_mutex_lock(&jobs_queue_lock);

        // Wait thread if no jobs. Consumers use condition variable to wait for the queue to be filled
        while (jobs_queue.size() == 0) {
            pthread_cond_wait(&jobs_queue_empty, &jobs_queue_lock);
        }

        // Safeguard against reading from empty `jobs_queue` after `pthread_cond_wait` is released
        if (jobs_queue.empty()) break;

        // Receiving new job here
        new_job = jobs_queue.front();  // get next job
        jobs_queue.pop_front();        // remove from queue

        // Consumers notify producers when they removed items from a full queue
        pthread_cond_signal(&jobs_queue_full);
        pthread_mutex_unlock(&jobs_queue_lock);

        input_num = atoi(new_job.substr(1).c_str());  // get `n`

        // [LOG]: Receive
        write_log_line(get_time(), (intptr_t)thread_id, jobs_queue.size(), "Receive", input_num);
        summary_increment("Receive");  // Consumer: # work assignments

        // (2) Execute new transaction
        // T<int> execute transaction
        Trans(input_num);
        summary_increment("Thread " + to_string((intptr_t)thread_id));  // Number of ‘T’s completed by this thread

        // [LOG]: Complete
        write_log_line(get_time(), (intptr_t)thread_id, -1, "Complete", input_num);
        summary_increment("Complete");  // Consumer: # completed tasks
    }

    pthread_exit(0);  // terminate thread
}

pthread_t consumer_thread(int i) {
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, consumer_execution, (void *)i))
        perror("Thread create error");
    return thread_id;
}
