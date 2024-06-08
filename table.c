#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <sys/shm.h>
#include <unistd.h>


#define SHM_SIZE sizeof(struct shared_memory)
#define ARRAY_SIZE 10
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

int main() {
    key_t key;
    int shmid;
    struct shared_memory *shm;
    int id;
    printf("Enter Table Number- ");
    scanf("%d", &id);
    key = ftok("share", id);
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    shm = (struct shared_memory *)shmat(shmid, NULL, 0);
    
    shm->valid = 1;
    shm->end = 0;
    int child;
    int s=0;
    FILE *file;
    char filename[] = "menu.txt"; // Adjust to your file path
    char buffer[256];
    char *inrPtr, *priceStart, *priceEnd;
    int price;

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        inrPtr = strstr(buffer, "INR");
        if (inrPtr != NULL) {
            for (priceEnd = inrPtr - 1; priceEnd > buffer && isspace(*(priceEnd - 1)); priceEnd--);
            for (priceStart = priceEnd; priceStart > buffer && !isspace(*(priceStart - 1)); priceStart--);
	    char priceStr[10] = {0};
            strncpy(priceStr, priceStart, priceEnd - priceStart);
            price = atoi(priceStr);
            shm->prices[s] = price;
            s++;
        }
    }
    
    shm->menu = s;
    fclose(file);
    while(1){	    
           if(shm->valid == 1){	
	    	    printf("\nEnter Number of Customers at Table (maximum no. of customers can be 5: ");
		    scanf("%d",&child);	
		    if(child == -1){
			shm->end = 1;
			shm->flag = 1;
			break;
		    }
		    file = fopen(filename, "r");
		    if (file == NULL) {
			perror("Error opening file");
			exit(EXIT_FAILURE);
		    }
    		    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        	        printf("%s",buffer);
    		    }
    		    fclose(file);	
	   }		    
	    pid_t pid;
	    int cnt=0;
	    for(int i=0;i<child;i++){
	    
	    	int fd[2];
	    	if(pipe(fd) == -1){
			perror("Pipe failed");
			exit(EXIT_FAILURE);
		}
		pid = fork();
		if(pid == -1){
			perror("Fork failed");
			exit(EXIT_FAILURE);
		}
		if(pid == 0){
			close(fd[0]);
			int arr[ARRAY_SIZE];
			int j=0;
			printf("\nEnter the serial number(s) of the item(s) to order from the menu by customer %d. Enter -1 when done: ",i+1);
			for(;j<ARRAY_SIZE;j++){
				scanf("%d",&arr[j]);
				if(arr[j] == -1){
					break;
				}
			}
			write(fd[1],arr,sizeof(arr));
			close(fd[1]);
			exit(0);
		}
		else{
			close(fd[1]);
			int arr[ARRAY_SIZE];
			if(read(fd[0],arr,sizeof(arr)) > 0){
				for(int k=0;arr[k] != -1;k++){
					shm->array[cnt] = arr[k];
					cnt++;
				}
			}		
			close(fd[0]);
		}
	     }			
			
			
	    shm->count = cnt;
	    shm->valid = 1;
	    shm->flag = 1;
	    printf("Waiting for the order to be recieved\n");
	    while (shm->flag != 2) {
        	sleep(1); 
    	    }
    	    if(shm->valid == 1){
		printf("The total bill amount is %d INR\n",shm->sum);
	    }else{
		printf("Invalid Order Item! Please Order Again\n");	
	    }
	    
    }  
    
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;   

}


