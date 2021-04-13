#include <stdio.h>
#include <string.h>

// Created by plipm on 2/3/2021.
//

// input
// PID_1 Arrival_Time_1 Total_CPU_Time_1 I/O_Frequency_1 I/O_Duration_1
// output
// Time_Of_Transition_1 PID_1 Old_State_1 New_State_1

#define arrlen 20
#define ticks 120

struct Process {
    int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration, runningC,waitingC;
};

struct Process ready[arrlen], running[arrlen], waiting[arrlen];

void remove_element(struct Process array[arrlen], int index, int array_length);

void admitted(struct Process process);

void terminate(int PID);

void wait(int PID);

void dispatch(int PID);

void done(int PID);

void interrupt(int PID);

void readFile();

void readFile2();

const char* getField(char* line, int num);

int getline(char **pString, size_t *pInt, FILE *pIobuf);

int readyLen = 0, runningLen = 0, waitingLen = 0, tick = 0;
//int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration;
//int runningC, waitingC;
int main(void) {

    readFile();

    int breakout = 0;
    while(tick < ticks){
        breakout = 0;
//        printf("\nt: %i", tick );
        if(readyLen > 0){
            struct Process* ptr = ready;
            for (int i=0; i<readyLen; i++, ptr++ ) {
                if (runningLen == 0 && tick >= ptr->Arrival_Time  && breakout == 0) {
                    printf("\n%i -> dispatch: %i",tick, ptr->PID);
                    dispatch(ptr->PID);
                    breakout = 1;

                }
            }
        }
        if(runningLen > 0){
            struct Process* ptr = running;
            for (int i=0; i<runningLen; i++, ptr++ ) {
                if (ptr->runningC >= ptr->Total_CPU_Time  && breakout == 0){
                    printf("\n%i -> terminate: %i",tick, ptr->PID);
                    terminate(ptr->PID);
                    breakout = 1;
                }
                else if (ptr->runningC > 0 && ptr->runningC % ptr->IO_Frequency == 0  && breakout == 0) {
                    printf("\n%i -> wait: %i",tick, ptr->PID);
                    wait(ptr->PID);
                    breakout = 1;
                }
                ptr->runningC ++;



            }
        }
        if(waitingLen > 0){
            struct Process* ptr = waiting;
            for (int i=0; i<waitingLen; i++, ptr++ ) {
                if (ptr->waitingC > 0 && ptr->waitingC % ptr->IO_Duration == 0  && breakout == 0) {
                    printf("\n%i -> done: %i",tick, ptr->PID);
                    done(ptr->PID);
                    breakout = 1;
                }
                ptr->waitingC ++;

            }
        }
        tick++;
    }
    return 0;
}



void admitted(struct Process process) {
    // gone -> ready
    printf("\nadmitted: %i", process.PID);
    ready[readyLen] = process;
    readyLen ++;
}

void dispatch(int PID) {
    // ready -> running
    struct Process* ptr = ready;
    for (int i=0; i<readyLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            running[runningLen] = *ptr;
            runningLen ++;
            remove_element(ready, i, readyLen);
            readyLen --;
        }
    }
}

void wait(int PID){
    // running -> waiting
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            waiting[waitingLen] = *ptr;
            waitingLen ++;
            remove_element(running, i, runningLen);
            runningLen --;
        }
    }
}



void done(int PID){
    // waiting -> ready
    struct Process* ptr = waiting;
    for (int i=0; i<waitingLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            ready[readyLen] = *ptr;
            readyLen ++;
            remove_element(waiting, i, waitingLen);
            waitingLen --;
        }
    }
}

void interrupt(int PID){
    // running -> ready
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            ready[readyLen] = *ptr;
            readyLen ++;
            remove_element(running, i, runningLen);
            runningLen --;
        }
    }
}

void terminate(int PID){
    // running -> gone
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            remove_element(running, i, runningLen);
            runningLen --;
        }
    }
}

void remove_element(struct Process array[arrlen], int index, int array_length) {
    for(int i = index; i < array_length - 1; i++) array[i] = array[i + 1];
}

void readFile()
{
    FILE* file = fopen ("input.txt", "r");
    int i,i1,i2,i3,i4;
    while (!feof (file))
    {

        fscanf (file, "%d %d %d %d %d", &i, &i1, &i2, &i3, &i4);
        //printf ("\n%d %d %d %d %d\n", i, i1, i2, i3, i4);
        struct Process p1;
        p1.PID = i;
        p1.Arrival_Time = i1;
        p1.Total_CPU_Time = i2;
        p1.IO_Frequency = i3;
        p1.IO_Duration = i4;
        p1.runningC = 0;
        p1.waitingC = 0;
        admitted(p1);

    }
    fclose (file);

}




