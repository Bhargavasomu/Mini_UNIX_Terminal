#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<pwd.h>
#include<signal.h>
#include<wait.h>
#include "global.h"
void cd(char *cwd)
{
	if((argv[1]==NULL)||(strcmp(argv[1],"~")==0))	//To change to home directory on "cd" or "cd ~"
	{
		if(chdir(cwd)==-1)
			fprintf(stderr,"ERROR : Directory Not Found\n");
	}
	else
	{
		if(argv[1][0]=='/')				//If whole path is entered (or) path starting from root
		{
			if(chdir(argv[1])==-1)
				fprintf(stderr,"ERROR : Directory Not Found\n");
		}
		else
		{
			char cwd2[1024];
			getcwd(cwd2,sizeof(cwd2));	// "cd directory" format
			strcat(cwd2,"/");
			strcat(cwd2,argv[1]);
			if(chdir(cwd2)==-1)
				fprintf(stderr,"ERROR : Directory Not Found\n");
		}
	}
}
