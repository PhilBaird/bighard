#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "sema.h"

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

    int child = fork();
    if(child == 0) {
        // child
        char str0[20], str1[20], str2[20];
        sprintf(str0, "%d", shm_id);
        sprintf(str1, "%d", s_mutex);
        char *args[] = {"./p2", str0, str1, NULL};
        execv(args[0], args);
    }else{
        // parent
        SemaphoreWait(s_mutex, -1);
        *shared_int = 5;
        SemaphoreSignal(s_mutex);
        int tempint = *shared_int;
        while( *shared_int != 0){
            if(*shared_int != tempint){
                printf("Parent: The value of the shared int is %d, temp %d\n", *shared_int, tempint);
                tempint = *shared_int;
            }
        }
        wait(NULL);
        printf("Parent: Done\n");
        printf("Parent: The value of the new shared int is %d\n", *shared_int);

        /** part 2 - 3 */
//        while(1) {
//            printf("Process 1 %u\n", getpid());
//            wait(NULL);
//            printf("Process 1 done \n");
//            exit(0);
//            int in = 0;
//            while (in != 1) {
//                scanf("%d", &in);
//            }
//        }
    }

        /** part 1 */
//    if(child == 0){
//            printf("Process 1 %u\n", getpid());
//            int child = getpid();
//            int parent = getppid();
//
//            kill(parent, SIGKILL);
//            kill(child, SIGKILL);
//
//    }
//    else{
//        while(1){
//            printf("Process 2 %u\n", getpid());
//        }
//    }

    return 0;
}


