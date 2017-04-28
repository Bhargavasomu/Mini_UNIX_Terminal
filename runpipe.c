#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<pwd.h>
#include<signal.h>
#include<wait.h>
#include<fcntl.h>
#include<signal.h>
#include<setjmp.h>
#include "global.h"
void runpipe()
{
	par=1;
	int fd[2],pid;
	int fd_in=0;
	int i=0;
	//printf("loop start\n");
	while(cmnd[i][0]!=NULL)
	{
		pipe(fd);
		pid=fork();
		if(pid==0)
		{
			int in_flag,out_flag,app_flag,j;
			char inp_file[100],out_file[100],app_file[100];
			for(j=0;cmnd[i][j]!=NULL;j++)		//to find where the '<' or '>' is present
			{
				in_flag=0;
				out_flag=0;
				app_flag=0;
				if(strcmp(cmnd[i][j],"<")==0)
				{
					cmnd[i][j]=NULL;
					strcpy(inp_file,cmnd[i][j+1]);
					in_flag=1;
					j++;
				}
				if(strcmp(cmnd[i][j],">")==0)
				{
					cmnd[i][j]=NULL;
					strcpy(out_file,cmnd[i][j+1]);
					out_flag=1;
					j++;
				}
				if(strcmp(cmnd[i][j],">>")==0)
				{
					cmnd[i][j]=NULL;
					strcpy(app_file,cmnd[i][j+1]);
					app_flag=1;
					j++;
				}
			}
			if(in_flag)		// '<' is detected
			{
				int fd0;
				fd0=open(inp_file,O_RDONLY,0);	
				dup2(fd0,0);	//Standard input is 0
				close(fd0);
			}
			if(out_flag)	// '>' is detected
			{
				int fd1;
				fd1=creat(out_file,0644);
				dup2(fd1,1);	//standard output is 1
				close(fd1);
			}
			if(app_flag)	// '>>' is detected
			{
				int fd2;
				if(access(app_file,F_OK)==-1)	// If file is not present create it
				{
					fd2=creat(app_file,0644);
					close(fd2);
				}
				fd2=open(app_file,O_RDWR | O_APPEND,0);	//open in append mode and Read/Write mode
				dup2(fd2,1);
				close(fd2);
			}
			if(!in_flag)	
				dup2(fd_in,0);	//input here is fd_in only if no other input file is mentioned in command
			if((cmnd[i+1][0]!=NULL)&&(!out_flag)&&(!app_flag))
				dup2(fd[1],1);
			close(fd[0]);
			//printf("fine\n");
			execvp(cmnd[i][0],cmnd[i]);
		}
		else
		{
			wait(NULL);
			close(fd[1]);
			fd_in=fd[0];
			i++;
		}
	}
	//printf("loop ok\n");
}
