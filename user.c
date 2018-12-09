/*
USER.C
 Shaon Islam
 CS 4760: 
 December 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <semaphore.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <errno.h>
#include "globals.h"

int main (int argc, char *argv[])
{

/*	srand((unsigned)time(NULL));
	int random_bit = rand() % 3 ;
	fprintf(stderr, "rando bit: %d\n", random_bit); */
	int random_address = rand() % 256;
	int arg1 = atoi(argv[1]);
	int random_bit = atoi(argv[2]);

	/*_________See Master Clock_________*/

	int shm_id;
        key_t key;
        void* shm;
        key = 6789;
        shm_id = shmget(key, sizeof(int)*2, IPC_CREAT | 0666);
        shm = shmat(shm_id, NULL, 0);
	int* master_clock = shm;

	
	/*________Set up Message Queue for Writer_____*/
/*	key_t key_msg;
	int msgid;
*/
	/* ftok to generate unique key */
/*	key_msg = ftok("/user.c", 'b');
*/	
	/*msgget creates a message q and returns identifier*/	
/*	msgid = msgget(key_msg, 0666 | IPC_CREAT);
	message.msg_type = 1;
*/
	/* msgsnd to send message */
	/* message.msg_text */

	
	/*_____________Send Address and Bit Info to OSS_________*/

        /*_________Setup shmMSG_________*/
	
	int shmMSG_id;
	key_t key_msg;
	void* shm_msg;
	key_msg = 2222;
	shmMSG_id = shmget(key_msg, sizeof(int)*2, IPC_CREAT | 0666);
        shm_msg = shmat(shmMSG_id, NULL, 0);

	/*_________Put Term_time in SHM _________*/
	
	int* shmail = shm_msg;
	shmail[0] = 0; /* process id */
	shmail[1] = 0; /* Address desired */	
	shmail[2] = 0; /* Read or Write Bit */
	shmail[3] = 0; /* process num */


	/*_________Setup Semaphore to Send Address And BIT Info_________*/

	sem_t *sem; /*synch semaphore*/
	sem = sem_open("thisSem", O_CREAT|O_EXCL, 0644, 10);
	sem_unlink("thisSem");


	/*_________Check System Clock Until Deadline _________*/

	/*______ENTERING CRITICAL SECTION_____*/

	sem_wait(sem);

			shmail[0] = getpid(); /*PID*/
			shmail[1] = random_address; /*Address*/
			shmail[2] = random_bit; /*Read or Write*/
			shmail[3] = arg1; /*process num*/
			sem_post(sem);
			exit(0);

	sem_destroy(sem);	


	return 0;
}
