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
}button_data;

typedef struct server_ready{
    int value;
}server_ready;

struct training_buffer{
    long mtype;
    button_data msg;
};

struct int_buffer{
    long mtype;
    server_ready value;
};

typedef struct {
    int attacker_id;
    int defender_id;
    int unit[3];
}attack_data;

struct attacking_buffer {
    long mtype;
    attack_data msg;
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
    if ((queue_id = msgget(1894 + identifier, IPC_CREAT|0640))== -1){
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

int send_message_button(int queue_id, button_data *msg, long type){
    int message_sent;
    struct training_buffer tmp;
    struct training_buffer *buf = &tmp;
    buf->mtype = type;
    buf->msg = *msg;
    if((message_sent = msgsnd(queue_id, buf ,sizeof(button_data),0)) == -1){
        perror("queue error - sending player");
    }
    return message_sent;
}

int send_message_int(int queue_id, server_ready *msg, long type){
    int message_sent;
    struct int_buffer tmp;
    struct int_buffer *buf = &tmp;
    buf->mtype = type;
    buf->value = *msg;
    if((message_sent = msgsnd(queue_id, buf ,sizeof(server_ready),0)) == -1){
        perror("queue error - sending player");
    }
    return message_sent;
}

int send_message_attack(int queue_id, attack_data *msg, long type){
    int message_sent;
    struct attacking_buffer tmp;
    struct attacking_buffer *buf = &tmp;
    buf->mtype = type;
    buf->msg = *msg;
    if((message_sent = msgsnd(queue_id, buf ,sizeof(attack_data),0)) == -1){
        perror("queue error - sending attack data");
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

int receive_message_button(int queue_id, button_data *buffer, long type){
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

int receive_message_int(int queue_id, server_ready *buffer, long type){
    int result;
    struct int_buffer tmp;
    struct int_buffer *ptr = &tmp;
    if((result = msgrcv(queue_id, ptr, sizeof(struct int_buffer), type, IPC_NOWAIT)) == -1){
        if(errno != ENOMSG){
            perror("queue error - receiving player" );
        }
    }
    *buffer = ptr->value;
    return result;
}

int receive_message_attack(int queue_id, attack_data *buffer, long type){
    int result;
    struct attacking_buffer tmp;
    struct attacking_buffer *ptr = &tmp;
    if((result = msgrcv(queue_id, ptr, sizeof(struct attacking_buffer), type, IPC_NOWAIT)) == -1){
        if(errno != ENOMSG){
            perror("queue error - receiving attack data" );
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