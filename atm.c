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

    // send   type 1 for PIN, 2 for BALANCE, 3 for WITHDRAW, 4 for UPDATEDB
    // return type 5 for PIN, 6 for BALANCE, 7 for WITHDRAW,
    while(1){
        int accountNumber, PIN;
        float funds;
        int breakout = 0;
        while(breakout < 50){
            printf("Account Number: ");
            scanf("%d", &accountNumber);
            while(breakout < 3){
                printf("PIN: : ");
                scanf("%d", &PIN);
                // send PIN
                struct my_message _msg;
                int msgLength = sizeof(struct my_message) - sizeof(long);
                _msg.message_type = 1;
                _msg.accountNumber = accountNumber;
                _msg.PIN = PIN;
                _msg.funds = -1;
                if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
                    perror("msgsnd: msgsnd failed");
                    exit(1);
                } else {
                    printf("PIN Message Sent: %i\n", _msg.accountNumber);
                }
                // receive PIN
                struct my_message rmsg;
                if(msgrcv(msqid, &rmsg, msgLength, 5, 0) == -1){
                    perror("msgrcv: msgrcv failed");
                    exit(1);
                } else {
                    printf("\nPIN Message Received: %i\n", rmsg.data);
                    if(rmsg.data == 1) breakout = 50;
                }
                breakout++;
            }
            if(breakout != 51){
                printf("PIN WRONG");
                breakout = 0;
            }
        }

        int choice = -1;
        while(choice != 0){
            printf("Banking Option, 1 for BALANCE, 2 for WITHDRAW, 0 for EXIT: ");
            scanf("%i", &choice);
            if(choice == 1){
                // send BALANCE
                struct my_message _msg;
                int msgLength = sizeof(struct my_message) - sizeof(long);
                _msg.message_type = 2;
                _msg.accountNumber = accountNumber;
                _msg.PIN = PIN;
                _msg.funds = -1;
                if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
                    perror("msgsnd: msgsnd failed");
                    exit(1);
                } else {
                    printf("BALANCE: Message Sent: %i\n", _msg.accountNumber);
                }
                // receive BALANCE
                struct my_message rmsg;
                if(msgrcv(msqid, &rmsg, msgLength, 6, 0) == -1){
                    perror("msgrcv: msgrcv failed");
                    exit(1);
                } else {
                    printf("\nBALANCE: Message Received: %i\n", rmsg.data);
                }
            }
            if(choice == 2){
                float withdraw = 0;
                printf("Withdraw: ");
                scanf("%f", &withdraw);
                // send WITHDRAW
                struct my_message _msg;
                int msgLength = sizeof(struct my_message) - sizeof(long);
                _msg.message_type = 3;
                _msg.accountNumber = accountNumber;
                _msg.PIN = PIN;
                _msg.funds = withdraw;
                if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
                    perror("msgsnd: msgsnd failed");
                    exit(1);
                } else {
                    printf("WITHDRAW: Message Sent: %i\n", _msg.accountNumber);
                }
                // receive WITHDRAW
                struct my_message rmsg;
                if(msgrcv(msqid, &rmsg, msgLength, 7, 0) == -1){
                    perror("msgrcv: msgrcv failed");
                    exit(1);
                } else {
                    printf("\nWITHDRAW: Message Received: %i\n", rmsg.data);
                }
            }
        }


//        printf("AccountNumber: %d PIN: %d Funds: %f\n",accountNumber,PIN,funds);

    }

    return 0;

}


