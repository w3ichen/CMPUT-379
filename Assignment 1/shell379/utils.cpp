#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/resource.h>
#include <iomanip>
#include "main.h"
#include <stdio.h>
using namespace std;

vector<string> split(string str)
{
    vector<string> words;
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(words));
    return words;
}

int getPidTime(string pid)
{
    // bash command: ps -p [pid] -o time=
    //    -p [pid] specifies the pid to get
    //    -o time  gets the time
    //    time= suppresses the TIME header

    string cmd = "ps -p " + pid + " -o time=";
    string timeString = executeShellCmd(cmd);
    // timeString is in HH:MM:SS format, need to convert to seconds
    int h, m, s = 0;
    sscanf(timeString.c_str(), "%d:%d:%d", &h, &m, &s);
    return h * 3600 + m * 60 + s;
}

string executeShellCmd(string cmd)
{
    FILE *pfile;
    char *output = new char[128];
    array<char, 128> buffer;
    const char *command = cmd.c_str();

    if ((pfile = popen(command, "r")) == NULL)
    {
        perror("Pipe open failed\n");
        return "";
    }
    else
    {
        while (fgets(buffer.data(), 128, pfile) != NULL)
            strcpy(output, buffer.data());

        if (pclose(pfile) < 0)
            perror("Failed to close pipe");
        return output;
    }
    return "";
}

void printSysUserTimes()
{
    struct rusage usage; // system usage
    getrusage(RUSAGE_CHILDREN, &usage);
    cout << "User time =" << setw(8) << right << usage.ru_utime.tv_sec << " seconds" << endl;
    cout << "Sys  time =" << setw(8) << right << usage.ru_stime.tv_sec << " seconds" << endl;
}

void removeByPid(vector<map<string, string>> &ProcessTable, pid_t pid)
{
    ProcessTable.erase(
        remove_if(ProcessTable.begin(), ProcessTable.end(),
                  [pid](map<string, string> m)
                  {
                      return m["pid"] == to_string(pid);
                  }),
        ProcessTable.end());
}