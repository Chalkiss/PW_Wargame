#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <errno.h>

int create_shmem_init(int identifier){
    //Wrapper function for initial communication of server and clients
    int mem_id;
    if((mem_id = shmget(1984 + identifier,sizeof(int),IPC_CREAT|0640))== -1){
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