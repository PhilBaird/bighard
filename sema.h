//
// Created by Philip Baird and Dimitry Koutchine on 2021-04-12.
//

#ifndef UNTITLED_SEMA_H
#define UNTITLED_SEMA_H
struct my_message {
    // send   type 1 for PIN, 2 for BALANCE, 3 for WITHDRAW, 4 for UPDATEDB
    // return type 5 for PIN, 6 for BALANCE, 7 for WITHDRAW,
    long message_type;
    int data, accountNumber, PIN;
    float funds;
};
int SemaphoreCreate(int iInitialValue);
void SemaphoreRemove( int semid );
int SemaphoreWait(int i, int iMayBlock );
int SemaphoreSignal( int semid );

#endif //UNTITLED_SEMA_H

