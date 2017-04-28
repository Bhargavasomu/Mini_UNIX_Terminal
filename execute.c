#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<pwd.h>
#include<signal.h>
#include<wait.h>
#include<math.h>
#include "global.h"
/*void deletenode(process *node)
{
	if(node==head)
	{
		if(head->next=NULL)
			head=NULL;
		else
		{
			process *n=head->next;
			head=head->next;
			free(n);
		}
	}
	else
	{
		process *prev=head;
		while((prev->next!=NULL)&&(prev->next!=node))
			prev=prev->next;
		prev->next=prev->next->next;
		free(node);
	}
}*/
void childhandler()
{
	if(par==0)
	{
		//printf("ncdjsk\n");
		int status;
		if(fg_call!=1)
			waitpid(-1,&status,0);
		///printf("argv[0] is %s",argv[0]);
		process *check=(process *)malloc(sizeof(process));
		check=head;
		//process *check=head;
		/*if(check==NULL)
		{
			printf("in\n");
			fprintf(stderr,"\n%s with PID %d Has Exited Normally\n",argv[0],pro);
		}*/
		while(check!=NULL)
		{
			//printf("in\n");
			char k[1024]="/proc/";
			char samp[10];	//to store the number as char
			int num=check->pid;
			int len=log10(num);	// to find the length of number
			int kk;
			samp[len+1]='\0';
			while(num!=0)		//converting number to string numerics
			{
				kk=num%10;
				kk+=48;
				samp[len]=kk;
				len--;
				num=num/10;
			}
			strcat(k,samp);
			strcat(k,"/");
			//printf("%s\n",k);
			if(access(k,F_OK)!=0)	//checking if process still exists
			{
				//printf("innk\n");
				fprintf(stderr,"\n%s with PID %d Has Exited Normally\n",check->argv[0],check->pid);
				check->status=0;		//not in background
				//deletenode(check);
				/*process *temp=head;
				if(temp!=NULL)
				{
					while((temp->next->pid)!=(check->pid))
						temp=temp->next;
					process *pp=temp->next;
					temp->next=temp->next->next;
					free(pp);
				}*/
			}
			check=check->next;
		}
		//printf("over\n");
	}
	/*process *temp5=head;
	while(temp5!=NULL)
	{
		temp5->status=0;
		temp5=temp5->next;
	}*/
	cntrlc=1;
	return;
}
void execute(char **argv)
{
	int status,pid,i;
	pid=fork();
	if(pid<0)
	{
		fprintf(stderr,"Error in Forking child process\n");
		return;
	}
	else if(pid==0)		//child process is executing
	{
		par=0;
		int i,in_flag,out_flag,app_flag;
		char inp_file[1024],out_file[1024],app_file[1024];
		in_flag=0;
		out_flag=0;
		app_flag=0;
		i=0;
		for(i=0;argv[i]!=NULL;i++)		//to find where the '<' or '>' is present
		{
			if(strcmp(argv[i],"<")==0)
			{
				argv[i]=NULL;
				if(argv[i+1]==NULL)
				{
					fprintf(stderr,"ERROR: Give the file\n");
					return;
				}
				strcpy(inp_file,argv[i+1]);
				in_flag=1;
				i++;
			}
			if(strcmp(argv[i],">")==0)
			{
				argv[i]=NULL;
				if(argv[i+1]==NULL)
				{
					fprintf(stderr,"ERROR: Give the file\n");
					return;
				}
				strcpy(out_file,argv[i+1]);
				out_flag=1;
				i++;
			}
			if(strcmp(argv[i],">>")==0)
			{
				argv[i]=NULL;
				if(argv[i+1]==NULL)
				{
					fprintf(stderr,"ERROR: Give the file\n");
					return;
				}
				strcpy(app_file,argv[i+1]);
				app_flag=1;
				i++;
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
		if(execvp(*argv,argv)<0)		//Executing Commands
		{
			fprintf(stderr,"ERROR : Command Not Found\n");
			kill(getpid(),SIGTERM);		//kill child process as it is no longer needed
			return;
		}
	}
	else				//parent process is executing
	{
		pro=pid;
		if(glob==0)	//Parent waits for child only if last argument is not "&"
		{
			par=1;
			waitpid(pid,&status,0);
		}
		else
		{

			process *new_node=(process *)malloc(sizeof(process));		//linked list formation starts here
			new_node->pid=pro;
			new_node->status=1;		//present in bg
			new_node->next=NULL;
			i=0;
			while(argv[i]!=NULL)
			{
				strcpy(new_node->argv[i],argv[i]);
				//new_node->argv[i]=argv[i];
				//printf("Arg to enter is %s\n",argv[i]);
				i++;
			}
			//printf("%p\n",head);
			if(head==NULL)
				head=new_node;
			else
			{
				//printf("pk\n");
				//process *temp=(process *)malloc(sizeof(process));
				process *temp=head;
				while(temp->next!=NULL)
					temp=temp->next;
				temp->next=new_node;
			}															//linked list closes here
		//	printf("%s\n",head->argv[0]);

			par=0;
			struct sigaction sigchld_action;
			memset(&sigchld_action,0,sizeof(sigchld_action));
			sigchld_action.sa_handler=&childhandler;
			sigaction(SIGCHLD,&sigchld_action,NULL);

			int kk;
			for(kk=0;kk<21;kk++)
				argv[kk]=NULL;
			//printf("Through here\n");
			//num=1;
		}
		return;
	}
}
