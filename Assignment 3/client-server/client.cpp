#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "main.h"
using namespace std;

ofstream ClientLogFile;

int main(int argc, char *argv[]) {
    struct sockaddr_in server;
    char server_reply[SERVER_MESSAGE_SIZE];
    int trans_sent = 0;  // number of transactions sent
    int sock, port;
    string ip_address, message;

    // Get host name
    char hostname[2000];
    gethostname(hostname, 2000);
    string machine_name = string(hostname) + "." + to_string(getpid());

    // Create log file
    ClientLogFile.open(machine_name + ".log", ofstream::out | ofstream::trunc);

    // Get port and ip address
    get_port_and_ip_address(argc, argv, port, ip_address, ClientLogFile);

    ClientLogFile << "Host " << machine_name << endl;
    cout << "Host " << machine_name << endl;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("Could not create socket");

    server.sin_addr.s_addr = inet_addr(ip_address.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("[ERROR] Connection failed");
        return 1;
    }

    cout << "Connected" << endl;

    int n;
    // Keep communicating with server until no inputs left
    while (!cin.eof() && getline(cin, message)) {
        // Clean message of \r and \n
        if (!message.empty() && (message[message.size() - 1] == '\r' || message[message.size() - 1] == '\n'))
            message.erase(message.size() - 1);

        // Parse message
        if (message[0] == 'T') {
            // Transaction
            // (1) Increment number of transactions
            trans_sent++;
            // (2) Log
            write_client_log("Send", message, ClientLogFile);
            // (3) Send transaction to server
            message = machine_name + "\t" + message + "\t";  // Prepend machine name to message
            if (send(sock, message.c_str(), message.length(), 0) < 0) {
                perror("Send failed");
                return 1;
            }

            // (4) Await server response
            // Receive a reply from the server
            if (recv(sock, server_reply, SERVER_MESSAGE_SIZE, 0) < 0) {
                perror("recv failed");
                break;
            }
            write_client_log("Recv", server_reply, ClientLogFile);
        } else if (message[0] == 'S') {
            // Sleep for `n` units
            n = atoi(message.substr(1).c_str());
            ClientLogFile << "Sleep " << n << " units" << endl;
            Sleep(n);
        } else {
            // Invalid
            cout << "[SKIP] \'" << message << "\' is not a recognized command" << endl;
            continue;  // skip
        }
    }

    // Exiting
    // Write number of transactions sent
    ClientLogFile << "Sent " << trans_sent << " transactions" << endl;
    // Close socket
    close(sock);
    // Close log file
    ClientLogFile.close();
    return 0;
}