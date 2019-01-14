#ifndef QUEUE_WRAPPERS_H
#define QUEUE_WRAPPERS_H
#include "queue-wrappers.c"

int create_queue();
int send_message(int queue_id, player *msg, long type);
int receive_message(int queue_id, player *buffer, long type);
int remove_queue(int queue_id);
int create_queue_w_key(key_t key);
int create_queue_init(int identifier);
int send_message_button(int queue_id, button_data *msg, long type);
int send_message_int(int queue_id, server_ready *msg, long type);
int send_message_attack(int queue_id, attack_data *msg, long type);
int receive_message_button(int queue_id, button_data *buffer, long type);
int receive_message_int(int queue_id, server_ready *buffer, long type);
int receive_message_attack(int queue_id, attack_data *buffer, long type);

#endif