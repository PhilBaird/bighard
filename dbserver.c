//
// Created by Philip Baird and Dimitry Koutchine on 2021-04-13.
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
#include "dbserver.h"


void readFile();

int main(int ac, char *av[]) {
    int s_mutex;

    int msgLength = sizeof(struct my_message) - sizeof(long);

    // create shared mutex
    if (( s_mutex = SemaphoreCreate(1)) == -1) {
        SemaphoreRemove(s_mutex);
        perror("Error in SemaphoreCreate");
        exit(1);
    }

    int msqid = msgget((key_t)1111, IPC_CREAT| 0600);
    if (msqid == -1){
        perror("msgget: msgget failed");
        exit(1);
    } else {
//        printf("Message ID: %i\n", msqid);
    }

    // receive message
    struct my_message rmsg;
    if(msgrcv(msqid, &rmsg, msgLength, 4, 0) == -1){
        perror("msgrcv: msgrcv failed");
        exit(1);
    } else {
        printf("\ndbServer: Message Received: %i\n", rmsg.accountNumber);
    }

}


//void readFile()
//{
//    FILE* file = fopen ("db.txt", "r");
//    float i,i1,i2
//    while (!feof (file))
//    {
//        fscanf (file, "%d,%d,%d", &i, &i1, &i2);
//        printf ("\n%d %d %d\n", i, i1, i2);
//    }
//    fclose (file);
//}