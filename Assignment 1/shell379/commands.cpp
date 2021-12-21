#include "main.h"
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

void jobs(vector<map<string, string>> &ProcessTable)
{
     map<string, string> process;
     string status;
     int ctn = 0;
     bool printedHeaders = false;
     int pTime;

     cout << "Running processes:" << endl;

     auto i = ProcessTable.begin();
     while (i != ProcessTable.end())
     {
          // Remove dead process for ProcessTable
          process = *i;
          status = executeShellCmd("ps -p " + process["pid"] + " -o stat=");

          if (status.empty() || status.at(0) == 'Z') // Z is zombie
          {
               // Is empty, then process does not exist
               i = ProcessTable.erase(i); // Erase from table
          }
          else
          {
               if (!printedHeaders)
               {
                    // Headers
                    cout << "  #    PID  S   SEC  COMMAND" << endl;
                    printedHeaders = true;
               }
               pTime = getPidTime(process["pid"]);
               // Print out each of the processes
               cout << setw(3) << right << ctn << ":"
                    << setw(6) << right << process["pid"]
                    << setw(3) << right << status.at(0) << setw(6) << right << pTime
                    << "  " << process["raw"] << endl;
               ctn++; // increment counter
               ++i;   //increase iterator
          }
     }
     cout << "Processes =" << setw(8) << right << ctn
          << " active" << endl;
     cout << "Completed processes:" << endl;
     printSysUserTimes();
}

void executeCommand(map<string, string> userInput)
{
     vector<string> args = split(userInput["args"]);
     int inputFile, outputFile;
     if (userInput.find("inputFile") != userInput.end())
     {
          // If input file, redirect stdin to input file
          if ((inputFile = open(userInput["inputFile"].c_str(), O_RDONLY)) < 0)
          {
               fprintf(stderr, "[ERROR] %s does not exist\n", userInput["inputFile"].c_str());
               close(inputFile);
               exit(0); // exit child process
          }
          dup2(inputFile, STDIN_FILENO); // make stdin go to file
          close(inputFile);              // Make sure to close file or will have segmentation fault
     }
     if (userInput.find("outputFile") != userInput.end())
     {
          // Redirect output of execvp to output file
          outputFile = open(userInput["outputFile"].c_str(), O_CREAT | O_TRUNC | O_RDWR);
          dup2(outputFile, STDOUT_FILENO); // make stdout go to file
          close(outputFile);               // Make sure to close file or will have segmentation fault
     }

     // construct argsPtrs array
     args.insert(args.begin(), userInput["command"]); // insert command to first
     char *argsPtrs[args.size()];
     argsPtrs[args.size()] = NULL; // push NULL to end
     for (int i = 0; i < args.size(); i++)
     {
          argsPtrs[i] = (char *)args[i].c_str();
     }

     // Execute command
     if (execvp(argsPtrs[0], argsPtrs) < 0)
          perror("[ERROR]");
}

void exitProgram(vector<map<string, string>> ProcessTable)
{
     map<string, string> process;
     string status;
     for (auto i = ProcessTable.begin(); i != ProcessTable.end();)
     {
          process = *i;
          status = executeShellCmd("ps -p " + process["pid"] + " -o stat=");

          if (status.at(0) != 'T')
          {
               // wait on running programs if not suspended
               waitpid(stoi(process["pid"]), NULL, 0);
          }
          // Kill all processes
          kill(stoi(process["pid"]), SIGKILL);
          ++i;
     }
     // Print out the total user and system time for all processes run by the shell.
     cout << "Resources used" << endl;
     printSysUserTimes();
     exit(0); // End the execution of shell379
}