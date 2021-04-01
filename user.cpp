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

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
    int mesg_index;
    int mesg_pid;
    char mesg_string[100];
}message;

int shmidClock;
int shmidProc;

int main(int argc, char* argv[]){
    

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
    key_t messageKey;
    int MAX = 10;

    messageKey = ftok("poggers", 67);

    //Change values of message queue to send 
    msgid = msgget(messageKey, 0666 | IPC_CREAT);
    message.mesg_type = 1;
    message.mesg_pid = getpid();

    strcpy(message.mesg_text, "Data Receieved");

    msgsnd(msgid, &message, sizeof(message), 0);
    //shmdt((void *) pTable);


}