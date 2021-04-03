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
#include <time.h>

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

struct mesg_buffer{
    long mesg_type;
    char mesg_text[100];
    int mesg_pid;
    int mesg_totalCPUTime;
    int mesg_totalTimeSystem;
    int mesg_lastBurst;
    int mesg_processPrio;
    int mesg_timeQuant;
    int mesg_timeUsed;
} message;

int shmidClock;
int shmidProc;


int main(int argc, char* argv[]){

    //Variable
    struct simClock *clock;
    struct processes *pTable;
    struct processes readyQueue[5];
    struct processes blockedQueue[5];
    int LEN = 18;
    int size = sizeof(pTable) * LEN;

    srand(time(NULL));

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

    
    
    int status = 0;
    char buffer[50] = "";

    int msgid;
    int msgidTwo;
    key_t messageKey = ftok("poggies", 65);
    key_t messageKeyTwo = ftok("homies",65);
    msgid = msgget(messageKey, 0666|IPC_CREAT);
    msgidTwo = msgget(messageKeyTwo, 0666|IPC_CREAT); 

    const int maxTimeBetweenNewProcsNS = 100;
    const int maxTimeBetweenNecProcsSecs = 1;

    //Randomly do a new process
    int interval = rand()%((maxTimeBetweenNewProcsNS - 1)+1);
    clock->clockNano+= interval;
    while(clock->clockNano >= 100){
        clock->clockNano-= 100;
        clock->clockSec+= 1;
    };

    

    //for(int i = 0; i < 4; i++){
        if(fork() == 0)
        {
            pTable[0].pid = getpid();
            cout << pTable[0].pid << " child pTable Pid" <<endl;
            
            cout << getpid() << " child getpid() pid" <<endl;
            execl("./user", buffer);
        }

    ofstream log("log.out");
    log << "OSS: Generating process with PID " << pTable[0].pid << " at time: " << clock->clockNano << " ns : " << clock->clockSec << "s \n";
    log.close();
    sleep(1); //This needs to be fixed, but we can leave it for now.
    pTable[0].processPrio = 1;
        
        
    //cout << pTable[0].pid << " ; oss PID" <<endl;
    
    
    
    
    //Randomly increase time by small amount for doing ready queue

    const int maxSystemTimeSpent = 15;
    interval = rand()%((maxSystemTimeSpent - 1)+1);
    clock->clockNano+= interval;
    while(clock->clockNano >= 100){
        clock->clockNano-= 100;
        clock->clockSec+= 1;
    };

    //Do ready Queue
    readyQueue[0].pid = pTable[0].pid;
    //cout << readyQueue[0].pid << " rq pid" <<endl;
    //cout << pTable[0].pid << " pt pid" <<endl;

    message.mesg_timeQuant = 50;
    message.mesg_type = readyQueue[0].pid;
    log.open("log.out", ios::app);
    log << "OSS: Process " << pTable[0].pid << " has entered ready queue \n";
    log.close();
    
    msgsnd(msgidTwo, &message, sizeof(message), 0);
    log.open("log.out",ios::app);
    log << "OSS: Process " << readyQueue[0].pid << " has been removed from ready queue \n";
    log.close();
    readyQueue[0].pid = -1;
    


    pid_t wpid;

    //while((wpid = wait(&status)) > 0){
        
        msgrcv(msgid, &message, sizeof(message), 1, 0);
        cout << message.mesg_text << endl;
        cout << "Process Prio: " << message.mesg_processPrio << endl;
        cout << "PID: " << message.mesg_pid <<endl;
        
        clock->clockNano+= message.mesg_timeUsed;
        while(clock->clockNano >= 100){
            clock->clockNano-= 100;
            clock->clockSec+= 1;
        };

        cout << clock->clockNano << "time passed" <<endl;
        cout << clock->clockSec << " time passed sec" <<endl;
    //}
    //}

    msgctl(msgid, IPC_RMID, NULL);
    shmctl(shmidClock, IPC_RMID, NULL);
    shmctl(shmidProc, IPC_RMID, NULL);

    return 0;
}


