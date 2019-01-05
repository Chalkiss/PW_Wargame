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
#include "sem-wrappers.h"
#include "queue-wrappers.h"
#include <ncurses.h>

char unit_type = '0';


int main(){
initscr();
noecho();
timeout(0);
refresh();   
    while(1){
            unit_type = getch();
            if(unit_type == 'w' || unit_type ==  'i' || unit_type == 'h' || unit_type == 'c'){
            mvprintw(0, 0, "%c\n",unit_type);
            refresh();
            }

    }

}