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
int main() {
    int shared_mem_id[3];
int *player_connected[3];
for(int i=0; i<3; i++){
    shared_mem_id[i] = create_shmem_init(i);
    player_connected[i] = att_shmem(shared_mem_id[i]);
    *player_connected[i] = 0;
}


while ( *player_connected[0]!=1 
        || *player_connected[1]!=1 || * player_connected[2]!=1 ){
    sleep(1);
}
printf("Wszyscy gracze polaczeni\n");

for(int i=0; i<3; i++){
        detach_shmem(player_connected[i]);
        delete_shmem_users(shared_mem_id[i]);
    }

return 0;
}