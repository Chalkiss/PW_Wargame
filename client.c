#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm-wrappers.h"


int main(int argc, char* argv[]){
    int *player_connected;
int client_id = atoi(argv[1]);
int shared_memory_id = create_shmem_init(client_id);
player_connected = att_shmem(shared_memory_id);
*player_connected = 1;
printf("Polaczono z serwerem\n");

}