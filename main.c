#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>


int main() {
    int *shared_int, shm_id;

    int child = fork();
//    if((shm_id = (shmget(IPC_PRIVATE, 1, SHM_MODE))))
//    if((shared_int = (int *) shmat(shm_id, (char*)0,0)) == (int *) -1){
//        *shared_int = 7;
//    }

//    shm_id = (shmget(IPC_PRIVATE, 1, 0666 | IPC_CREAT));
//    shared_int = (int *) shmat(shm_id, (char*)0,0);
//    shmdt(shared_int);
//    shmctl(shm_id,IPC_RMID, NULL);

    if(child == 0) {
        // child
        char *args[] = {"./p2", NULL};
        execv(args[0], args);
    }else{
        // parent
        while(1) {
            printf("Process 1 %u\n", getpid());
            wait(NULL);
            printf("Process 1 done \n");
            exit(0);
//            int in = 0;
//            while (in != 1) {
//                scanf("%d", &in);
//            }
        }
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
