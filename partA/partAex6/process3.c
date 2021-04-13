//
// Created by Philip Baird on 2021-04-04.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "sema.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>



int main(int ac, char *av[]) {
    printf("Process 3 %u\n", getpid());
    int *shared_int, shm_id, s_mutex, msqid;
    int out = 1;
    shm_id = atoi(av[1]);
    s_mutex = atoi(av[2]);
    msqid = atoi(av[3]);

//    printf("Message ID: %i\n", msqid);
    struct my_message msg;
    int msgLength = sizeof(struct my_message) - sizeof(long);

    SemaphoreWait(s_mutex, -1);
    if(msgrcv(msqid, &msg, msgLength, 1, 0) == -1){
        perror("msgrcv: msgrcv failed");
        exit(1);
    } else {
        printf("Child 2: Message Received: %i\n", msg.data + 1);
    }
    SemaphoreSignal(s_mutex);
    printf("Child 2: Done\n");
    exit(0);
}



