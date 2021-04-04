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
    bool typeOfSystem;
    int blockRestartSec;
    int blockRestartNS;
    bool unblocked;
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
    bool mesg_terminated;
    bool mesg_typeOfSystem;
    bool mesg_blocked;
    int mesg_unblockNS;
    int mesg_unblockSec;
    int mesg_rqIndex;
} message;

int shmidClock;
int shmidProc;


int main(int argc, char* argv[]){

    //Variable
    struct simClock *clock;
    struct processes *pTable;
    struct processes readyQueue[20];
    struct processes blockedQueue[20];
    const int maxSystemTimeSpent = 15;
    const int billion = 1000000000;
    int LEN = 18;
    int size = sizeof(pTable) * LEN;
    int processesOpen = 0;
    int totalCreated = 0;
    ofstream log("log.out");
    log.close();
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

    //Initialize ready queue, blocked queue, and ptable queue to empty pid identifier.
    

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
        //perror("Shared memory attach");
        //return 1;
    }

    //for(int i = 0; i < 18; i++){
        //pTable[i] = (processes*)shmat(shmidProc,NULL,0);
        //if(pTable == (void*) -1){
            //perror("Shared memory attach");
        //return 1;
    //}
    //}

    for(int i = 0; i < 18; i++){
        readyQueue[i].pid = -1;
        blockedQueue[i].pid = -1;
        pTable[i].pid = -1;
    }

    
    
    int status = 0;
    char buffer[50] = "";

    int msgid;
    int msgidTwo;
    key_t messageKey = ftok("poggers", 65);
    key_t messageKeyTwo = ftok("homies",65);
    msgid = msgget(messageKey, 0666|IPC_CREAT);
    msgidTwo = msgget(messageKeyTwo, 0666|IPC_CREAT); 

    const int maxTimeBetweenNewProcsNS = 100;
    const int maxTimeBetweenNecProcsSecs = 1;
    const int outofOneHund = 100;

    //Randomly do a new process
    int interval = rand()%((maxTimeBetweenNewProcsNS - 1)+1);
    clock->clockNano+= interval;
    while(clock->clockNano >= billion){
        clock->clockNano-= billion;
        clock->clockSec+= 1;
    };

    int typeOfSystemNum = rand()%((outofOneHund - 1)+1);

        //do{
            //cout << "Start Do" <<endl;
            //Checks the blocked queue, if an index in the blocked queue is ready to be unblocked, set it to unblocked. 
            for(int i = 0; i < 18 ; i++){
                if(blockedQueue[i].blockRestartSec < clock->clockSec || (blockedQueue[i].blockRestartSec = clock->clockSec && blockedQueue[i].blockRestartNS > clock->clockNano)){
                    blockedQueue[i].unblocked = true;
                }
            }
            //The first index in the blocked queue that is unblocked will be moved to the first available ready queue position.
            for(int i = 0; i < 18; i++){
                if(blockedQueue[i].unblocked = true){
                    for(int j = 0; j < 18; j++){
                        if(readyQueue[j].pid = -1){
                            readyQueue[j].pid = blockedQueue[i].pid;
                            j = 18;
                        }
                    }
                i = 18;
                }   
            }

            //cout << "Gen Process" << endl;
            //Generate a new process if processTable isn't empty

            for(int i = 0; i < 18; i++){
                //cout << "Enter For" << endl;
                //cout << pTable[i].pid << " pTablepid" << endl;
                if(pTable[i].pid == -1 && totalCreated != 100){
                    //cout << "Enter if Pid" << endl;
                    totalCreated++;
                    //cout << totalCreated << " ; total created" <<endl;
                    if(fork() == 0){

                        //Handles all new process creation
                        interval = rand()%((maxTimeBetweenNewProcsNS - 1)+1);
                        clock->clockNano+= interval;
                        while(clock->clockNano >= billion){
                            clock->clockNano-= billion;
                            clock->clockSec+= 1;
                        };
                        log.open("log.out",ios::app);
                        log << "OSS: Generating process with PID " << getpid() << " at time: " << clock->clockSec << " s : " << clock->clockNano << "ns \n";
                        log.close();
                        //cout << "Enter Fork" <<endl;
                        typeOfSystemNum = rand()%((outofOneHund - 1)+1);
                        pTable[i].pid = getpid();
                        //cout << endl;
                        //cout << pTable[i].pid << " child pTable Pid" <<endl;
                        if(typeOfSystemNum >=51){
                            pTable[i].typeOfSystem = true;
                        }
                        else{
                            pTable[i].typeOfSystem = false;
                        }
                        //cout << getpid() << " child getpid() pid" <<endl;

                        for(int j = 0 ; j < 18; j++){
                            if(readyQueue[j].pid == -1){
                                readyQueue[j].pid = pTable[i].pid;
                                //cout << readyQueue[j].pid << " RQ PID WITHN FOR" << endl;
                                j = 18;
                            }
                        }
                        execl("./user", buffer);
                    }
                }
            }

            

            //cout << "Log to File" << endl;
            //log.open("log.out",ios::app);
            //log << "OSS: Generating process with PID " << pTable[0].pid << " at time: " << clock->clockNano << " ns : " << clock->clockSec << "s \n";
            //log.close();
            sleep(1); //This needs to be fixed, but we can leave it for now.
            pTable[0].processPrio = 1;

            for(int i = 0; i < 18; i++){
                cout << pTable[i].pid << " pTablepid: " << i << endl;
            }


            //Fills Ready Queue with Processes
            for(int i = 0 ; i < 18; i++){
                readyQueue[i].pid = pTable[i].pid;
                interval = rand()%((maxSystemTimeSpent - 1)+1);
                clock->clockNano+= interval;
                while(clock->clockNano >= billion){
                    clock->clockNano-= billion;
                    clock->clockSec+= 1;
                };
                log.open("log.out", ios::app);
                log << "OSS: Process " << readyQueue[i].pid << " has entered ready queue at time: " << clock->clockSec << "s : " << clock->clockNano << "ns \n";
                log.close();
                cout << readyQueue[i].pid << " rqPid: " << i << endl;
            }
            
            
            
            
            //Randomly increase time by small amount for doing ready queue
            interval = rand()%((maxSystemTimeSpent - 1)+1);
            clock->clockNano+= interval;
            while(clock->clockNano >= billion){
                clock->clockNano-= billion;
                clock->clockSec+= 1;
            };


            message.mesg_timeQuant = 50;

            //Handles Ready Queue Checks

            for(int i = 0; i < 18; i++){
                if(readyQueue[i].pid != -1){
                    message.mesg_type = readyQueue[i].pid;
                    message.mesg_rqIndex = i;
                    for(int j = 0; j < 18; j++){
                        if(pTable[j].pid = readyQueue[i].pid){
                            if(pTable[i].typeOfSystem == true)
                                message.mesg_typeOfSystem = true;
                            else
                            message.mesg_typeOfSystem = false;
                            j = 18;
                        }
                    }

                log.open("log.out",ios::app);
                log << "OSS: Process " << readyQueue[i].pid << " has been removed from ready queue \n";
                log.close();
                readyQueue[i].pid = -1;
                i = 18;
                }
            }
            

            //if(pTable[0].typeOfSystem == true)
                //message.mesg_typeOfSystem = true;
            //else
                //message.mesg_typeOfSystem = false;

            msgsnd(msgidTwo, &message, sizeof(message), 0);
            
            
            


            pid_t wpid;

            //while((wpid = wait(&status)) > 0){
                msgrcv(msgid, &message, sizeof(message), 2, 0);
                if(message.mesg_terminated == true){
                    cout << "Process " << message.mesg_pid << " terminated" <<endl;
                    //Remove the terminated process from process list.
                    for(int i = 0; i < 18; i++){
                        if(pTable[i].pid = message.mesg_pid){
                            pTable[i].pid = -1;
                            i = 18;
                        }
                    }
                    //Log the process removal
                    log.open("log.out",ios::app);
                    log << "OSS: Process " << message.mesg_pid << " has been removed from available processes" << endl;
                    log.close();
                }
                else{
                    if(message.mesg_blocked == true){
                        cout << "Process " << message.mesg_pid << " blocked" <<endl;
                        //Add blocked process to blocked queue
                        blockedQueue[0].pid = message.mesg_pid;
                        blockedQueue[0].blockRestartSec = clock->clockSec + message.mesg_unblockSec;
                        blockedQueue[0].blockRestartNS = clock->clockNano + message.mesg_unblockNS;
                        log.open("log.out",ios::app);
                        log << "OSS: Process " << blockedQueue[0].pid << " has been added to blocked queue at index: 0" <<endl;
                        cout << blockedQueue[0].blockRestartSec << " ; time to restart sec" <<endl;
                        cout << blockedQueue[0].blockRestartNS << " ; time to restart ns" <<endl;
                        log.close();
                    }
                    else{
                        cout << message.mesg_text << endl;
                        cout << "Process Prio: " << message.mesg_processPrio << endl;
                        cout << "PID: " << message.mesg_pid <<endl;
                        //Remove process from process queue
                        for(int i = 0 ; i < 18; i++){
                            if(pTable[i].pid == message.mesg_pid){
                                pTable[i].pid = -1;
                                i = 18;
                            }
                        }
                    }
                }
                
                clock->clockNano+= message.mesg_timeUsed;
                while(clock->clockNano >= billion){
                    clock->clockNano-= billion;
                    clock->clockSec+= 1;
                };
                processesOpen = 0;
                for(int i = 0; i < 18; i++){
                    if(pTable[i].pid == -1)
                        processesOpen++;
                }

                cout << processesOpen << " processesOpen" <<endl;


        //}while(processesOpen != 18);

        //cout << clock->clockNano << "time passed" <<endl;
        //cout << clock->clockSec << " time passed sec" <<endl;
    //}
    //}

    msgctl(msgid, IPC_RMID, NULL);
    msgctl(msgidTwo, IPC_RMID,NULL);
    shmctl(shmidClock, IPC_RMID, NULL);
    shmctl(shmidProc, IPC_RMID, NULL);

    return 0;
}


