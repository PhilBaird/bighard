#include <stdio.h>
#include <stdlib.h>
// Created by plipm on 2/3/2021.
//

// input
// PID_1 Arrival_Time_1 Total_CPU_Time_1 I/O_Frequency_1 I/O_Duration_1
// output
// Time_Of_Transition_1 PID_1 Old_State_1 New_State_1

#define n 20

struct Process {
    int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration;
};

struct Process ready[n], running[n], waiting[n];
int readyLen = 0, runningLen = 0, waitingLen = 0, tick = 0;

int main(void) {
    while(tick < 100){
        if(readyLen > 0){
            struct Process* ptr = ready;
            for (int i=0; i<readyLen; i++, ptr++ ) {

            }
        }
        if(runningLen > 0){
            struct Process* ptr = running;
            for (int i=0; i<runningLen; i++, ptr++ ) {
                ptr->Total_CPU_Time--;
            }
        }
        if(waitingLen > 0){
            struct Process* ptr = waiting;
            for (int i=0; i<waitingLen; i++, ptr++ ) {

            }
        }
        tick++;
    }
    return 0;
}

int admitted(struct Process* process) {
    // gone -> ready
    ready[readyLen] = process;
    readyLen ++;
}

int dispatch(int PID) {
    // ready -> running
    struct Process* ptr = ready;
    for (int i=0; i<readyLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            running[runningLen] = ptr;
            runningLen ++;
            this.remove_element(ready, i, readyLen)
            readyLen --;
        }
    }
}

int wait(int PID){
    // running -> waiting
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            waiting[waitingLen] = ptr;
            waitingLen ++;
            this.remove_element(running, i, runningLen)
            runningLen --;
        }
    }
}

int done(int PID){
    // waiting -> ready
    struct Process* ptr = waiting;
    for (int i=0; i<waitingLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            ready[readyLen] = ptr;
            readyLen ++;
            this.remove_element(waiting, i, waitingLen)
            waitingLen --;
        }
    }
}

int interrupt(int PID){
    // running -> ready
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            ready[readyLen] = ptr;
            readyLen ++;
            this.remove_element(running, i, runningLen)
            runningLen --;
        }
    }
}

int terminate(int PID){
    // running -> gone
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            this.remove_element(running, i, runningLen)
            runningLen --;
        }
    }
}

void remove_element(struct Process *array, int index, int array_length){
    for(int i = index; i < array_length - 1; i++) array[i] = array[i + 1];
}

