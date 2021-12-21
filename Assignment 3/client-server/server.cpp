#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "main.h"

#define MAX_CLIENTS 10
using namespace std;

ofstream ServerLogFile;

int main(int argc, char *argv[]) {
    // (1) Get port from command arguments
    int port = get_port(argc, argv);

    // (2) Variable definitions
    int master_socket, client_sockets[MAX_CLIENTS], new_socket, addrlen, opt = 1, max_sd, sd, i, activity, n, valread;
    struct sockaddr_in server, client;
    char client_message[SERVER_MESSAGE_SIZE];
    map<string, int> trans_sent_map;
    int trans_id = 0;  // Transaction id
    fd_set readfds;    // set of socket descriptors
    char buffer[1025];
    string server_response, machine_name, cmd;
    bool initialized = false;
    chrono::high_resolution_clock::time_point start_time;

    // (3) Create server log file
    ServerLogFile.open("server.log", ofstream::out | ofstream::trunc);
    ServerLogFile << "Using port " << port << endl;

    // (4) Create master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("[ERROR] Could not create socket");
        exit(EXIT_FAILURE);
    }
    // set master socket to allow multiple connections
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0) {
        perror("[ERROR] setsockopt");
        exit(EXIT_FAILURE);
    }

    // (5) initialise all client_sockets[] to 0 so not checked
    for (i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // (6) Configure socket: prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    // (7) Bind master socket
    if (bind(master_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        // print the error message
        perror("[ERROR] bind failed");
        exit(EXIT_FAILURE);
    }

    // (8) Listen up to 3 pending connections
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(struct sockaddr_in);
    // (9) Timeout of server after 30 seconds of idling
    struct timeval timeout, timeout_copy;
    timeout.tv_sec = TIMEOUT;  // 30 seconds
    timeout.tv_usec = 0;

    // (10) Infinite server loop
    while (true) {
        // clear the socket set
        FD_ZERO(&readfds);
        // add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add child sockets to set
        for (i = 0; i < MAX_CLIENTS; i++) {
            // socket descriptor
            sd = client_sockets[i];
            // if valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);
            // highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        memcpy(&timeout_copy, &timeout, sizeof(timeout));  // reset timeout b/c select() can mutate it
        // Wait for any activity in the servers, select() is blocking
        activity = select(max_sd + 1, &readfds, NULL, NULL, &timeout_copy);
        if ((activity < 0) && (errno != EINTR))
            perror("[ERROR] select error");
        else if (activity == 0)
            break;  // TIMEOUT

        // Start timer if not initialized
        if (!initialized) {
            start_time = chrono::high_resolution_clock::now();
            initialized = true;
        }

        // incoming connection if something happened on the master socket
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&client, (socklen_t *)&addrlen)) < 0) {
                perror("[ERROR] accept failed");
                exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            cout << "New connection       | ip: " << inet_ntoa(client.sin_addr) << " | port: " << ntohs(client.sin_port) << endl;

            // add new socket to array of sockets
            for (i = 0; i < MAX_CLIENTS; i++) {
                // if position is empty
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // else its some IO operation on some other socket
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                // Check if it was for closing , and also read the incoming message
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // Somebody disconnected , get its details and print
                    getpeername(sd, (struct sockaddr *)&client,
                                (socklen_t *)&addrlen);
                    cout << "Client disconnected  | ip: " << inet_ntoa(client.sin_addr) << " | port: " << ntohs(client.sin_port) << endl;

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_sockets[i] = 0;
                }

                // Receive message from client
                else {
                    // (1) Parse client response
                    split_message(string(buffer), machine_name, cmd);

                    n = atoi(cmd.substr(1).c_str());
                    // (2) Get new transaction number
                    trans_id++;
                    server_response = "D" + to_string(trans_id) + "\t";
                    // (3) Log the transaction
                    write_server_log(trans_id, cmd, machine_name, ServerLogFile);
                    // (4) Execute the transaction
                    Trans(n);
                    // (5) Send the `server_response` back to client
                    write(sd, server_response.c_str(), server_response.length());

                    // (6) Log done
                    write_server_log(trans_id, "Done", machine_name, ServerLogFile);

                    // (7) Increment transactions from
                    if (trans_sent_map.find(machine_name) == trans_sent_map.end())
                        trans_sent_map[machine_name] = 1;  // Not in summary yet
                    else
                        trans_sent_map[machine_name]++;  // Increment transaction count
                }
            }
        }
    }
    cout << "Exiting server" << endl;
    // Write summary
    write_summary(trans_sent_map, start_time, ServerLogFile);

    // Close server log file
    ServerLogFile.close();
    return 0;
}
