#ifndef SHM_WRAPPERS_H
#define SHM_WRAPPERS_H
#include "shm-wrappers.c"

int create_shmem_init(int identifier);
void* att_shmem(int mem_id);
int detach_shmem(void* memory_adr);
int delete_shmem_users(int mem_id);
int create_shmem();
int create_shmem_atk(int identifier);

#endif