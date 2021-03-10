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
#define input "input_0.txt"
#define output "output.txt"

struct Process {
    int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration, runningC, waitingC, priority, size, partition;
};
struct ProcessData{
    int startWait, totalWait, arrivalTime, executedTime;
};
struct Partition{
    int size, PID, used;
};

struct Process ready[arrlen], running[arrlen], waiting[arrlen];
struct ProcessData data[arrlen];
struct Partition storage[4];


void remove_element(struct Process array[arrlen], int index, int array_length);
void printStatsFile(int type);
void admitted(struct Process process);

void terminate(int PID, int type);
void clearStats();
void wait(int PID);

void dispatch(int PID);

void done(int PID);

void interrupt(int PID);

void readFile(int type, int test);

void clearOutput();

void printFile(int t, int PID, char *s1, char *s2);

void fcfs();
void ep();
void rr();

void initMem();
int usedPart();
int freePart();
int freeMem();
int usableMem();
int usedMem();
void removeMem(struct Process process);
int addMem(struct Process process);

int memory(int size, int PID);


int readyLen = 0, runningLen = 0, waitingLen = 0, tick = 0;
int storageLen = 0;
//int PID, Arrival_Time, Total_CPU_Time, IO_Frequency, IO_Duration;
//int runningC, waitingC;
int main() {
    clearStats();


    clearOutput();
    initMem();
    readFile(1, 0);
    fcfs();


    return 0;
}
/**
 *  When going from RUNNING -> TERMINATED, RUNNING -> WAITING, the process pid is removed from the mem
 *  When going from READY -> RUNNING this will only happen if addmem returns a positive value that corresponds to a partition
 *  this doesnt work because pointers have hurt my feelings. please help my decrepit soul and let us use java.
 */
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
                    removeMem(*ptr);
                    terminate(ptr->PID, 1);
                    breakout = 1;
                }
                    // if the process didnt just start and it is evenly divisible by the io frequency and no other action has happnened
                else if (ptr->runningC > 0 && ptr->IO_Frequency != 0 && ptr->runningC % ptr->IO_Frequency == 0  && breakout == 0) {
                    printf("%i -> wait: %i\n",tick, ptr->PID);
                    printFile(tick, ptr->PID, "RUNNING", "WAITING");
                    removeMem(*ptr);
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
                if(ptr->Arrival_Time == tick){
                    data[ptr->PID].startWait = tick;
                    printf("%i -> admitted: %i\n",tick, ptr->PID);
                }
                // if the running array is empty and the tick is after the arrival time and there has not been a action preformed yet
                if (runningLen == 0 && tick >= ptr->Arrival_Time  && breakout == 0 ) {
                    int index = addMem(*ptr);
                    if (index != -1){
                        ptr->partition = index;
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
    printf("loaded: %i\n", process.PID);
    data[process.PID].arrivalTime = process.Arrival_Time;
    data[process.PID].totalWait = 0;
    // adds process to the end of the ready array
    ready[readyLen] = process;
    readyLen ++;
}

void dispatch(int PID) {
    // ready -> running
    // goes through each process in the ready array
    data[PID].totalWait += tick-data[PID].startWait;
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
    data[PID].startWait = tick;
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
    data[PID].startWait = tick;
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

void terminate(int PID, int type){
    // running -> Done
    data[PID].executedTime = tick;
    struct Process* ptr = running;
    for (int i=0; i<runningLen; i++, ptr++ ) {
        if(ptr->PID == PID){
            remove_element(running, i, runningLen);
            runningLen --;
        }
    }
    if(readyLen == 0 && waitingLen == 0){
        printStatsFile(type);
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
void readFile(int type, int run)
{
    char fileName [20] = "";
    switch (type){
        case 1:
            sprintf(fileName, "data/rr/input_%i.txt",  run);
            break;
        case 2:
            sprintf(fileName, "data/rr/input_%i.txt",run);
            break;
        case 3:
            sprintf(fileName, "data/rr/input_%i.txt", run);
            break;
    }
    FILE* file = fopen (fileName , "r");
    int i,i1,i2,i3,i4,i5,i6 = 0;
    while (!feof (file))
    {
        fscanf (file, "%d %d %d %d %d %d %d", &i, &i1, &i2, &i3, &i4, &i5, &i6);

//        printf ("\n%d %d %d %d %d\n", i, i1, i2, i3, i4);
        struct Process p1;
        p1.PID = i;
        p1.Arrival_Time = i1;
        p1.Total_CPU_Time = i2;
        p1.IO_Frequency = i3;
        p1.IO_Duration = i4;
        p1.runningC = 0;
        p1.waitingC = 0;
        p1.priority = i5;
        p1.size = i6;

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
void printStatsFile(int type){
    char algorithm[20] = "";
    switch(type){
        case 1:
            sprintf(algorithm, "FCFS");
//            algorithm = "FCFS";
            break;
        case 3:
            sprintf(algorithm, "RR");
//            algorithm = "RR";
            break;
        case 2: sprintf(algorithm, "EP");
//            algorithm = "EP";
            break;
        default: sprintf(algorithm, "Yikes");
//            algorithm = "Yikes";
            break;

    }

    int count = 0;
    FILE* file = fopen ("Stats.txt", "a");
    fprintf(file,"Currently using %s algorithm\n", algorithm);
    for(int i = 0; i < (sizeof(data)/sizeof(data[0])); i++ ){
        if(data[i].executedTime != 0) {
            fprintf(file, "%d. turnaround: %d,  time waited: %d  \n", i, (data[i].executedTime - data[i].arrivalTime), data[i].totalWait - data[i].arrivalTime);
            count++;
        }

    }
    fprintf(file,"Average throughput was %d\n", tick/count);
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


/**
 * adds the process into the first free memory partition
 *
 * note: can't figure out how to actually change the storage values.
 *
 */
int addMem(struct Process process){
    printf("%i, %i, %i, %i, %i \n", usedMem(), freePart(), freeMem(), usableMem(), usedMem()  );
    struct Partition* ptr = storage;
    printf("%i\n", storageLen);
    for (int i=0; i<storageLen; i++ ) {
        if(ptr->used == 0 && ptr->size >= process.size){

            ptr->used = process.size;
            ptr->PID = process.PID;
            return i;
        }
    }
    return -1;
}

/**
 *  removes the process from the partition associated with the pid
 * @param process
 */
void removeMem(struct Process process){
    struct Partition* ptr = storage;
    for (int i=0; i<storageLen; i++, ptr++ ) {
        if(ptr->PID == process.PID){
            ptr->used = 0;
            ptr->PID = 0;
        }
    }
}
int usedMem(){
    int used = 0;
    struct Partition* ptr = storage;
    for (int i=0; i<storageLen; i++, ptr++ ) {
        used += ptr->used;
    }
    return used;
}
int usableMem(){
    int usable = 0;
    struct Partition* ptr = storage;
    for (int i=0; i<storageLen; i++, ptr++ ) {
        if (ptr->used == 0) usable += ptr->size;
    }
    return usable;
}
int freeMem(){
    int free = 0;
    struct Partition* ptr = storage;
    for (int i=0; i<storageLen; i++, ptr++ ) {
        free += ptr->size - ptr->used;
    }
    return free;
}
int freePart(){
    int free = 0;
    struct Partition* ptr = storage;
    for (int i=0; i<storageLen; i++, ptr++ ) {
        if (ptr->used == 0) free++;
    }
    return free;
}
int usedPart(){
    int used = 0;
    struct Partition* ptr = storage;
    for (int i=0; i<storageLen; i++, ptr++ ) {
        if (ptr->used != 0) used++;
    }
    return used;
}
void initMem(){
    struct Partition p1, p2, p3, p4;
    p1.PID = 0;
    p1.size = 500;
    p1.used = 0;
    p2.PID = 0;
    p2.size = 250;
    p2.used = 0;
    p3.PID = 0;
    p3.size = 150;
    p3.used = 0;
    p4.PID = 0;
    p4.size = 100;
    p4.used = 0;
    storage[storageLen] = p1;
    storageLen ++;
    storage[storageLen] = p2;
    storageLen ++;
    storage[storageLen] = p3;
    storageLen ++;
    storage[storageLen] = p4;
    storageLen ++;
}







