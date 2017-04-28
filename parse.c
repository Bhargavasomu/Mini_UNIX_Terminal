#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<pwd.h>
#include<signal.h>
#include<wait.h>
void parse(char *comm,char **argv)
{
	while((*comm)!='\0')
	{
		while((((*comm)==' ')||((*comm)=='\t')||((*comm)=='\n'))&&((*comm)!='\0'))
		{
			*comm='\0';
			comm++;
		}
		if(((*comm)!='\0')&&((*comm)!='"'))
			*(argv++)=comm;
		if((*comm)=='"')	//For quoted argument in case of echo
		{
			comm++;			
			*(argv++)=comm;	//Argument starting after quotes
			while((*comm)!='"')
				comm++;
			*comm='\0';
			comm++;
		}
		else	//For unquoted arguments in echo and other commands
		{
			while((*comm!='\0')&&(*comm!=' ')&&(*comm!='\t')&&(*comm!='\n'))
				comm++;
		}
	}
	*argv='\0';
}
