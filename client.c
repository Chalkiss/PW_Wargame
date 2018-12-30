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
#include "sem-wrappers.h"
#include "queue-wrappers.h"


int main(int argc, char* argv[]){
    
    int *player_connected;
    int client_id = atoi(argv[1]);
    int shared_memory_id = create_shmem_init(client_id);
    int queue_id = create_queue_init(client_id);
    int msg_received;
    int resources;
    player buf;
    player *rec = &buf;
    player_connected = att_shmem(shared_memory_id);
    *player_connected = 1;
    printf("Polaczono z serwerem\n");
    
    while(1){
        msg_received = receive_message(queue_id, rec, client_id+1);
        if(msg_received != -1){
            resources = rec->resources_amount;
            printf("Ilosc surowcow: %d\n",resources);
            }
    }
}