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
    int totalCPUTime;
    int totalTimeSystem;
    int lastBurst;
    int processPrio;
    bool typeOfSystem;
    int blockRestartSec;
    int blockRestartNS;
    bool unblocked;
};

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
    int mesg_pid;
    int mesg_totalCPUTime;
    int mesg_totalTimeSystem;
    int mesg_lastBurst;
    int mesg_processPrio;
    int mesg_timeQuant;
    int mesg_timeUsed;
    bool mesg_terminated;
    bool mesg_typeOfSystem;
    bool mesg_blocked;
    int mesg_unblockNS;
    int mesg_unblockSec;
}message;

int shmidClock;
int shmidProc;

int main(int argc, char* argv[]){

    //cout << "enter user" <<endl;
    //Variables
    struct simClock *clock;
    struct processes *pTable;
    int LEN = 18;
    int size = sizeof(pTable) * LEN;
    srand(getpid());
    

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

    shmidProc = shmget(keyProc, sizeof(struct processes), 0644|IPC_CREAT);
    if (shmidProc == -1) {
        perror("Shared memory");
        return 1;
    }

    pTable = (processes*)shmat(shmidProc,NULL,0);
    if(pTable == (void*) -1){
        perror("Shared memory attach");
        return 1;
    }


    //Create Message Queue
    int msgid;
    int msgidTwo;
    key_t messageKey = ftok("poggers", 65);
    key_t messageKeyTwo = ftok("homies",65);
    int MAX = 10;



    //Change values of message queue to send 
    msgid = msgget(messageKey, 0666|IPC_CREAT);
    msgidTwo = msgget(messageKeyTwo, 0666|IPC_CREAT);

    message.mesg_type = getpid();

    cout << getpid() << " user pid" <<endl;

    
    ofstream log("log.out", ios::app);

    msgrcv(msgidTwo, &message, sizeof(message), message.mesg_type, 0);


    //Random states for the user process
    const int chanceToTerminate = 10;
    const int outOfOneHund = 100;
    int didItTerminate = rand()%((outOfOneHund - 1)+1);
    int timeUsed = rand()%((message.mesg_timeQuant - 1)+1);

    int chanceToBlock;
    int didItBlock;
    const int blockRestartSecMax = 5;
    const int blockRestartNSMax = 1000;
    int blockRestartSec = rand()%((blockRestartSecMax - 1)+1);
    int blockRestartNS = rand()%((blockRestartNSMax - 1)+1);
    



    message.mesg_timeUsed = timeUsed;
    message.mesg_pid = getpid();

    if(message.mesg_typeOfSystem == true) // True == CPU bound , false == io/bound. CPU Bound lower chance to block, IO bound higher chance to block
        chanceToBlock = 10;
    else
        chanceToBlock = 60;

    didItBlock = rand()%((outOfOneHund - 1)+1);


    if(didItTerminate <= chanceToTerminate){
        cout << "Process Terminated" <<endl;
        log << "User: Process " << getpid() << " has terminated unexpectedly \n";
        message.mesg_terminated = true;
        
    }
    else{
        if(didItBlock <= chanceToBlock){
            cout << "Process Blocked" <<endl;
            log << "User: Process " << getpid() << " has been blocked \n";
            message.mesg_blocked = true;
            message.mesg_unblockNS = blockRestartNS;
            message.mesg_unblockSec = blockRestartSec;
        }
        else{
            log << "User: Process " << getpid() << " has started working \n";
            cout << "Time Quant: " << message.mesg_timeQuant << endl;
            message.mesg_processPrio = pTable[0].processPrio;
            strcpy(message.mesg_text, "Data Receieved");
            log << "User: Process " << getpid() << " has stopped working \n";
        }
    }
    cout << message.mesg_processPrio << " ; user prio" <<endl;
    cout << message.mesg_pid << " ; user pid" << endl;
    message.mesg_type = 2;
    msgsnd(msgid, &message, sizeof(message), 0);


    
    //shmdt((void *) pTable);

    return 0;
}