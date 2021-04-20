//
// Created by Philip Baird on 2021-04-04.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include "sema.h"

// 0 is x in this code.
int main(int ac, char *av[]) {
    printf("Process 2 %u\n", getpid());
    int *shared_int, shm_id, s_mutex;
    int out = 3;
    shm_id = atoi(av[1]);
    s_mutex = atoi(av[2]);
    /** part 5 and less */
    if ( ( shared_int = (int * ) shmat (shm_id, (char *)0, 0 )) == (int *)-1)
        perror("shmat");
    printf("Child: The value of the shared int is %d\n", *shared_int);
    while(out != 0){
        out = 3;
        while(out != 2 && out != 1 && out != 0){
            scanf("%d", &out);
        }
        SemaphoreWait(s_mutex, -1);
        /** part 4 */
        *shared_int = out;
        SemaphoreSignal(s_mutex);
        printf("Child: %d was pressed\n", out);
    }
    exit(0);

}

