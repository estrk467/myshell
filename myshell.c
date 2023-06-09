#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

#define BUFFER_SIZE 50

static char buffer[BUFFER_SIZE];
pid_t tempPid;



/* the signal handler function */
void handle_SIGQUIT() {
      write(STDOUT_FILENO,buffer,strlen(buffer));

      
}
 
/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a
 * null-terminated string.
 */

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
   
    ct = 0;

    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE); 

    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
        exit(-1);           /* terminate with error code of -1 */
    }

    /* examine every character in the inputBuffer */
    for (i=0;i<length;i++) {
        switch (inputBuffer[i]){
          case ' ':
          case '\t' :               /* argument separators */
            if(start != -1){
                    args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
          case '\n':                 /* should be the final char examined */
            if (start != -1){
                    args[ct] = &inputBuffer[start];    
                ct++;
            }
                inputBuffer[i] = '\0';
                args[ct] = NULL; /* no more arguments to this command */
            break;
          default :             /* some other character */
            if (start == -1)
                start = i;
            if (inputBuffer[i] == '&'){
                *background  = 1;
                start = -1;
                inputBuffer[i] = '\0';
            }
          }
     }   
     args[ct] = NULL; /* just in case the input line was > 80 */
}


int main(void)
{
char inputBuffer[MAX_LINE];      /* buffer to hold the command entered */
    int background;              /* equals 1 if a command is followed by '&' */
    char *args[(MAX_LINE/2)+1];  /* command line (of 80) has max of 40 arguments */
    int i = 1; 			 /*variable to keep track of command prompt number*/
  



    
   printf("Welcome to kveshell. My pid is %d.\n", getpid());  /*welcome message that prints the shell pid*/


 /* set up the signal handler */
      struct sigaction handler;
      handler.sa_handler = handle_SIGQUIT;
      handler.sa_flags = SA_RESTART;
      sigaction(SIGQUIT, &handler, NULL);

      strcpy(buffer,"Caught <ctrl><\>\n");
 
    while (1){            /* Program terminates normally inside setup */

       
       background = 0; 
       setbuf(stdout, NULL); 
       printf("kveshell[%d]->", i);  

       setup(inputBuffer,args,&background);       /* get next command */

      

	if(strcmp(args[0], "stop") == 0){
		tempPid = args[1];
		kill(tempPid, SIGSTOP);
	
	}
	else if(strcmp(args[0], "bg") == 0){
		tempPid = args[1];
		kill(tempPid, SIGCONT);
	}
	else if(strcmp(args[0], "fg")== 0){
		tempPid = args[1];
		kill(tempPid, SIGCONT);
		if(waitpid(0) <= 0){
			printf("Child Complete\n");
		}
	}
	else if(strcmp(args[0], "kill")==0){
		tempPid = args[1];
		kill(tempPid, SIGKILL);
	}
	else if(strcmp(args[0], "exit")==0){
		printf("kveshell exiting...\n");
		exit(0);
	}
	

	else{ /*fork a new child process*/

	
	pid_t pid;
	pid = fork();  			/*fork a child process*/
	int childPID = getpid();



	if (pid <0){  			 /*fork has failed*/

		printf("Fork Failed");
		return 1; 
	
	}else if(pid == 0) { 		/*child process*/
		char* flag;
		if(background == 1){
			flag = "TRUE";  /*process will run in the background, so no waiting from parent*/
		}else if(background ==0){
			flag = "FALSE"; /*parent process with wait for the child process to finish*/
		}
		
		printf("[Child pid = %d, background = %s]\n", childPID, flag); /*child process info*/



		execvp(args[0], args); /*executes command*/
		putchar('\n');
		exit(0);

	}else{   			/*parent process*/
		
		if(background == 0){    /*waits for the child process to finish*/
			waitpid(0);
			
			
		}
	
	}

    }/*end of else for forking a child process*/

	


	i++; /*increment the number for the command prompt*/


	
    }/*end of while loop*/
}/*end of main function*/
