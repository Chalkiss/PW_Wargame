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
#include <ncurses.h>

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

    initscr();
    mvprintw(4,5,"Resources: ");
    mvprintw(7,5,"Workers: ");
    mvprintw(10,5,"Units: ");
    mvprintw(12,5,"Infantry: ");
    mvprintw(15,5,"Heavy Infantry: ");
    mvprintw(18,5,"Cavalry: ");
    mvprintw(4,55,"Who to attack?");
    mvprintw(13,55,"Victories: ");
    




    refresh();   
    while(1){
        msg_received = receive_message(queue_id, rec, client_id+1);
        if(msg_received != -1){
            resources = rec->resources_amount;
                mvprintw(5, 8,"%d", resources);
                refresh();
            }
    }
    return 0;
}