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
#include "shm-wrappers.h"
#include "sem-wrappers.h"

typedef struct {
    int player_id;
    int resources_amount;
    int workers;
    int unit[3];
    int resources_income;
    int victories;
}player;

struct queue_buffer{
    long mtype;
    player msg;
};

typedef struct{
    int id;
    player *addr;
    int semaphore;
}shared_player_data;

typedef struct{
    char unit_type;
}train_data;

struct training_buffer{
    long mtype;
    train_data msg;
};

int create_queue(){
    int queue_id;
    if ((queue_id = msgget(IPC_PRIVATE, IPC_CREAT|0640))== -1){
        perror("queue error - creating a player queue");
    }
    return queue_id;
}

int create_queue_w_key(key_t key){
    int queue_id;
    if ((queue_id = msgget(key, IPC_CREAT|0640))== -1){
        perror("queue error - creating a player queue");
    }
    return queue_id;
}

int create_queue_init(int identifier){
    int queue_id;
    if ((queue_id = msgget(1984 + identifier, IPC_CREAT|0640))== -1){
        perror("queue error - creating a player queue");
    }
    return queue_id;
}

int send_message(int queue_id, player *msg, long type){
    int message_sent;
    struct queue_buffer tmp;
    struct queue_buffer *buf = &tmp;
    buf->mtype = type;
    buf->msg = *msg;
    if((message_sent = msgsnd(queue_id, buf ,sizeof(player),0)) == -1){
        perror("queue error - sending player");
    }
    return message_sent;
}

int send_message_train(int queue_id, train_data *msg, long type){
    int message_sent;
    struct training_buffer tmp;
    struct training_buffer *buf = &tmp;
    buf->mtype = type;
    buf->msg = *msg;
    if((message_sent = msgsnd(queue_id, buf ,sizeof(train_data),0)) == -1){
        perror("queue error - sending player");
    }
    return message_sent;
}


int receive_message(int queue_id, player *buffer, long type){
    int result;
    struct queue_buffer tmp;
    struct queue_buffer *ptr = &tmp;
    if((result = msgrcv(queue_id, ptr, sizeof(struct queue_buffer), type, IPC_NOWAIT)) == -1){
        if(errno != ENOMSG){
            perror("queue error - receiving player" );
        }
    }
    *buffer = ptr->msg;
    return result;
}

int receive_message_train(int queue_id, train_data *buffer, long type){
    int result;
    struct training_buffer tmp;
    struct training_buffer *ptr = &tmp;
    if((result = msgrcv(queue_id, ptr, sizeof(struct training_buffer), type, IPC_NOWAIT)) == -1){
        if(errno != ENOMSG){
            perror("queue error - receiving player" );
        }
    }
    *buffer = ptr->msg;
    return result;
}

int remove_queue(int queue_id){
    int result;
    if((result = msgctl(queue_id, IPC_RMID, 0)) == -1){
        perror("queue error - removing queue");
    }
    return result;
}