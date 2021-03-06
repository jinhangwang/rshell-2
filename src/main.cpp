#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <string>
#include <boost/tokenizer.hpp>
#include <vector>
#include <queue>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
using namespace boost;



int rshell()
{
    char host[64];
    char *login = getlogin();
    gethostname(host,sizeof host);
    cout << login << '@' << host << "$ ";
    
    // cout << "$ ";
    
    string cmdstring;
    getline(cin, cmdstring);
    if (cmdstring.size() == 0) {
        return rshell();
    }
    if (cmdstring.find('#') != string::npos) {
        cmdstring.erase(cmdstring.find('#'), cmdstring.size() - 1);
    }
    while (cmdstring.at(0) == ' ') {
        cmdstring.erase(0,1);
    }
    if (cmdstring.size() > 3 && cmdstring.substr(0,4) == "exit") {
        exit(0);
    }
    string tempString;
    
    for (size_t t = 0; t < cmdstring.size(); t++) {
        if (cmdstring.at(t) != ' ') {
            tempString = tempString + cmdstring.at(t);
        }
        else tempString = tempString + "SPACE";
    }
    
    cmdstring = tempString;
    // cout << "cmdstring: "<< cmdstring << endl;
    // cout << "last char: " << cmdstring.at(cmdstring.size() - 1)<< endl;
    while (cmdstring.size() > 0 && (cmdstring.at(cmdstring.size() - 1) == ';' || cmdstring.at(cmdstring.size() - 1) == '|' || cmdstring.at(cmdstring.size() - 1) == '&' || cmdstring.at(cmdstring.size() - 1) == ' ')) {
        cmdstring.erase(cmdstring.size() - 1, 1);
    }
    if (cmdstring.size() == 0) {
        return rshell();
    }
    vector<string> tokens;
    char_separator<char> dash("SPACE","-|&; ");
    tokenizer<char_separator<char> > tok(cmdstring, dash);
    for (tokenizer<char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); iter++) {
        tokens.push_back(*iter);
        // cout << *iter << endl;
    }
    vector<string> tokensWithFlags;
    //vector<vector<string> > totalCMD;
    queue<vector<string> > totalCMD;
    size_t cmdPos = 0;
    for (size_t h = 0; h < tokens.size(); h++) {
        if (tokens.at(h) == "-" &&  (h + 1) < tokens.size()) {
            string temp = tokens.at(h) + tokens.at(h + 1);
            tokensWithFlags.push_back(temp);
            h++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == ";") {
            // cout << "CMD; Added " << tokensWithFlags.at(0) <<endl;
            if (tokensWithFlags.at(0) == "exit") return 0;
            //totalCMD.push_back(tokensWithFlags);
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == "&" && tokens.at(h + 1) == "&") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("&&");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            h++;
        }
        else if ((h + 1) < tokens.size() && tokens.at(h) == "|" && tokens.at(h + 1) == "|") {
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            tokensWithFlags.push_back("||");
            totalCMD.push(tokensWithFlags);
            tokensWithFlags.clear();
            cmdPos++;
            h++;
        }
        else tokensWithFlags.push_back(tokens.at(h));
    }
    // cout << "CMD Added " << tokensWithFlags.at(0) <<endl;
    if (tokensWithFlags.at(0) == "exit") return 0;
    //totalCMD.push_back(tokensWithFlags);
    totalCMD.push(tokensWithFlags);
    //for (size_t m = 0; m <= totalCMD.size(); m++) {
    while (!(totalCMD.empty())) {
        int pid = fork();
        //tokensWithFlags = totalCMD.back();
        //totalCMD.pop_back();
        tokensWithFlags = totalCMD.front();
        totalCMD.pop();
        vector<string> nextToken;
        bool isAND = false;
        bool isOR = false;
        if (!(totalCMD.empty())) {
            nextToken = totalCMD.front();
            // cout << "Next Tok: " << endl;
            if (nextToken.front() == "&&") {
                isAND = true;
                // cout << "AND TRUE" << endl;
                totalCMD.pop();
            }
            if (nextToken.front() == "||") {
                isOR = true;
                // cout << "OR TRUE" << endl;
                totalCMD.pop();
            }
        }
        
        char **ARGV = new char*[tokensWithFlags.size() + 1];
        for (size_t j = 0; j < tokensWithFlags.size(); j++) {
            ARGV[j] = new char[tokensWithFlags.at(j).size() + 1];
            strcpy(ARGV[j], tokensWithFlags.at(j).c_str());
            ARGV[tokensWithFlags.size()] = 0;
        }
        string CMD = ARGV[0];
        // cout << "CMD: " << CMD << endl;
        // cout << "PID: " << pid << endl;
        int status = 0;
        if (pid == -1) {
            perror("error in fork");
            exit(1);
            //return 1;
        }
        else if (pid == 0) {
            
            if (execvp(ARGV[0], ARGV) != 0) {
                perror("error in execvp");
                isOR = false;
                isAND = false;
                // if ((wait(&status) == -1)) 
                exit(1);
            }
            return 0;
        }
        else if (wait(&status) == -1) {
            perror("error in wait");
            //exit(1);
        }
        else if (totalCMD.empty()) {
            return rshell();
        }
        // if ((wait(&status) == -1)) {
        //     exit(1);
        // }
        // cout << "STATUS: " << status << endl;
        if (isOR && status == 0) {
            // cout << "POP OR" << endl;
            
            if (!(totalCMD.empty())) {
                totalCMD.pop();
            }
        }
        if (isAND && !(status == 0)) {
            // cout << "POP AND" << endl;
            
            if (!(totalCMD.empty())) {
                totalCMD.pop();
            }
        }
    }

    if (rshell() == 0) exit(0);
    return 0;
}

int main(int argc, char** argv)
{
    return rshell();
}
