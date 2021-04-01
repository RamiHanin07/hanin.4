#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <sys/wait.h>
#include <csignal>
#include <bits/stdc++.h>
#include <sys/msg.h>
#include <string>
#include <ctime>

using namespace std;

struct simClock{
    int clockSec;
    int clockNano;
};

struct processes{
    int pid;
};

struct mesg_buffer{
    long mesg_type;
    char mesg_text[100];
    int mesg_index;
    int mesg_pid;
    char mesg_string[100];
} message;

int shmidClock;
int shmidProc;

int main(int argc, char* argv[]){

    //Variable
    struct simClock *clock;
    struct processes *pTable;
    int LEN = 18;
    int size = sizeof(pTable) * LEN;
    //Command Line Parsing
    string argNext = "";
    for(int i  = 1; i < argc; i++){
        string arg = argv[i];
        if(arg == "-h"){
            cout << "Help:" << endl;
            cout << "master [-h] [-s t] [-l f]" <<endl;
            cout << " -h : Help Options" <<endl;
            cout << " -s : Indicate how many maximum seconds before the system terminates" <<endl;
            cout << " -l : Specify a particular name for the log file" <<endl;
            cout << "Example run: ./oss -s 3 -l log.txt" <<endl;
            return 0;
        } else if(arg == "-s"){
            argNext = argv[i+1];
            cout << "-s " << argNext << endl;
        } else if(arg == "-l"){
            argNext = argv[i+1];
            cout << "-l " << argNext << endl;
        }
    }


    //Shared Memory Creation for System Clock (Seconds)
    int sizeMem = 1024;
    key_t keyClock = 786575;

    shmidClock = shmget(keyClock, sizeof(struct simClock), 0644|IPC_CREAT);
    if (shmidClock == -1) {
        perror("Shared memory");
        return 1;
    }

    clock = (simClock*)shmat(shmidClock,NULL,0);
    if(clock == (void*) -1){
        perror("Shared memory attach");
        return 1;
    }

    //Shared Memory Creation for Process Table 
    key_t keyProc = 76589;

    shmidProc = shmget(keyProc, size, 0644|IPC_CREAT);
    if (shmidProc == -1) {
        perror("Shared memory");
        return 1;
    }

    pTable = (processes*)shmat(shmidProc,NULL,0);
    if(pTable == (void*) -1){
        perror("Shared memory attach");
        return 1;
    }

    //Message Queues
    key_t messageKey = ftok("poggers", 67);
    int msgid;
    char buffer[50] = "";
    if(fork() == 0)
        execl("./user", buffer);

    msgid = msgget(messageKey, 0666|IPC_CREAT);

    msgrcv(msgid, &message, sizeof(message), 1, 0);

    cout << message.mesg_text << endl;

    return 0;
}