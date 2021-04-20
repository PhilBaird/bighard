//
// Created by Philip Baird on 2021-04-12.
//
#include <sys/ipc.h>
#include <sys/sem.h>
#include <printf.h>

int SemaphoreWait(int semid, int iMayBlock ) {
//    printf("Wait\n");
    struct sembuf sbOperation;
    sbOperation.sem_num = 0;
    sbOperation.sem_op = -1;
    sbOperation.sem_flg = iMayBlock;
    return semop( semid, &sbOperation, 1 );
}

int SemaphoreSignal( int semid ) {
//    printf("Signal\n");
    struct sembuf sbOperation;
    sbOperation.sem_num = 0;
    sbOperation.sem_op = +1;
    sbOperation.sem_flg = 0;
    return semop( semid, &sbOperation, 1 );
}

void SemaphoreRemove( int semid ) {
//    printf("Remove\n");
    if(semid != -1 )
        semctl( semid, 0, IPC_RMID , 0);
}

int SemaphoreCreate(int iInitialValue) {
//    printf("Create\n");
    int semid;
    union semun suInitData;
    int iError;
/* get a semaphore */
    semid = semget( IPC_PRIVATE, 1, IPC_CREAT | 0660 );
/* check for errors */
    if( semid == -1 )
        return semid;
/* now initialize the semaphore */
    suInitData.val = iInitialValue;
    if(semctl( semid, 0, SETVAL, suInitData) == -1)
    { /* error occurred, so remove semaphore */
        SemaphoreRemove(semid);
        return -1;
    }
    return semid;
}