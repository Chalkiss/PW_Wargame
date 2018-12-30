#ifndef SEM_WRAPPERS_H
#define SEM_WRAPPERS_H
#include "sem-wrappers.c"

int create_semaphore(key_t key);
int create_semaphore2();
int initialize_semaphore(int semid);
int remove_semaphore(int semid);
int P_operation(int semid);
int V_operation(int semid);
#endif