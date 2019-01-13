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
        case 'w':
            if(player -> resources_amount >= 150){
                player -> resources_amount -= 150;
                V_operation(resources_sem);
            }else{
                unit = 'e'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }
            break;

        case 'i':
            if(player -> resources_amount >= 100){
                player -> resources_amount -= 100;
                V_operation(resources_sem);
            }else{
                unit = 'e'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }
            break;
            
        case 'h':
            if(player -> resources_amount >= 350){
                player -> resources_amount -= 350;
                V_operation(resources_sem);
            }else{
                unit = 'e'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }            break;

        case 'c':
            if(player -> resources_amount >= 550){
                player -> resources_amount -= 550;
                V_operation(resources_sem);                
            }else{
                unit = 'e'; //unit error type, nie trenuj jednostek
                V_operation(resources_sem);
            }
            break;

    }

}

void train_unit(char unit, player *player, int unit_sem){
    switch(unit)
    {

        case 'w':
            printf("Jestem w train_unit switchu\n");
            sleep(2);
            P_operation(unit_sem);
            player->workers++;
            V_operation(unit_sem);
            break;

        case 'i':
            sleep(2);
            P_operation(unit_sem);
            player->unit[0]++;
            V_operation(unit_sem);            
            break;
        
        case 'h':
            sleep(3);
            P_operation(unit_sem);
            player->unit[1]++;
            V_operation(unit_sem);
            break;

        case 'c':
            sleep(5);
            P_operation(unit_sem);
            player->unit[2]++;
            V_operation(unit_sem);
            break;
    }
};

server_ready bufr;
server_ready *sr = &bufr;
int *player_connected[3];
int shared_mem_id[3];
int resources_sem;
int queue_id[3];
int unit_sem[3];
int shm_players_ids[3];
player *detach_player[3];
void cleanup (){
    sr->value = 1;
    for(int i=0; i<3; i++){
        detach_shmem(player_connected[i]);
        detach_shmem(detach_player[i]);
        send_message_int(queue_id[i], sr, 200+i);
        delete_shmem_users(shared_mem_id[i]);
        delete_shmem_users(shm_players_ids[i]);
    }
    sleep(1);
    for(int i=0; i<3;i++){
        remove_queue(queue_id[i]);
        printf("removing unit sem...\n");
        remove_semaphore(unit_sem[i]);
    }
    remove_semaphore(resources_sem);
    exit(1);
}

int main() {
    //utworzenie kolejek komunikatow dla kazdego z procesow obslugujacych klienta




    //inicjalizacja pamieci wspoldzielonej, inicjalizacja zmiennych polaczenia klientow zerami
 

    for(int i=0; i<3; i++){
        
        shared_mem_id[i] = create_shmem_init(i);
        player_connected[i] = att_shmem(shared_mem_id[i]);
        *player_connected[i] = 0;
        queue_id[i] = create_queue_init(i);
}


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

    for(int i =0; i<3; i++){
        shm_players_ids[i]=create_shmem();
        detach_player[i] = player[i] = att_shmem(shm_players_ids[i]);
        player[i]->player_id = i;
        player[i]->resources_amount = 250;
        player[i]->workers = 0;
        player[i]->unit[0] = 0;
        player[i]->unit[1] = 0;
        player[i]->unit[2] = 0;
        unit_sem[i] = create_semaphore2();
        initialize_semaphore(unit_sem[i]);
    }
  


//podzial procesow na parent i client_processes
//tablica identyfikatorow procesow potomnych
int client_service_id[3];
//inicjalizacja client_service_id

for (int i=0; i<3; i++){
    client_service_id[i] = -5;
}



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
train_data bf;
train_data *rcv = &bf;
int rcv_data;

for(int i=0; i<3; i++){
    if(client_service_id[i] == 0){
        training_process_id = fork();
        if(training_process_id != 0){
            train_data buf;
            train_data *msg = &buf;
            while(1){
                rcv_data = receive_message_train(queue_id[i], rcv, i+4);
                if(rcv_data != -1){
                    unit = rcv -> unit_type;
                    pay_units(unit, player[i], resources_sem);
                    if(unit != 'e'){
                        msg->unit_type = unit;
                        send_message_train(queue_id[i], msg, i+7);
                    }
                }
            }

        }
        
        else{
            int enq_unit;
            train_data buf;
            train_data *msg = &buf;
            char unit;
            while(1){
                enq_unit = receive_message_train(queue_id[i],msg,i+7);
                if(enq_unit != -1){
                    unit = msg -> unit_type;
                    train_unit(unit, player[i], unit_sem[i]);

                }
            }
        }
    }
}

remove_semaphore(resources_sem);


for(int i=0; i<3; i++){
        detach_shmem(player_connected[i]);
        delete_shmem_users(shared_mem_id[i]);
    }

return 0;
}