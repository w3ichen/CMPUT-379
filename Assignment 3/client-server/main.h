#include <chrono>
#include <fstream>
#include <map>

#ifndef MAIN_H  // Guard against multiple delcarations
#define MAIN_H

using namespace std;

#define SERVER_MESSAGE_SIZE 2000
#define TIMEOUT 30

// tands.cpp
void Sleep(int n);
void Trans(int n);

// utils.cpp
/**
 * @brief Get port number from command line arguments
 * 
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @return port number as integer
 */
int get_port(int argc, char *argv[]);
/**
 * @brief Get the port and ip address from command line arguments
 * 
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @param port variable to set the port number
 * @param ip_address variable to set the ip address
 */
void get_port_and_ip_address(int argc, char *argv[], int &port, string &ip_address, ofstream &LogFile);
/**
 * @brief print server log line to stdout
 * 
 * @param trans_id transaction id
 * @param cmd T/Done
 * @param machine_name name of client machine
 * @param LogFile log file to write output
 */
void write_server_log(int trans_id, string cmd, string machine_name, ofstream &LogFile);
/**
 * @brief print client log to stdout
 * 
 */
void write_client_log(string type, string cmd, ofstream &LogFile);
/**
 * @brief split message by delimiter
 * 
 * @param message string to split
 * @param machine_name name of machine that message is coming from
 * @param cmd command that server should execute
 */
void split_message(string message, string &machine_name, string &cmd);
/**
 * @brief Write symmary to end of server log file
 * 
 * @param trans_sent_map map of how many transactions each client sent
 * @param start_time time that server started
 * @param ServerLogFile Log file to write to
 */
void write_summary(map<string, int> trans_sent_map, chrono::high_resolution_clock::time_point start_time, ofstream &LogFile);
/**
 * @brief Clean string of \t delimiter
 * 
 * @param str string to clean
 */
void clean_string(string &str);
#endif