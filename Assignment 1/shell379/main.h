#ifndef MAIN_H // Guard against multiple delcarations
#define MAIN_H

#include <string>
#include <vector>
#include <map>

using namespace std;

// Global variables
extern vector<map<string, string>> ProcessTable;

int main();

// userInput.cpp
/**
 * Gets and parses the user input from the command line
 * Returns a map with parsed info
 */
map<string, string> getUserInput();
/**
 * Returns whether the user input string is validate or not
 * according to the constants
 */
bool isUserInputValid(string str);
/**
 * Parses through user input string to get a map of parsed info
 */
map<string, string> parseUserInput(string str);

// utils.cpp
/**
 * Splits a string by spaces and returns a vector
 */
vector<string> split(string str);
/**
 * Input pid number and returns the time in seconds of the process
 * Uses popen pipe and ps to get the time
 */
int getPidTime(string pid);
/**
 * Prints the User and Sys time using `getrusage()`
 */
void printSysUserTimes();
/**
 * Remove process from ProcessTable by pid
 */
void removeByPid(vector<map<string, string>> &ProcessTable, pid_t pid);
/**
 * Executes shell command using `popen` and returns the string output
 */
string executeShellCmd(string cmd);

// commands.cpp
/**
 * Display the status of all running processes spawned by shell379
 */
void jobs(vector<map<string, string>> &ProcessTable);
/**
 * Executes command and prints the output
 */
void executeCommand(map<string, string> userInput);
/**
 * Exit program: wait on active process and terminate suspended ones
 */
void exitProgram(vector<map<string, string>> ProcessTable);

#endif