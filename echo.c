#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<pwd.h>
#include<signal.h>
#include<wait.h>
#include "global.h"
void echo()
{
	if(argv[1]==NULL)		
		fprintf(stderr,"ERROR : No Argument Specified\n");
	else
	{
		int i=1;					//since argv[0]="echo"
		while(argv[i]!=NULL)		//print all arguments with space
		{
			printf("%s ",argv[i]);
			i++;
		}
		printf("\n");
	}
}
