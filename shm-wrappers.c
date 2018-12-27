#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>

int create_shmem_init(int i){
    //Wrapper function for initial communication of server and clients
    int mem_id;
    if((mem_id = shmget(1984 + i,sizeof(int),IPC_CREAT|0640))== -1){
        perror("shm error - initial shmem creation");
    }
    return mem_id;
}