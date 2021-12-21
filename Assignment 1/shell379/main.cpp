#include "main.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <map>
#include <time.h>
#include <signal.h>

using namespace std;

vector<map<string, string>> ProcessTable;
int MAX_PT_ENTRIES = 32; // Max entries in the Process Table

int main()
{
    pid_t newPid;
    map<string, string> userInput;
    string command;
    vector<string> args;

    while (true)
    {
        // (1) Get user input
        userInput = getUserInput();
        command = userInput["command"];
        args = split(userInput["args"]);
        // (2) Find and execute the command
        if (command == "exit")
        {
            exitProgram(ProcessTable);
        }
        else if (command == "jobs")
        {
            jobs(ProcessTable);
        }
        else if (command == "kill")
        {
            // Kill process <parameter>
            if (args.empty())
                cout << "[ERROR] must enter a <pid>" << endl;
            else
                kill(stoi(args[0]), SIGKILL);
        }
        else if (command == "resume")
        {
            // Resume the execution of process <int>. This undoes a suspend.
            if (args.empty())
                cout << "[ERROR] must enter a <pid>" << endl;
            else
                kill(stoi(args[0]), SIGCONT);
        }
        else if (command == "sleep")
        {
            // Sleep for <int> seconds
            if (args.empty())
                cout << "[ERROR] must enter a <pid>" << endl;
            else
                sleep(stoi(args[0]));
        }
        else if (command == "suspend")
        {
            // Suspend execution of process <int>. A resume will reawaken it
            if (args.empty())
                cout << "[ERROR] must enter a <pid>" << endl;
            else
                kill(stoi(args[0]), SIGSTOP);
        }
        else if (command == "wait")
        {
            // Wait until process <int> has completed execution
            if (args.empty())
                cout << "[ERROR] must enter a <pid>" << endl;
            else
                waitpid(stoi(args[0]), NULL, 0);
        }
        else
        {
            // Spawn a process to execute command <cmd> with 0 or more arguments
            newPid = fork();
            if (newPid == 0)
            {
                // Child
                executeCommand(userInput); // fork a new process and then call another function
                exit(0);
            }
            else
            {
                // Parent
                userInput["pid"] = to_string(newPid);
                // Add child to ProcessTable
                ProcessTable.push_back(userInput);

                if (userInput["background"] != "true")
                    waitpid(newPid, NULL, 0); // wait for child to finish
            }
        }
        // break;
    }

    return 0;
}