#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<pwd.h>
#include<signal.h>
#include<wait.h>
#include "global.h"
void pinfo()
{
	if(argv[1]==NULL)
		fprintf(stderr,"ERROR : No PID\n");
	else
	{
		char buf[1024];
		char file[1024]="/proc/";
		char temp[100];
		char exepath[500]="/proc/";
		strcat(argv[1],"/");
		strcpy(temp,argv[1]);
		strcat(temp,"exe");
		strcat(argv[1],"stat");	
		strcat(file,argv[1]);	//file has the whole path to file
		strcat(exepath,temp);	//exepath has the path to exe link
		readlink(exepath,buf,1024);
		FILE *f = fopen(file,"r");
		if(f==NULL)
			fprintf(stderr,"ERROR : Cannot find the Process\n");
		else
		{
			int w1,w2,w3,w4,w5,w6,w7,w8,w9,w10,w11,w12,w13,w14,w15,w16,w17,w18,w19,vsize;
			int pi;
			char execu[1024];
			char state;
			fscanf(f,"%d %s %c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",&pi,execu,&state,&w1,&w2,&w3,&w4,&w5,&w6,&w7,&w8,&w9,&w10,&w11,&w12,&w13,&w14,&w15,&w16,&w17,&w18,&w19,&vsize);
			printf("PID -- %d\n",pi);
			printf("Process Status -- %c\n",state);
			printf("Memory -- %d BYTES\n",vsize);
			printf("Executable Path -- %s\n",buf);
		}
	}
}
