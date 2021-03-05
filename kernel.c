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
#define input "input.txt"
#define output "output.txt"

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

void clearOutput();

void printFile(int t, int PID, char *s1, char *s2);


int readyLen = 0, runningLen = 0, waitingLen = 0, tick = 0;
//int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration;
//int runningC, waitingC;
int main(int argc, char *argv[]) {
    clearOutput();
    readFile();
    // to make sure max only one transition per tick
    int breakout = 0;
    // loops for each tick
    while(tick < ticks){
        breakout = 0;
        // if there is process in the running array, should never be more than one
        if(runningLen > 0){
            struct Process* ptr = running;
            for (int i=0; i<runningLen; i++, ptr++ ) {
                // if the running count for the process greater than the total cpu time and no other action has happened
                if (ptr->runningC >= ptr->Total_CPU_Time  && breakout == 0){
                    printf("%i -> terminate: %i\n",tick, ptr->PID);
                    printFile(tick, ptr->PID, "RUNNING", "TERMINATED");
                    terminate(ptr->PID);
                    breakout = 1;
                }
                // if the process didnt just start and it is evenly divisible by the io frequency and no other action has happnened
                else if (ptr->runningC > 0 && ptr->runningC % ptr->IO_Frequency == 0  && breakout == 0) {
                    printf("%i -> wait: %i\n",tick, ptr->PID);
                    printFile(tick, ptr->PID, "RUNNING", "WAITING");
                    // the processes is moved to waiting
                    wait(ptr->PID);
                    breakout = 1;
                }
                ptr->runningC ++;



            }
        }
        // if there are processes in the ready array
        if(readyLen > 0){
            // go through all the processes
            struct Process* ptr = ready;
            for (int i=0; i<readyLen; i++, ptr++ ) {
                // if the running array is empty and the tick is after the arrival time and there has not been a action preformed yet
                if (runningLen == 0 && tick >= ptr->Arrival_Time  && breakout == 0) {
                    printf("%i -> dispatch: %i\n",tick, ptr->PID);
                    printFile(tick, ptr->PID, "READY", "RUNNING");
                    // move the process to running
                    dispatch(ptr->PID);
                    breakout = 1;

                }
            }
        }

        // if there is processes in the waiting array
        if(waitingLen > 0){
            struct Process* ptr = waiting;
            for (int i=0; i<waitingLen; i++, ptr++ ) {
                // if the process didnt just start and is the time waiting for io is divisible by the io duration
                if (ptr->waitingC > 0 && ptr->waitingC % ptr->IO_Duration == 0  && breakout == 0) {
                    printf("%i -> done: %i\n",tick, ptr->PID);
                    printFile(tick, ptr->PID, "WAITING", "READY");
                    // move to ready
                    done(ptr->PID);
                    breakout = 1;
                }
                // each process in waiting increases
                ptr->waitingC ++;

            }
        }
        tick++;
    }
    return 0;
}



void admitted(struct Process process) {
    // gone -> ready
    printf("admitted: %i\n", process.PID);
    // adds process to the end of the ready array
    ready[readyLen] = process;
    readyLen ++;
}

void dispatch(int PID) {
    // ready -> running
    // goes through each process in the ready array
    struct Process* ptr = ready;
    for (int i=0; i<readyLen; i++, ptr++ ) {
        // finds the matching pid
        if(ptr->PID == PID){
            // adds process to the running array
            running[runningLen] = *ptr;
            // increments running len
            runningLen ++;
            // removes the process from the ready array
            remove_element(ready, i, readyLen);
            // decrements the ready len
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
/**
 * removes the element at selected index from the array
 */
void remove_element(struct Process array[arrlen], int index, int array_length) {
    for(int i = index; i < array_length - 1; i++) array[i] = array[i + 1];
}

/**
 * reads in file from the input filename
 */
void readFile()
{
    FILE* file = fopen (input, "r");
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

/**
 * prints one line to the output file
 * @param t
 * @param PID
 * @param s1
 * @param s2
 */
void printFile(int t, int PID, char *s1, char *s2)
{
    FILE* file = fopen (output, "a");
    fprintf (file, "%d %d %s %s\n", t, PID, s1, s2);
    fclose (file);
}

/**
 * clears the output file
 */
void clearOutput()
{
    FILE* file = fopen (output, "w");
    fclose (file);
}







