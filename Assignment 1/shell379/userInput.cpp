#include "main.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <map>

using namespace std;

// Constants
int LINE_LENGTH = 100; // Max # of characters in an input line
int MAX_ARGS = 7;      // Max number of arguments to a command
int MAX_LENGTH = 20;   // Max # of characters in an argument

map<string, string> getUserInput()
{
    string userInput;
    // (1) Get user input
    printf("SHELL379: ");
    getline(cin, userInput);

    // (2) Validate user input against constants
    if (!isUserInputValid(userInput))
        return getUserInput(); // if not validate, re-prompt for input

    // (3) Parse the input string
    return parseUserInput(userInput);
}

bool isUserInputValid(string str)
{
    if (str.size() == 0)
    {
        // Assertion #0: must not be empty
        return false;
    }

    vector<string> args = split(str);
    args.erase(args.begin()); // Remvoe the first element which is the command

    if (str.size() > LINE_LENGTH)
    {
        // Assertion #1: character length
        fprintf(stderr, "[ERROR] line length exceeds %d characters\n", LINE_LENGTH);
        return false;
    }
    else if (args.size() > MAX_ARGS)
    {
        // Assertion #2: number of arguments
        fprintf(stderr, "[ERROR] number of arguments exceeds %d\n", MAX_ARGS);
        return false;
    }
    // Assertion #3: number of characters in an argument
    for (string arg : args)
        if (arg.size() > MAX_LENGTH)
        {
            fprintf(stderr, "[ERROR] length of %s exceeds %d\n", arg.c_str(), MAX_LENGTH);
            return false;
        }
    return true;
}

map<string, string> parseUserInput(string inputStr)
{
    string str(inputStr); // fixes segmentation fault

    vector<string> words = split(str);
    map<string, string> parsedValues;
    parsedValues["raw"] = str; // raw string
    string args("");           // string of arguments

    parsedValues["command"] = words[0]; // first is always the command
    words.erase(words.begin());         // remove from command

    // Special Arguments
    // (i) & is run in background and must be the last entry
    if (words.back() == "&")
    {
        parsedValues["background"] = "true";
        words.erase(words.end()); // remove from vector
    }
    if (words.empty())
    {
        return parsedValues;
    }

    for (string word : words)
    {
        if (word[0] == '<')
        {
            // (ii) <fname is input file
            string inputFileStr = word.substr(1);
            parsedValues["inputFile"] = inputFileStr;
        }
        else if (word[0] == '>')
        {
            // (iii) >fname is output file
            string outputFileStr = word.substr(1);
            parsedValues["outputFile"] = outputFileStr;
        }
        else
        {
            // Another argument
            args += " ";  // space separated
            args += word; // add word to args
        }
    }

    parsedValues["args"] = args;

    // for (auto p : parsedValues)
    // {
    //     cout << p.first << ": " << p.second << endl;
    // }

    return parsedValues;
}