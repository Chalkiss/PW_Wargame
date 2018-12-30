#ifndef QUEUE_WRAPPERS_H
#define QUEUE_WRAPPERS_H
#include "queue-wrappers.c"

int create_queue();
int send_message(int queue_id, player *msg, long type);
int receive_message(int queue_id, player *buffer, long type);
int remove_queue(int queue_id);
int create_queue_w_key(key_t key);
int create_queue_init(int identifier);

#endif