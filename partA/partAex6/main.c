#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "sema.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main() {
    int *shared_int, shm_id, s_mutex;;

    if (( shm_id = (shmget (IPC_PRIVATE, 1, IPC_CREAT | 0660 ))) == (int)-1)
        perror("shmget error");
    if (( shared_int = (int * ) shmat (shm_id, (char *)0, 0 )) == (int *)-1)
        perror("shmat");

    if (( s_mutex = SemaphoreCreate(1)) == -1) {
        SemaphoreRemove(s_mutex);
        perror("Error in SemaphoreCreate");
        exit(1);
    }



    int msqid = msgget((key_t)1234, IPC_CREAT| 0600);
    if (msqid == -1){
        perror("msgget: msgget failed");
        exit(1);
    } else {
//        printf("Message ID: %i\n", msqid);
    }

    int child = fork();
    if(child == 0) {
        // child
        char str0[20], str1[20], str2[20];
        sprintf(str0, "%d", shm_id);
        sprintf(str1, "%d", s_mutex);
        sprintf(str2, "%d", msqid);
        char *args[] = {"./p2", str0, str1, str2, NULL};
        execv(args[0], args);
    }else{
        // parent
        SemaphoreWait(s_mutex, -1);
        struct my_message msg;
        msg.message_type = 1;
        msg.data = 100;
        int msgLength = sizeof(struct my_message) - sizeof(long);
        if(msgsnd(msqid, &msg, msgLength, 0) == -1){
            perror("msgsnd: msgsnd failed");
            exit(1);
        } else {
            printf("Parent: Message Sent: %i\n", msg.data);
        }
        SemaphoreSignal(s_mutex);
        wait(NULL);
        printf("Parent: Done\n");
    }

    return 0;
}


