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



int main(int argc, char* argv[]){
    int *player_connected;
int client_id = atoi(argv[1]);
int shared_memory_id = shmget(1984 + client_id, sizeof(int),IPC_CREAT|0640);
player_connected = shmat(shared_memory_id,0,0);
*player_connected = 1;
printf("Polaczono z serwerem\n");

}