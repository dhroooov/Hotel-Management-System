#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include<string.h>

#define SHM_SIZEA sizeof(struct shared_memoryA)

struct shared_memoryA {
    	int flag;
};

int main(){
	int shmida;
	struct shared_memoryA *shma;
    	key_t key;
    	key = ftok("Ids.txt",'A');
    	if (key == -1) {
        	perror("ftok failed");
        	exit(EXIT_FAILURE);
    	}
    	shmida = shmget(key, SHM_SIZEA, 0644 | IPC_CREAT);
    	if (shmida == -1) {
        	perror("shmget failed");
        	exit(EXIT_FAILURE);
    	}
    	shma = (struct shared_memoryA *)shmat(shmida, NULL, 0);
    	shma->flag = 0; 

    	char input[2];
    	while (1) {
        	printf("Do you want to close the hotel? Enter Y for Yes and N for No.\n");
        	scanf("%s", input);

        	if (strcmp(input, "Y") == 0 || strcmp(input, "y") == 0) {
            		shma->flag = 1;
            		break; 
        	} else if (strcmp(input, "N") == 0 || strcmp(input, "n") == 0) {
            		
        	} else {
            		printf("Invalid input. Please enter Y or N.\n");
        	}
    	}
    	shmdt(shma);
    	return 0;
}    	
