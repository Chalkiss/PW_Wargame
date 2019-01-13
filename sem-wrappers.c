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
#include "queue-wrappers.h"


// initialization of the sempahores
struct sembuf p = {0, -1, 0};
struct sembuf v = {0, 1, 0};

//Wrapper for semaphore creation with key
int create_semaphore(key_t key){
    int semid;
    if((semid = semget(key, 1, IPC_CREAT|0640))== -1){
        perror ("sem error - creating a semaphore");
    }
    return semid;
}

//Wrapper for semaphore creation without key
int create_semaphore2(){
    int semid;
    if((semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0640))== -1){
        perror("sem error - creating a semaphore");
    }
    return semid;
}

//Wrapper for semaphore initialization
int initialize_semaphore(int semid){
    int state;
    if((state = semctl(semid, 0, SETVAL, 1))== -1){
        perror("sem error - initializing a semaphore");
    }
    return state;
}

//Wrapper for single semaphore removal
int remove_semaphore(int semid){
    int state;
    if((state = semctl(semid, 0, IPC_RMID, NULL))== -1){
        perror("sem error - removing a semaphore");
    }
    return state;
}

int semaphore_state(int semid){
    int state;
    if((state = semctl(semid, 0, GETVAL, NULL))== -1){
        perror("sem error - checking sem value");
    }
    return state;
}

//Wrapper for setting semaphore down
int P_operation(int semid){
    int state;
    if((state = semop(semid, &p, 1))== -1){
        perror("sem error - setting semaphore down");
    }
    return state;
}

//Wrapper for setting semaphore up
int V_operation(int semid){
    int state;
    if ((state = semop(semid, &v, 1))== -1){
        perror("sem error - setting semaphore up");
    }
    return state;
}