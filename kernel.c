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
#define quantum 3
#define input "input.txt"
#define output "output.txt"

struct Process {
    int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration, runningC, waitingC, priority;
};
struct ProcessData{
    int startWait, totalWait, arrivalTime, executedTime;
};

struct Process ready[arrlen], running[arrlen], waiting[arrlen];
struct ProcessData data[arrlen];


void remove_element(struct Process array[arrlen], int index, int array_length);
void printStatsFile(char type);
void admitted(struct Process process);

void terminate(int PID);
void clearStats();
void wait(int PID);

void dispatch(int PID);

void done(int PID);

void interrupt(int PID);

void readFile();

void clearOutput();

void printFile(int t, int PID, char *s1, char *s2);

void fcfs();

void ep();

void rr();

// note add to ready -> run => ptr->runningC ++;
// node add to run  -> wait => && ptr->IO_Frequency != 0 &&

int readyLen = 0, runningLen = 0, waitingLen = 0, tick = 0;
//int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration;
//int runningC, waitingC;
int main() {

    char strScheduler[20];
    int scheduler = 0;
    while(scheduler == 0){
        printf("Scheduler to use: fcfs, ep, rr ?\n");
        fgets(strScheduler,20,stdin);
        if (strstr(strScheduler, "fcfs") != NULL) {
            scheduler = 1;
        }
        else if (strstr(strScheduler, "ep") != NULL) {
            scheduler = 2;
        }
        else if (strstr(strScheduler, "rr") != NULL) {
            scheduler = 3;
        }
        else {
            printf("Wrong choice ?\n");
        }
    }

    clearOutput();
    switch(scheduler){
        case 1:
//            for (int i=0; i<runningLen; i++, ptr++ ) {
            readFile();
            fcfs();
            break;
        case 2:
            readFile();
            ep();
            break;
        case 3:
            readFile();
            rr();
            break;
    }


    return 0;
}

void fcfs(){
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
                else if (ptr->runningC > 0 && ptr->IO_Frequency != 0 && ptr->runningC % ptr->IO_Frequency == 0  && breakout == 0) {
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
                    // first tick it is in running for it counts
                    ptr->runningC ++;
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
}

void ep(){
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
            int priorities = arrlen;

            for (int i=0; i<readyLen; i++, ptr++ ) {
                if (ptr->priority < priorities) {
                    priorities = ptr->priority;

                }
            }
            for (int i=0; i<readyLen; i++, ptr++ ) {
                // if the running array is empty and the tick is after the arrival time and there has not been a action preformed yet
                if (runningLen == 0 && tick >= ptr->Arrival_Time  && breakout == 0 && ptr-> priority == priorities) {
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
}

void rr(){
    // to make sure max only one transition per tick
    int breakout = 0;
    int run = 0;
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
                    run = 0;
                }
                // if the process has been for longer than the quantum consecutively
                else if (run >= quantum && breakout == 0 ){
                    printf("%i -> kicked: %i ------> %i \n",tick, ptr->PID, ptr->runningC+1);
                    printFile(tick, ptr->PID, "RUNNING", "READY");
                    // the processes is moved to waiting
                    interrupt(ptr->PID);
                    breakout = 1;
                    run = 0;
                }
                    // if the process didnt just start and it is evenly divisible by the io frequency and no other action has happened
                else if (ptr->runningC > 0 && ptr->IO_Frequency != 0 && ptr->runningC % ptr->IO_Frequency == 0  && breakout == 0) {
                    printf("%i -> wait: %i\n",tick, ptr->PID);
                    printFile(tick, ptr->PID, "RUNNING", "WAITING");
                    // the processes is moved to waiting
                    wait(ptr->PID);
                    breakout = 1;
                    run = 0;
                }
                ptr->runningC ++;
                run ++;



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
                    // the tick it is put into the running queue counts as run time.
                    ptr->runningC ++;

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
}




void admitted(struct Process process) {
    // arrival -> ready
    printf("admitted: %i\n", process.PID);
    data[process.PID-1].arrivalTime = tick;
    // adds process to the end of the ready array
    ready[readyLen] = process;
    readyLen ++;
}

void dispatch(int PID) {
    // ready -> running
    // goes through each process in the ready array
    data[PID-1].totalWait += tick-data[PID].startWait;
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
    data[PID-1].startWait =tick;
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
    // running -> Done
    data[PID-1].executedTime = tick;
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            remove_element(running, i, runningLen);
            runningLen --;
        }
    }
    if(readyLen == 0 && waitingLen == 0){
        printStatsFile();
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
    FILE* file = fopen (input , "r");
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
    FILE* file = fopen (output+'_', "a");
    fprintf (file, "%d %d %s %s\n", t, PID, s1, s2);
    fclose (file);
}
void printStatsFile(char type){
    clearStats();
    int count = 0;
    FILE* file = fopen ("Stats.txt", "a");
    printf("Currently using %s algorithm\n", type);
    for(int i = 0; i < (sizeof(data)/sizeof(data[0])); i++ ){
        if(data[i].executedTime != 0) {
            fprintf(file, "%d. turnaround: %d,  time waited: %d  \n", i, (data[i].executedTime - data[i].arrivalTime),data[i].totalWait);
            count++;

        }

    }
    fprintf(file,"Average throughput was %d", tick/count);
    fclose(file);


}
void clearStats()
{
FILE* file = fopen ("Stats.txt", "w");
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







