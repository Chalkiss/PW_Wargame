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
    printf("Updating resources\n");
    P_operation(semaphore);
    player->resources_amount = player->resources_amount + 50+ 5*(player->workers);
    send_message(client_queue_id, player, player->player_id+1);
    V_operation(semaphore);
}

void train_units(char unit, player *player, int resources_sem, int unit_sem){
    P_operation(resources_sem);

    if(unit == 'w' && player->resources_amount >= 150){
        player->resources_amount = player->resources_amount - 150;
        V_operation(resources_sem);
        sleep(2);
        P_operation(unit_sem);
        player->workers = player->workers + 1;
        V_operation(unit_sem);
    }

    else if(unit == 'i' && player->resources_amount >= 100){
        player->resources_amount = player->resources_amount - 100;
        V_operation(resources_sem);
        sleep(2);
        P_operation(unit_sem);
        player->unit[0] = player->unit[0] + 1;
        V_operation(unit_sem);
    }

    else if(unit == 'h' && player->resources_amount >= 350){
        player->resources_amount = player->resources_amount - 350;
        V_operation(resources_sem);
        sleep(3);
        P_operation(unit_sem);
        player->unit[1] = player->unit[1] + 1;
        V_operation(unit_sem);
    }

    else if(unit == 'c' && player->resources_amount >= 550){
        player->resources_amount = player->resources_amount - 550;
        V_operation(resources_sem);
        sleep(5);
        P_operation(unit_sem);
        player->unit[2] = player->unit[2] + 1;
        V_operation(unit_sem);
    }

    else V_operation(resources_sem);

}


int main() {
    //utworzenie kolejek komunikatow dla kazdego z procesow obslugujacych klienta




    //inicjalizacja pamieci wspoldzielonej, inicjalizacja zmiennych polaczenia klientow zerami


    int shared_mem_id[3];
    int *player_connected[3];
    int queue_id[3];
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
  
    int shm_players_ids[3];
    player *player[3];

    for(int i =0; i<3; i++){
        shm_players_ids[i]=create_shmem();
        player[i] = att_shmem(shm_players_ids[i]);
        player[i]->player_id = i;
        player[i]->resources_amount = 250;
        player[i]->workers = 0;
        player[i]->unit[0] = 0;
        player[i]->unit[1] = 0;
        player[i]->unit[2] = 0;
    }
  


//podzial procesow na parent i client_processes
//tablica identyfikatorow procesow potomnych
int client_service_id[3];
//inicjalizacja client_service_id

for (int i=0; i<3; i++){
    client_service_id[i] = -5;
}



int resources_sem = create_semaphore2();
//int parent_id = getpid();
int check;

if((check = initialize_semaphore(resources_sem))== 0){
    if ((client_service_id[0] = fork()) != 0){
        if ((client_service_id[1] = fork()) != 0){
            if ((client_service_id[2] = fork()) != 0){
                //petla liczenia surowcow w procesie macierzystym
                while(1){
                    for(int i=0; i<3;i++){
                        update_resources(queue_id[i], player[i], resources_sem);
                            }
                    sleep(1);
                    
                }
            }

        }

    }
}

//tworzenie jednostek
int unit_sem = create_semaphore2();
initialize_semaphore(unit_sem);
char unit;
train_data bf;
train_data *rcv = &bf;
int rcv_data;
for(int i=0; i<3; i++){
    if(client_service_id[i] == 0){
        while(1){
            rcv_data = receive_message_train(queue_id[i],rcv,i+4);
            if(rcv_data != -1){
                unit = rcv->unit_type;
                train_units(unit, player[i],resources_sem, unit_sem);
            }
        }
    }
}


remove_semaphore(resources_sem);
remove_semaphore(unit_sem);

for(int i=0; i<3; i++){
        detach_shmem(player_connected[i]);
        delete_shmem_users(shared_mem_id[i]);
    }

return 0;
}