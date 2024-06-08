#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE sizeof(struct shared_memory)
#define ARRAY_SIZE 10
#define SHM_SIZEH sizeof(struct shared_memoryH)
#define MENU_SIZE 9

struct shared_memory {
    int array[5*ARRAY_SIZE];
    int prices[MENU_SIZE];
    int menu;
    int flag;
    volatile int valid;
    int count;
    int end;
    int sum;
};
struct shared_memoryH {
	int sum;
	volatile int flag;
	int end;
};


int main() {
    key_t key;
    key_t keyH;
    int shmid;
    int shmidH;
    struct shared_memory *shm;
    struct shared_memoryH *shmH;
    int id;
    printf("Enter Waiter ID: ");
    scanf("%d", &id);
    key = ftok("share", id);
    keyH = ftok("waiterHM.txt", id);
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    if (keyH == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    shm = (struct shared_memory *)shmat(shmid, NULL, 0);
    shmidH = shmget(keyH, SHM_SIZEH, 0644 | IPC_CREAT);
    if (shmidH == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    shmH = (struct shared_memoryH *)shmat(shmidH, NULL, 0);

    while(1){	     
    	    printf("Waiting for the order...\n");
            while(shm->flag != 1) {
		sleep(1);
	    }
	    shmH->flag = 0;
    	    if(shm->end == 1){
    	    	shmH->end = 1;
    	    	shmdt(shmH);
		shmdt(shm);
		shmctl(shmid, IPC_RMID, NULL);
		return 0;
	    }	
	    
	    shm->valid = 1;	
	    printf("Order Received\n");
	    int cost=0;
	    for (int i = 0; i < shm->count; i++) {
	    	if(shm->array[i]>=1 && shm->array[i] <= shm->menu){
			cost += shm->prices[shm->array[i]-1];
		}
		else{
			shm->valid = 0;
			cost = -1;
			break;
		}		
	    }

	    if(shm->valid == 1){
		printf("Total Bill of table %d is %d\n",id,cost);
		shmH->sum = cost;
		shmH->flag = 1;
	    }else{
		printf("Please Enter Valid Items\n");
	    }	
		
	    shm->sum = cost;
	    shm->flag = 2;

    }
    shmdt(shm);

    return 0;
}


