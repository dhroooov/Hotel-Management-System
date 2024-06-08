#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE sizeof(struct shared_memory)
#define SHM_SIZEA sizeof(struct shared_memoryA)

struct shared_memory {
	int sum;
	volatile int flag;
	int end;
};
struct shared_memoryA {
    	int flag;
};

int main(){
	int n;
	printf("Enter the Total Number of Tables at the Hotel: ");
   	scanf("%d", &n);
   	int* shmids = malloc((n+1) * sizeof(int));
   	int shmida;
    	struct shared_memory** shm = malloc((n+1)*sizeof(struct shared_memory*));
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
    	for (int i = 1; i <= n; i++) {
		key_t key = ftok("waiterHM.txt", i);

		if (key == -1) {
		    perror("ftok failed");
		    exit(EXIT_FAILURE);
		}
		shmids[i] = shmget(key, sizeof(struct shared_memory), 0644 | IPC_CREAT);
		if (shmids[i] == -1) {
		    perror("shmget failed");
		    exit(EXIT_FAILURE); 
		}
		shm[i] = (struct shared_memory*) shmat(shmids[i], NULL, 0);
		if (shm[i] == (void*)-1) {
		    perror("shmat failed");
		    exit(EXIT_FAILURE); 
		}
	}
	int total=0;
	int end=0;
	FILE *file;
	char fname[] = "earnings.txt";
   	file = fopen(fname, "w");
    	if (file == NULL) {
		perror("Error opening file");
		return EXIT_FAILURE;
    	}
    	fprintf(file,"-----------------HOTEL EARNINGS---------------\n");
	while(1){
		for(int i=1;i<=n;i++){
			
			if(shm[i]->flag == 1){
				shm[i]->flag = 0;
				
				printf(	"Earnings from table %d : %d\n",i,shm[i]->sum);	
				total = total + shm[i]->sum;
				fprintf(file, "Earnings from table %d : %d\n",i,shm[i]->sum);
				
			}
			if(shm[i]->end == 1){
				end++;
				shm[i]->end =0;
				break;
			}	  
		}
		if(end == n){
			break;
		}
		usleep(2000);
	}
	for (int i = 1; i <= n; i++) {
        	shmdt(shm[i]);
        	shmctl(shmids[i], IPC_RMID, NULL);
    	}

    	free(shmids);
    	free(shm);
	while(shma->flag != 1){
		sleep(1);
	}
	float wage = 0.4*(float)total;
	float profit = 0.6*(float)total;
	printf("Total Earnings of Hotel: %d INR\n",total);
	printf("Total Wages of Waiters: %.2f INR\n",wage);
	printf("Total Profit: %.2f INR\n",profit);
	fprintf(file, "Total Earnings of Hotel: %d INR\n",total);
	fprintf(file, "Total Wages of Waiters: %.2f INR\n",wage);
	fprintf(file, "Total Profit: %.2f INR\n",profit);
	fclose(file);
	shmdt(shma);
        shmctl(shmida, IPC_RMID, NULL);
        printf("Thank you for visiting hotel");
        return 0;
}
