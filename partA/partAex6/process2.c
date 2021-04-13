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
    printf("Process 2 %u\n", getpid());
    int *shared_int, shm_id, s_mutex, msqid;
    int out = 1;
    shm_id = atoi(av[1]);
    s_mutex = atoi(av[2]);
    msqid = atoi(av[3]);
//    printf("Message ID: %i\n", msqid);

    int child = fork();
    if(child == 0) {
        // child
        char str0[20], str1[20], str2[20];
        sprintf(str0, "%d", shm_id);
        sprintf(str1, "%d", s_mutex);
        sprintf(str2, "%d", msqid);
        char *args[] = {"./p3", str0, str1, str2, NULL};
        execv(args[0], args);
    }else{
        // parent
        struct my_message msg;
        int msgLength = sizeof(struct my_message) - sizeof(long);
        SemaphoreWait(s_mutex, -1);
        if(msgrcv(msqid, &msg, msgLength, 1, 0) == -1){
            perror("msgrcv: msgrcv failed");
            exit(1);
        } else {
            printf("Child 1: Message Received: %i\n", msg.data);
        }
        struct my_message _msg;
        _msg.message_type = 1;
        _msg.data = msg.data * 2;
        if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
            perror("msgsnd: msgsnd failed");
            exit(1);
        } else {
            printf("Child 1: Message Sent: %i\n", _msg.data);
        }
        SemaphoreSignal(s_mutex);
        wait(NULL);
        printf("Child 1: Done\n");

    }
    exit(0);

}

