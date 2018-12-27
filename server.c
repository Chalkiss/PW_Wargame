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

int main() {
    int shared_mem_id[3];
int *player_connected[3];
for(int i=0; i<3; i++){
    shared_mem_id[i] = shmget(1984+ i,sizeof(int),IPC_CREAT|0640);
    perror("jeszcze inny komunikat\n");
    player_connected[i] = shmat(shared_mem_id[i],0,0);
    perror("jakis komunikat");
    *player_connected[i] = 0;
}


while ( *player_connected[0]!=1 
        || *player_connected[1]!=1 || * player_connected[2]!=1 ){
    sleep(1);
}
printf("Wszyscy gracze polaczeni\n");

for(int i=0; i<3; i++){
        shmdt(player_connected[i]);
        shmctl(shared_mem_id[i],IPC_RMID, 0);
    }

return 0;
}