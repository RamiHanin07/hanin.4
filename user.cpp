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


    //Create Message Queue
    int msgid;
    int msgidTwo;
    key_t messageKey = ftok("poggies", 65);
    key_t messageKeyTwo = ftok("homies",65);
    int MAX = 10;

    


    //Change values of message queue to send 
    msgid = msgget(messageKey, 0666|IPC_CREAT);
    msgidTwo = msgget(messageKeyTwo, 0666|IPC_CREAT);

    msgrcv(msgidTwo, &message, sizeof(message), 1, 0);

    cout << "Time Quant: " << message.mesg_timeQuant << endl;


    message.mesg_type = 1;
    message.mesg_pid = pTable[0].pid;
    message.mesg_processPrio = pTable[0].processPrio;

    strcpy(message.mesg_text, "Data Receieved");
    msgsnd(msgid, &message, sizeof(message), 0);
    //shmdt((void *) pTable);


}