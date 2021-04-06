//
// Created by Philip Baird on 2021-04-04.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>

int main() {
    int *shared_int, shm_id;
    int out = 1;
    // x = 0

    shm_id = (shmget(IPC_PRIVATE, 1, 0666 | IPC_CREAT));
    shared_int = (int *) shmat(shm_id, (char*)0,0);
    while(out != 0){
        out = 1;
        printf("Process 2 %u\n", getpid());
        while(out != 2 && out != 0){
            scanf("%d", &out);
        }

    }
    printf("Process 2 exits\n");
    exit(0);


    return 0;
}