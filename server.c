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

void update_resources (int client_queue_id, player *player, int semaphore){
    P_operation(semaphore);
    player->resources_amount = player->resources_amount + 50+ 5*(player->workers);
    send_message(client_queue_id, player, player->player_id+1);
    V_operation(semaphore);
}

void pay_units(char unit, player *player, int resources_sem){
    P_operation(resources_sem);
    switch(unit)
    {
        case 'q':
            if(player -> resources_amount >= 150){
                player -> resources_amount -= 150;
                V_operation(resources_sem);
            }else{
                unit = ']'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }
            break;

        case 'w':
            if(player -> resources_amount >= 100){
                player -> resources_amount -= 100;
                V_operation(resources_sem);
            }else{
                unit = ']'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }
            break;
            
        case 'e':
            if(player -> resources_amount >= 350){
                player -> resources_amount -= 350;
                V_operation(resources_sem);
            }else{
                unit = ']'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }            break;

        case 'r':
            if(player -> resources_amount >= 550){
                player -> resources_amount -= 550;
                V_operation(resources_sem);                
            }else{
                unit = ']'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }
            break;

    }

}

void train_unit(char unit, player *player, int unit_sem){
    switch(unit)
    {

        case 'q':
            sleep(2);
            P_operation(unit_sem);
            player->workers++;
            V_operation(unit_sem);
            break;

        case 'w':
            sleep(2);
            P_operation(unit_sem);
            player->unit[0]++;
            V_operation(unit_sem);            
            break;
        
        case 'e':
            sleep(3);
            P_operation(unit_sem);
            player->unit[1]++;
            V_operation(unit_sem);
            break;

        case 'r':
            sleep(5);
            P_operation(unit_sem);
            player->unit[2]++;
            V_operation(unit_sem);
            break;
    }
}

void mark_attack(char command, player *player, attack_data *atkb, int unit_sem){
    switch(command){
        case 'v':
        if(player->unit[0] > 0){
            P_operation(unit_sem);
            player->unit[0]--;
            atkb ->unit[0]++;
            V_operation(unit_sem);
            break;
        }
        else{
            break;
        }
        case 'b':
        if(player->unit[1] > 0){
            P_operation(unit_sem);
            player->unit[1]--;
            atkb ->unit[1]++;
            V_operation(unit_sem);
            break;
        }
        else{
            break;
        }    
        case 'n':
        if(player->unit[2] > 0){
            P_operation(unit_sem);
            player->unit[2]--;
            atkb ->unit[2]++;
            V_operation(unit_sem);
            break;
        }
        else{
            break;
        }
        
    }
}


int ceiling(double var){
    int value;
    if(var - (int)var >0){
        value = (int)var +1;
    }else{
        value = (int)var;
    }
    return value;
}

double attack_strength(attack_data *atk){
double attack_power = atk->unit[0]*1.0 + atk->unit[1]*1.5 +atk->unit[2]*3.5;
return attack_power;
}

double defense_strength(player *player){
double defense_power = player->unit[0]*1.2 + player->unit[1]*3.0 +player->unit[2]*1.2;
return defense_power;
}

int recount_units(player *player, attack_data *atk, int unit_sem){
    int win;
    int soldiers_left_param;
    int soldiers_left_param2;
    double attack_power;
    double defense_power;
    attack_power = attack_strength(atk);
    printf("Siła ataku to: %f\n",attack_power);
    P_operation(unit_sem);
    defense_power = defense_strength(player);
    V_operation(unit_sem);
    printf("Siła obrony to: %f\n",defense_power);
    if(attack_power - defense_power > 0){
        if(defense_power < 1.0){
            defense_power = 1.0;
        }else if(attack_power <1.0){
            attack_power = 1.0;
        }
        soldiers_left_param = defense_power/attack_power;
        P_operation(unit_sem);
        for(int i=0; i<3; i++){
            player->unit[i] = 0;
            atk->unit[i] -= ceiling(atk->unit[i] * soldiers_left_param);
        }
        V_operation(unit_sem);
        win = 1;
    }else{
        if(defense_power < 1.0){
            defense_power = 1.0;
        }else if(attack_power <1.0){
            attack_power = 1.0;
        }
        soldiers_left_param = defense_power/attack_power;
        P_operation(unit_sem);
        for(int i=0;i<3;i++){
            player->unit[i] -= ceiling(player->unit[i] * soldiers_left_param);
        }
        V_operation(unit_sem);
        if(attack_power < 1.0){
            attack_power = 1.0;
        }
        soldiers_left_param2 = attack_power/defense_power;
        for(int i=0;i<3;i++){
            atk -> unit[i] -= ceiling(atk->unit[i] * soldiers_left_param2); 
        }
        win = 0;
    }
    return win;
}



server_ready bufr;
server_ready *sr = &bufr;
int *player_connected[3];
int shared_mem_id[3];
int resources_sem;
int queue_id[3];
int unit_sem[3];
int shm_players_ids[3];
player *detach_player[3];
attack_data *detach_atk[3];
int shm_atk_id[3];
int atk_sem[3];
int end_shm_id;
int *endgame;

void cleanup (){
    sr->value = 1;
    for(int i=0; i<3; i++){
        detach_shmem(player_connected[i]);
        detach_shmem(detach_player[i]);
        detach_shmem(detach_atk[i]);
        send_message_int(queue_id[i], sr, 200+i);
        delete_shmem_users(shared_mem_id[i]);
        delete_shmem_users(shm_players_ids[i]);
        delete_shmem_users(shm_atk_id[i]);
    }
    detach_shmem(endgame);
    delete_shmem_users(end_shm_id);
    sleep(1);
    for(int i=0; i<3;i++){
        remove_queue(queue_id[i]);
        remove_semaphore(unit_sem[i]);
        remove_semaphore(atk_sem[i]);
    }
    remove_semaphore(resources_sem);
    exit(0);
}

int main() {

    //inicjalizacja pamieci wspoldzielonej, inicjalizacja zmiennych polaczenia klientow zerami
    for(int i=0; i<3; i++){        
        shared_mem_id[i] = create_shmem_init(i);
        player_connected[i] = att_shmem(shared_mem_id[i]);
        *player_connected[i] = 0;
        queue_id[i] = create_queue_init(i);
}

    end_shm_id = create_shmem_end();
    endgame = att_shmem(end_shm_id);
    *endgame = 0;

    while ( *player_connected[0]!=1 
        || *player_connected[1]!=1 || *player_connected[2]!=1 ){
        sleep(1);
    }
    printf("Wszyscy gracze polaczeni\n");

server_ready buffer;
server_ready *srv = &buffer;
    srv->value = 1;
    for(int i = 0; i < 3; i++){
        send_message_int(queue_id[i], srv, 100 + i);
    }
  

    player *player[3];
    attack_data *atk[3];
    for(int i =0; i<3; i++){
        shm_atk_id[i] = create_shmem_atk(i);

        shm_players_ids[i]=create_shmem();
        detach_player[i] = player[i] = att_shmem(shm_players_ids[i]);
        detach_atk[i] = atk[i] = att_shmem(shm_atk_id[i]);
        player[i]->player_id = i;
        player[i]->resources_amount = 250;
        player[i]->workers = 0;
        player[i]->unit[0] = 0;
        player[i]->unit[1] = 0;
        player[i]->unit[2] = 0;
        atk[i] ->attacker_id = -3;
        atk[i] ->defender_id = -3;
        atk[i] ->unit[0] = 0;
        atk[i] ->unit[1] = 0;
        atk[i] ->unit[2] = 0;
        unit_sem[i] = create_semaphore2();
        initialize_semaphore(unit_sem[i]);
        atk_sem[i] = create_semaphore2();
        initialize_semaphore(atk_sem[i]);
    }
  


//podzial procesow na parent i client_processes
//tablica identyfikatorow procesow potomnych
int client_service_id[3];
//inicjalizacja client_service_id




resources_sem = create_semaphore2();
//int parent_id = getpid();
int check;

if((check = initialize_semaphore(resources_sem))== 0){
    if ((client_service_id[0] = fork()) != 0){
        if ((client_service_id[1] = fork()) != 0){
            if ((client_service_id[2] = fork()) != 0){
                signal(SIGINT,cleanup);
                //petla liczenia surowcow w procesie macierzystym
                while(1){
                    printf("Updating resources\n");
                    for(int i=0; i<3;i++){
                        update_resources(queue_id[i], player[i], resources_sem);
                        if(player[i]->victories == 5){
                            printf("Grę wygrał gracz %d",i);
                            *endgame = 1;
                            sleep(2);
                            cleanup();
                        }
                        }
                    sleep(1);
                    signal(SIGINT, cleanup);                    
                }
            }

        }

    }
}

//tworzenie jednostek
int training_process_id;
char unit;
char fight;
button_data bf;
button_data *rcv = &bf;
int rcv_data;
int rcv_data_atk_who;
int rcv_data_atk_unit;
int rcv_data_send_atk;
int atker, dfnder;
int units[3];
int parent;
int training_ppid;
int victory = 0;
for(int i=0; i<3; i++){
    if(client_service_id[i] == 0){
        training_process_id = fork();
        if (training_process_id==0){
            training_ppid = getppid();
        }
        if(training_process_id != 0){
            button_data buf;
            button_data *msg = &buf;
            while(*endgame != 1){
                rcv_data = receive_message_button(queue_id[i], rcv, i+4);
                rcv_data_atk_who = receive_message_attack(queue_id[i],atk[i],i+10);
                rcv_data_atk_unit = receive_message_button(queue_id[i],rcv,i+13);
                rcv_data_send_atk = receive_message_button(queue_id[i],rcv,i+19);
                if(rcv_data != -1 && semaphore_state(atk_sem[i]) == 1){
                    unit = rcv -> unit_type;
                    pay_units(unit, player[i], resources_sem);
                    if(unit != ']'){
                        msg->unit_type = unit;
                        send_message_button(queue_id[i], msg, i+7);
                    }
                }
                else if(rcv_data_atk_who != -1 && semaphore_state(atk_sem[i]) == 1){
                    printf("%d atakuje %d, jednostki %d %d %d\n", atk[i]->attacker_id,atk[i]->defender_id, atk[i]->unit[0],atk[i]->unit[1],atk[i]->unit[2]);
                    atker = atk[i]->attacker_id;
                    dfnder = atk[i]->defender_id;
                    P_operation(atk_sem[i]);
                }
                else if(rcv_data_atk_unit != -1 && semaphore_state(atk_sem[i]) == 0){
                    fight = rcv->unit_type;
                    atk[i]->attacker_id = atker;
                    atk[i] -> defender_id = dfnder;
                    mark_attack(fight, player[i],atk[i],unit_sem[i]);
                    for(int j=0; j<3;j++){
                        units[j] = atk[i]->unit[j];
                    }
                    send_message_attack(queue_id[i],atk[i],i+16); 
                }
                else if(rcv_data_send_atk != -1 && semaphore_state(atk_sem[i]) == 0){
                    atk[i] -> attacker_id = atker;
                    atk[i] -> defender_id = dfnder;
                    for(int j=0; j<3;j++){
                        atk[i]->unit[j] = units[j];
                    }
                    parent = fork();
                    if(parent !=0){
                        victory = recount_units(player[dfnder], atk[i], unit_sem[i]);
                        send_message_attack(queue_id[i], atk[i],i+22);
                        if(victory == 1){
                            player[i]->victories++;
                        }
                        dfnder = -3;
                        atker = -3;
                        for(int j=0; j<3;j++){
                            units[j] = 0;
                        }
                        V_operation(atk_sem[i]);
                    }
                    else{
                        break;
                    }
                }

            }
            if(*endgame == 1){
                exit(0);
            }

        }
        
        else if (training_ppid == getppid()){
            int enq_unit;
            button_data buf;
            button_data *msg = &buf;
            char unit;
            while(*endgame != 1){
                enq_unit = receive_message_button(queue_id[i],msg,i+7);
                if(enq_unit != -1){
                    unit = msg -> unit_type;
                    train_unit(unit, player[i], unit_sem[i]);

                }
            }

            if(*endgame == 1){
                exit(0);
            }
        }
        else{
            sleep(5);
            attack_data buff;
            attack_data *atk_retr = &buff;
            receive_message_attack(queue_id[i],atk_retr,i+22);
            P_operation(unit_sem[i]);
            for(int j=0; j<3;j++){
                player[i]->unit[j] +=  atk_retr->unit[j];
            }
            V_operation(unit_sem[i]);
            exit(0);
        }
    }
}


return 0;
}