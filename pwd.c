#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<pwd.h>
#include<signal.h>
#include<wait.h>
void pwd()
{
	char cwd3[1024];
	getcwd(cwd3,sizeof(cwd3));
	printf("%s\n",cwd3);
}
