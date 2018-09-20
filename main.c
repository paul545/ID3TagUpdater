#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
//#include "thread.h"
#include "GetWebpage1.h"
#include "tagReader.h"


struct arg {
    int id;
    char* name;
    char* country;
};


void getTrackNames(char* dirPath);
void *child_thread(void * targ);

char* bandNames[1000];
int threadCount = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char** argv){
		
	
	if(strcmp("read", argv[1]) != 0){
		
		FILE *fp = fopen(argv[1], "r");
		pthread_t thread_handle[100];
		struct arg targ[100];
	
		int k = 0;
		int i = 0;
		if (fp != NULL){
			char line[128];
			while (fgets(line, sizeof line, fp) != NULL){
				bandNames[i] = strdup(line);
		        i++;        	
		    }
		}
	
	
		char *name;
	
		int j;
		for (j = 0; j < i-1; j++){
			name = bandNames[j];
			targ[j].id = j;
			targ[j].name = strtok(name, "\n");
			threadCount++;
			pthread_create(&thread_handle[j], NULL, child_thread, &targ[j]);
		} 
		
		while (threadCount > 0){
			sleep(1);
		}
		    
	   
		for (k=0; k < i; k++){
			printf("Thread %i joined\n", k);
			pthread_join(thread_handle[k],NULL);
		}
		
		fclose(fp);
		return 0;
	}
	else {
		getTrackNames(argv[2]);      //./Music/
	}

}


void getTrackNames(char* dirPath){
	
	char trackPath[1000];
	strcat(trackPath, dirPath);
	struct dirent *de;  // Pointer for directory entry
 
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(dirPath);
 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory\n" );
        exit(-1);
    }
 
    while ((de = readdir(dr)) != NULL){
    		trackPath[0] = '\0';
            printf("%s\n", de->d_name);
            strcat(trackPath, dirPath);
            strcat(trackPath, de->d_name);
            runTagReader(trackPath);
    }          
}

 


// function to run one thread

void *child_thread(void *targ_in){
    struct arg* targ =  (struct arg*) targ_in;
    printf("Thread %i runnning\n", targ->id);
    
    pthread_mutex_lock(&mutex1);
    printf("Band name is %s\n", targ->name);
    targ->country = getWebpage(targ->name);
    threadCount--;
    pthread_mutex_unlock(&mutex1);
    
    pthread_exit(0);
}











