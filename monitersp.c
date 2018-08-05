/*
 * Copyright (C) Chesley Diao
 */

#include <unistd.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>


#define PROCESS_NAME "nginx"
#define PROCESS_BIN "/usr/local/nginx/sbin/nginx"
#define PROC_OK	1

int main(int argc,char** argv){
 	int process_lift = 0;
 	DIR *dp;
 	DIR *dp1;
 	char fn[512];
 	char fn1[512];
 	char str[512];
 	struct dirent *dr;
 	struct dirent *dr1;
 	pid_t pid;
 	pid = fork();
 	switch(pid){
 		case -1:
			perror("fork()");
			exit(1);
 		case 0:
			break;
 		default:
			exit(0);
 	}

 	setsid();
 	chdir("/");
 	close(STDIN_FILENO);
 	close(STDOUT_FILENO);
 	close(STDERR_FILENO);
 	signal(SIGHUP,SIG_IGN);
 	signal(SIGINT,SIG_IGN);
 	signal(SIGQUIT,SIG_IGN);
 	signal(SIGTERM,SIG_IGN);
 	signal(SIGTSTP,SIG_IGN);

 	dp = opendir("/proc");
 	if(!dp)
 		exit(1);
 	while(1){
 		while(dr = readdir(dp)){
 			if((dr->d_type != DT_DIR)||(strncmp(dr->d_name,".",1) == 0))
 				continue;
 			snprintf(fn,512,"/proc/%s",dr->d_name);
 			dp1 = opendir(fn);
 			if(!dp1)
 				continue;
 			while(dr1 = readdir(dp1)){
 				if(strncmp(dr1->d_name,"exe",3) != 0)
 					continue;
 				snprintf(fn1,512,"%s/%s",fn,dr1->d_name);
 				memset(str,0,512);
 				if(readlink(fn1,str,512) <0 )
 					break;
 				str[511] = '\0';
 				if(strstr(str,PROCESS_NAME))
 					process_lift = PROC_OK;
 				break;
 			}
 			closedir(dp1);
			/*process_lift 1:process is running
			*             0:process is dead
			*/
			if(process_lift)
				break;
                 }
                 if(!process_lift)
                 	system(PROCESS_BIN);
                 process_lift = 0;
                 sleep(10);
                 rewinddir(dp);
	}
        closedir(dp);
        return(0);
}
