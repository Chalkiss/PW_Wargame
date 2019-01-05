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

    char unit_type = '0';

    player buf;
    player *rec = &buf;

    player_connected = att_shmem(shared_memory_id);
    *player_connected = 1;

    train_data trbuf;
    train_data *snd = &trbuf;

    initscr();
    mvprintw(4,5,"Resources: ");
    mvprintw(9,5,"Workers: ");
    mvprintw(7,5,"Units: ");
    mvprintw(12,5,"Infantry: ");
    mvprintw(15,5,"Heavy Infantry: ");
    mvprintw(18,5,"Cavalry: ");
    mvprintw(4,55,"Who to attack?");
    mvprintw(13,55,"Victories: ");
    timeout(0);
    noecho();




    refresh();   
    while(1){
        msg_received = receive_message(queue_id, rec, client_id+1);
        if(msg_received != -1){
                mvprintw(5, 8,"%d", rec->resources_amount);
                mvprintw(10, 8,"%d", rec->workers);
                mvprintw(13, 9,"%d", rec->unit[0]);
                mvprintw(16, 11,"%d", rec->unit[1]);
                mvprintw(19, 8,"%d", rec->unit[2]);
                mvprintw(14, 59,"%d", rec->victories);
                refresh();
            }
            unit_type = getch();
            if(unit_type == 'w' || unit_type ==  'i' || unit_type == 'h' || unit_type == 'c'){
                snd->unit_type = unit_type;
            send_message_train(queue_id, snd, client_id+4);
            refresh();
            }
    }
    return 0;
}

