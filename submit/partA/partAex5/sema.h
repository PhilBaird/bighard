//
// Created by Philip Baird on 2021-04-12.
//

#ifndef UNTITLED_SEMA_H
#define UNTITLED_SEMA_H

int SemaphoreCreate(int iInitialValue);
void SemaphoreRemove( int semid );
int SemaphoreWait(int i, int iMayBlock );
int SemaphoreSignal( int semid );

#endif //UNTITLED_SEMA_H

