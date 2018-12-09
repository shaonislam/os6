#ifndef GLOBALS_H
#define GLOBALS_H

/*
struct process_page{
	int procNum; 
	int procID; 
	int filledwith;  
	int reference_bit; 
	int valid_bit;
	int dirty_bit;
};
*/

int shm_id;

struct msg_buffer {
	long msg_type;
	char msg_text[1000];
} message;






#endif
