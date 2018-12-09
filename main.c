/*
 Main.c 
 Shaon Islam
 CS 4760: Project 6
 December 2018
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include "globals.h"


struct table_info{
        int procNum;                     
        int procId;              
        int filledWith;                 
        int referenceBit;             
        int validBit;
        int dirtyBit;
};


void handle_terminate(int sig)
{
        fprintf(stderr, "Terminating due to 2 second program life span is over.\n");
	shmctl(shm_id, IPC_RMID, NULL);
        exit(1);
}


int main (int argc, char *argv[])
{


	int option;
	int max_spawn;
	FILE *fname;
	fname = fopen("log.txt", "w+");
	int loglines = 0;
	srand((unsigned)time(NULL));
	char arg1[10];
	int random_address;
	int random_bit;

	/*______Set up signal______*/
        signal(SIGALRM, handle_terminate);
        alarm(2);	



	/*______Set GETOPT______*/
        while((option = getopt(argc, argv, "hm:")) != -1)
        {
                switch (option)
                        {
				case 'm':
					max_spawn  = atoi(optarg); 
					fprintf(stderr, "Max children processes: %d\n", max_spawn);
					break;
				case 'h':
					fprintf(stderr, "How it should run: \n");
	                                fprintf(stderr, "	-m  x: x is the max number of user process to spawn (default: 5)\n");
					exit(1);
         	               case '?':
                	                fprintf(stderr, "Error\n");
                        	        break;
                        }
        
	}

	/* If spawn input is bigger than 18, default to 18 */

	if (max_spawn > 18 || max_spawn == 0) 
	{	
		max_spawn = 18;
		fprintf(stderr, "Invalid Input: Max processes changed to: %d\n\n", max_spawn);
	}


	/* _______________Set Up Tables _____________ */

	
	/*___FRAME__*/
	struct table_info frame_table [256];
	int i;
	for (i = 0; i < 256; i++)
	{
		frame_table[i].filledWith = -1;		
	}


	/*_________Setup Shared Memory For Master Clock_________*/

	int shm_id;
        key_t key;
        void* shm;
        key = 6789;
        shm_id = shmget(key, sizeof(int)*2, IPC_CREAT | 0666);
        if (shm_id == -1)
        {
                perror("shmget");
                exit(1);
        }
        shm = shmat(shm_id, NULL, 0);



	/*_________PUT THE CLOCK IN SHARED MEMORY_________ */

        int*  master_clock = shm;  
	int seconds = 0;
	int nanosec = 0;
	master_clock[0] = seconds;
	master_clock[1] = nanosec;


	/*________Set up Message Q for Reader______*/
/*
	key_t key_msg;
	int msgid;
	key = ftok("/user.c", 'b');
	msgid = msgget(key_msg, 0666 | IPC_CREAT);

	msgrcv(msgid, &message, sizeof(message), 1, 0);
*/	/* message is in message.msg_text */


	/*_________Setup Shared Mail to Receive_________*/

        int shmMSG_id;
        key_t key_msg;
        void* shm_msg;
        key_msg = 2222;
        shmMSG_id = shmget(key_msg, sizeof(int)*2, IPC_CREAT | 0666);
        shm_msg = shmat(shmMSG_id, NULL, 0);
	int* shmail = shm_msg;



	/*_______Creating an array of sorted random times to fork kids______*/
	int b, c, d;
	int rand_array [max_spawn];
	for (b = 0; b < 5; b++)
	{	int random_time = rand() % 1000000;
		rand_array[b] = random_time;
	}	
	/*___ sorting random times ascending____ */;
	for (c = 0; c < max_spawn; c++)     
	{
		for (d = 0; d < max_spawn; d++)            
		{
			if (rand_array[d] > rand_array[c])               
			{
				int tmp = rand_array[c];         
				rand_array[c] = rand_array[d];         
				rand_array[d] = tmp;             			
			}  
		}
	}

        /*_________Creating User Processes_______*/
	
	int live_processes = 0;
	while (loglines < 100 && master_clock[0] < 2)
	{

		/* Make Processes -but not more than 5 */
		if (live_processes < max_spawn)
		{
	

			/* Check if random time has passed for Fork() */
			if (rand_array[live_processes] < master_clock[1])
			{
				/*___prep random____*/
				random_bit = rand() % 2;
				random_address = rand() % 256;
				char arg2[10];
				snprintf(arg2 , 10, "%d", random_bit);


				/*________Create Page Table for Process________*/
				int page_table [33] = {-1};
				page_table[0] = live_processes;
				snprintf(arg1 , 10, "%d", page_table[0]);

				/*___ Fork Process__ */
				pid_t child_pid = 0;
				child_pid = fork();
        			if (child_pid == 0)
       				{
					fprintf(fname, "Master: Process %d created at time %d:%d\n", live_processes, master_clock[0], master_clock[1]);		
					loglines++;
					execlp("./user", "./user", arg1, arg2, (char *)NULL);		
        			}

				if (child_pid)
				{
					live_processes++;
				}
				wait(NULL);
			}
		}

		/*______Increment Master Clock_______*/	
		/* Increment System Clock BY 100000 Nanoseconds */
		master_clock[1] = master_clock[1] + 100000;
        	if (master_clock[1] > 999999999)
        	{
        		master_clock[1] = master_clock[1] %  1000000000;
                	master_clock[0] = master_clock[0] + 1;
        	}


		/* ___Check shm mail  for Message____ */
		if(shmail[0] != 0 || shmail[3] != 0)
		{
			/* Recieved a message */
			if (shmail[2] == 0)
			{
				fprintf(fname, "Master : Process %d want to READ at Address: %d at %d:%d\n", shmail[3], random_address, master_clock[0], master_clock[1]); 
				loglines++;
			} 
			else
			{
				fprintf(fname, "Master : Process %d want to WRITE at Address: %d at %d:%d\n", shmail[3], shmail[1], master_clock[0], master_clock[1]);
				loglines++;
			}		

			/*__________Check if Address Available in FRAME TABLE _______*/
			if (frame_table[random_address].filledWith == -1)
			{
	                        if (random_bit  == 0)
                      	  	{
                                	fprintf(fname, "Master : Address: %d in frame %d, GIVING data to Process %d at %d:%d\n", random_address, (random_address/256), shmail[3], master_clock[0], master_clock[1]);
                                	loglines++;
                        	}
                        	else
                        	{
                                	fprintf(fname, "Master : Address: %d in frame %d, WRITING data to Process %d at %d:%d\n", random_address, (random_address/256), shmail[3], master_clock[0], master_clock[1]);
                                	loglines++;
                        	}
			}

			/*__set bits__*/
			/*___ Address not in frame___ */
			else{
				fprintf(fname, "Master : Address %d is not in a frame, page fault\n", random_address);
				loglines++;
			}

			/* Wait for user to finish */
			waitpid(shmail[0], NULL, 0);

			/* Decrement Number of Live Processes */
			/*live_processes--;*/	

			/* Reset */
			shmail[0] = 0;
			shmail[1] = 0;
			shmail[2] = 0;
			shmail[3] = 0;
		}
	}
	


	/*______Clean Out Shared Memory_______*/
	fprintf(stderr, "\n*** Program Finished: Please view log.txt ***\n\n\n");
	shmctl(shm_id, IPC_RMID, NULL);
/*      msgctl(msgid, IPC_RMID, NULL); */
	fclose(fname);
	return 0;
}



