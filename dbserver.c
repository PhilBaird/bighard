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
#include "stdbool.h"

float readBalance(int acntNumber);
int readPin(int pin, int acntNumber);
int withdraw(int acntNumber, float amnt);
void updateDB(int acntNumber, int pin, float funds);

int main(int ac, char *av[]) {
    int s_mutex;

    int msgLength = sizeof(struct my_message) - sizeof(long);

    // create shared mutex
    if (( s_mutex = SemaphoreCreate(1)) == -1) {
        SemaphoreRemove(s_mutex);
        perror("Error in SemaphoreCreate");
        exit(1);
    }

    // open message
    int msqid = msgget((key_t)1111, IPC_CREAT| 0600);
    if (msqid == -1){
        perror("msgget: msgget failed");
        exit(1);
    } else {
//        printf("Message ID: %i\n", msqid);
    }
    while (1){
        // send   type 1 for PIN, 2 for BALANCE, 3 for WITHDRAW, 4 for UPDATEDB
        // return type 5 for PIN, 6 for BALANCE, 7 for WITHDRAW,

        // create a thread for each message type.
        int child = fork();
        if(child == 0) {
            // child
            int gchild = fork();
            if(gchild == 0) {
                // child
                int ggchild = fork();
                if(ggchild == 0) {
                    // ggchild
                    // receive PIN
                    struct my_message rmsg;
                    if(msgrcv(msqid, &rmsg, msgLength, 1, 0) == -1){
                        perror("msgrcv: msgrcv failed");
                        exit(1);
                    } else {
                        printf("\nPIN Message Received: %i\n", rmsg.accountNumber);
                    }
                    struct my_message _msg;
                    _msg.message_type = 5;
                    // while the file is being accessed lock the semaphore
                    SemaphoreWait(s_mutex, -1);
                    // sends to atm or dbeditor the return from readpin in the data
                    _msg.data = readPin(rmsg.PIN, rmsg.accountNumber);
                    SemaphoreSignal(s_mutex);
                    if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
                        perror("msgsnd: msgsnd failed");
                        exit(1);
                    } else {
                        printf("PIN Message Sent: %i\n", _msg.data);
                    }
                }else{
                    // gchild
                    // receive BALANCE
                    struct my_message rmsg;
                    if(msgrcv(msqid, &rmsg, msgLength, 2, 0) == -1){
                        perror("msgrcv: msgrcv failed");
                        exit(1);
                    } else {
                        printf("\nBALANCE Message Received: %i\n", rmsg.accountNumber);
                    }
                    struct my_message _msg;
                    _msg.message_type = 6;
                    SemaphoreWait(s_mutex, -1);
                    _msg.data = readBalance(rmsg.accountNumber);
                    SemaphoreSignal(s_mutex);
                    if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
                        perror("msgsnd: msgsnd failed");
                        exit(1);
                    } else {
                        printf("BALANCE Message Sent: %i\n", _msg.data);
                    }
                }

            }else{
                // child
                // receive WITHDRAW
                struct my_message rmsg;
                if(msgrcv(msqid, &rmsg, msgLength, 3, 0) == -1){
                    perror("msgrcv: msgrcv failed");
                    exit(1);
                } else {
                    printf("\nWITHDRAW Message Received: %i\n", rmsg.accountNumber);
                }
                struct my_message _msg;
                _msg.message_type = 7;
                SemaphoreWait(s_mutex, -1);
                _msg.data = withdraw(rmsg.accountNumber, rmsg.funds);
                SemaphoreSignal(s_mutex);
                if(msgsnd(msqid, &_msg, msgLength, 0) == -1){
                    perror("msgsnd: msgsnd failed");
                    exit(1);
                } else {
                    printf("WITHDRAW Message Sent: %i\n", _msg.data);
                }
            }

        }else{
            // parent
            // receive UPDATEDB
            struct my_message rmsg;
            if(msgrcv(msqid, &rmsg, msgLength, 4, 0) == -1){
                perror("msgrcv: msgrcv failed");
                exit(1);
            } else {
                printf("\nUPDATEDB Message Received: %i\n", rmsg.accountNumber);
                SemaphoreWait(s_mutex, -1);
                updateDB(rmsg.accountNumber, rmsg.PIN, rmsg.funds);
                SemaphoreSignal(s_mutex);
            }
        }
    }


}
// Function checks if pin matches the pin in the database
int readPin(int pin, int acntNumber){

    FILE* file = fopen ("../db.txt", "r");
    int i,i1; //pin, acntNumber
    float f; //balance
    bool check = false;// boolean to see if acntNumber was found

    while (!feof (file)){
        fscanf (file, "%d,%d,%f", &i, &i1, &f);
        if(i == acntNumber){
            check = true; // if acnt number is found it breaks out of while loop
            break;
        }

    }
    fclose (file);

    if(check == true)
    {
        if (pin - 1 == i1) {
            return 1; //correct pin was entered
        } else {
            return 0; //wrong pin was entered
        }
    }
    else{
        return -1; // acnt number was not found
    }

}
// returns balance for the given acnt number from database
float readBalance(int acntNumber){

    FILE* file = fopen ("../db.txt", "r");
    int i,i1;
    float f;
    bool check = false;
    while (!feof (file)){
        fscanf (file, "%d,%d,%f", &i, &i1, &f);
        if(i == acntNumber){
            check = true;
            break;
        }

    }
    fclose (file);
    if (check) {
        return f; // returns balance from database
    }
    return -1; // if account number is not found

}
int withdraw(int acntNumber, float amnt){

    FILE* file = fopen ("../db.txt", "r");
    int i,i1,i3,i4; // i3 , i4, and f1 are for second while loop
    float f,f1;
    int pos = 0;// keeps track of which line the acnt is found on
    bool check = false;
    while (!feof (file)){
        fscanf (file, "%d,%d,%f", &i, &i1, &f);
        pos++;
        if(i == acntNumber){
            check = true;
            break;
        }

    }

    fclose (file);
    if (check) {
        if (f < amnt){
            return 0; // returns 0 if the amnt is larger than balance
        }
        else{
            float newAmnt = f - amnt;

            FILE* file = fopen ("../db.txt", "r");
            FILE* file2 = fopen ("../_db.txt", "w");
            while (!feof (file))
            {
                fscanf (file, "%d,%d,%f", &i3, &i4, &f1);
//        printf ("\n%f %f %f\n", i, i1, i2);
                if(i3 == acntNumber){
                    fprintf(file2,"%d,%d,%f\n", i3, i4, newAmnt);
                }else{
                    fprintf(file2,"%d,%d,%f\n", i3, i4, f1);
                }

            }

            fclose (file);
            fclose (file2);
            remove("../db.txt");
            rename("../_db.txt", "../db.txt");
            return 1;


        }
    }
    else
    return -1; // if account number is not found

}
//adds another entry in the database
void updateDB(int acntNumber, int pin, float funds){
    FILE* file = fopen ("../db.txt", "a");
    fprintf(file,"\n%d,%d,%f", acntNumber, pin-1, funds);
    fclose (file);
}
