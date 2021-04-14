//
// Created by Philip Baird and Dimitry Koutchine on 2021-04-13.
//
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

    // create shared message
    int msqid = msgget((key_t)1111, IPC_CREAT| 0600);
    if (msqid == -1){
        perror("msgget: msgget failed");
        exit(1);
    } else {
//        printf("Message ID: %i\n", msqid);
    }


    while(1){
        int accountNumber, PIN;
        float funds;
        printf("Account Number: ");
        scanf("%d", &accountNumber);
        printf("PIN: : ");
        scanf("%d", &PIN);
        printf("Funds: ");
        scanf("%f", &funds);
        printf("AccountNumber: %d PIN: %d Funds: %f\n",accountNumber,PIN,funds);
        struct my_message _msg;
        int msgLength = sizeof(struct my_message) - sizeof(long);
        _msg.message_type = 4;
        _msg.accountNumber = accountNumber;
        _msg.PIN = PIN;
        _msg.funds = funds;
        SemaphoreWait(s_mutex, -1);
        if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
            perror("msgsnd: msgsnd failed");
            exit(1);
        } else {
            printf("dbEditor: Message Sent: %i\n", _msg.accountNumber);
        }
        SemaphoreSignal(s_mutex);
    }

    return 0;

}
