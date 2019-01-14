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

int *player_connected;
int *endgame;
int *atk_shm;
void cleanup(){
    detach_shmem(player_connected);
    detach_shmem(atk_shm);
    detach_shmem(endgame);
    printf("Odlaczono od pamieci wspoldzielonych\n");
    clear();
    echo();
    refresh();
    endwin();
    exit(0);
}

int main(int argc, char* argv[]){
    
    int client_id = atoi(argv[1]);

    int shared_memory_id = create_shmem_init(client_id);
    int attack_memory_id = create_shmem_atk(client_id);

    int queue_id = create_queue_init(client_id);
    int msg_received,msg_received_end, msg_received_marked;

    char unit_type = 'z';
    char other_button = '/';

    player buf;
    player *rec = &buf;



    atk_shm = att_shmem(attack_memory_id);
    player_connected = att_shmem(shared_memory_id);
    int server_on =-4;

    server_ready buffer;
    server_ready *srv = &buffer;

    while(server_on < 0){
    server_on = receive_message_int(queue_id, srv, 100  + client_id);
    *player_connected = 1;
    signal(SIGINT, cleanup);
    }

    attack_data bfr;
    attack_data *atk = &bfr;
    atk ->attacker_id = -3;
    atk ->defender_id = -3;
    atk ->unit[0] = 0;
    atk ->unit[1] = 0;
    atk ->unit[2] = 0;

    server_ready bufr;
    server_ready *end = &bufr;

    button_data trbuf;
    button_data *snd = &trbuf;

    char atk1, atk2;

    attack_data bum;
    attack_data *atk_back = &bum;

    int end_shm_id;
    end_shm_id = create_shmem_end();
    endgame = att_shmem(end_shm_id);
    *endgame = 0;

        switch(client_id){
    case 0:
        atk1 = '1';
        atk2 = '2';
        break;
    case 1:
        atk1 = '0';
        atk2 = '2';
        break;
    case 2:
        atk1 = '0';
        atk2 = '1';
        break;
    }
    initscr();
    mvprintw(4,5,"Resources: ");
    mvprintw(9,5,"Workers: (Q) ");
    mvprintw(7,5,"Units: (press button to train)");
    mvprintw(12,5,"Infantry: (W)");
    mvprintw(15,5,"Heavy Infantry: (E)");
    mvprintw(18,5,"Cavalry: (R)");
    mvprintw(4,55,"Whom to attack?");
    mvprintw(13,55,"Victories: ");
    mvprintw(5, 57,"%c",atk1);
    mvprintw(5, 60,"%c",atk2);
    mvprintw(8,30,"Choose units to attack. By pressing given button ");
    mvprintw(9,30,"once you mark single unit of a given type to fight");
    mvprintw(10,30, "V- Infantry, B- Heavy Infantry, N- Cavalry");
    mvprintw(11,30,"Send units to fight by pressing S");
    mvprintw(13,30, "Units marked to fight:");
    mvprintw(14,30, "Infantry:");
    mvprintw(15,30, "Heavy Infantry:");
    mvprintw(16,30, "Cavalry:");
    timeout(0);
    noecho();




    refresh();   
    while(*endgame != 1){
        signal(SIGINT, cleanup);
        msg_received = receive_message(queue_id, rec, client_id+1);
        msg_received_end = receive_message_int(queue_id, end,200+client_id);
        msg_received_marked = receive_message_attack(queue_id,atk_back,client_id+16);
        if(msg_received != -1){
                mvprintw(5, 5,"%d      ", rec->resources_amount);
                mvprintw(10, 5,"%d     ", rec->workers);
                mvprintw(13, 5,"%d     ", rec->unit[0]);
                mvprintw(16, 5,"%d     ", rec->unit[1]);
                mvprintw(19, 5,"%d     ", rec->unit[2]);
                mvprintw(14, 55,"%d", rec->victories);
                refresh();
            }
        else if(msg_received_end != -1){
           cleanup();
        }else if(msg_received_marked != -1){
            mvprintw(14,41,"%d       ",atk_back->unit[0]);
            mvprintw(15,47,"%d       ",atk_back->unit[1]);
            mvprintw(16,40,"%d       ",atk_back->unit[2]);
        }
        unit_type = getch();
        other_button = unit_type;                    
        if(unit_type == 'q' || unit_type ==  'w' || unit_type == 'e' || unit_type == 'r'){
                snd->unit_type = unit_type;
            send_message_button(queue_id, snd, client_id+4);
            refresh();
            }
        else if(other_button == atk1 || other_button == atk2){
            atk->attacker_id = client_id;
            atk->defender_id = atoi(&other_button);
            send_message_attack(queue_id, atk, client_id+10);
        }
        else if(other_button == 'v'|| other_button == 'b'|| other_button == 'n'){
            snd->unit_type = other_button;
            send_message_button(queue_id, snd, client_id+13);
        }
        else if (other_button == 's'){
            snd->unit_type = other_button;
            send_message_button(queue_id, snd, client_id+19);
        }        
    }
    cleanup();
    exit(0);
    return 0;
}

