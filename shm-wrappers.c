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
#include "sem-wrappers.h"
#include "queue-wrappers.h"

int create_shmem_init(int identifier){
    //Wrapper function for initial communication of server and clients
    int mem_id;
    if((mem_id = shmget(1984 + identifier,sizeof(int),IPC_CREAT|0640))== -1){
        perror("shm error - initial shmem creation");
    }
    return mem_id;
}

int create_shmem(){
    //Wrapper function for initial communication of server and clients
    int mem_id;
    if((mem_id = shmget(IPC_PRIVATE,sizeof(player),IPC_CREAT|0640))== -1){
        perror("shm error - initial shmem creation");
    }
    return mem_id;
}

int create_shmem_atk(int identifier){
    //Wrapper function for initial communication of server and clients
    int mem_id;
    if((mem_id = shmget(ftok("./server",identifier),sizeof(attack_data),IPC_CREAT|0640))== -1){
        perror("shm error - initial shmem creation");
    }
    return mem_id;
}

int create_shmem_end(){
    //Wrapper function for initial communication of server and clients
    int mem_id;
    if((mem_id = shmget(ftok("./queue-wrappers.h",0),sizeof(int),IPC_CREAT|0640))== -1){
        perror("shm error - initial shmem creation");
    }
    return mem_id;
}


void* att_shmem(int mem_id){
    void *adr;
  if((adr = shmat(mem_id,0,0)) == (void*)-1){
      perror(" shm error - problem attaching shmem");
  }
  return adr;
}

int detach_shmem(void* memory_adr){
    int detach_value;
    if((detach_value =shmdt(memory_adr))== -1){
        perror("shm error - problem detaching shmem");
    }
    return detach_value;    
}

int delete_shmem_users(int mem_id){
    int delete_value;
    if((delete_value = shmctl(mem_id,IPC_RMID, 0))== -1){
        perror("shm error - problem removing user processes");
    }
    return delete_value;
}