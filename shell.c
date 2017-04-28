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
#include "execute.h"
#include "parse.h"
#include "pwd.h"
#include "cd.h"
#include "echo.h"
#include "pinfo.h"
#include "runpipe.h"
sigjmp_buf mark;
void controlc()
{
	loop_flag=1;
	glob=0;
	num=0;
	reset=0;
	cntrlc=1;
	printf("\n");
	return;
}
void controlz()
{
	if(glob==0)
	{
		kill(pro,SIGSTOP);
		process *new=(process *)malloc(sizeof(process));
		new->status=1;
		new->pid=pro;
		new->next=NULL;
		int i=0;
		while(argv[i]!=NULL)
		{
			strcpy(new->argv[i],argv[i]);
			i++;
		}
		if(head==NULL)
			head=new;
		else
		{
			process *t=head;
			while((t->next)!=NULL)
				t=t->next;
			t->next=new;
		}
		printf("\n");
		siglongjmp(mark,-1);
	}
}
int main()	
{
	glob=0;				//global initializations
	num=0;
	par=1;
	head=NULL;
	cntrlc=0;
	loop_flag=0;

	struct passwd *pw;	//structure in which all values are present
	int uid,c;
	uid=getuid();
	pw=getpwuid(uid);	

	char host[1024];	//This array has the host name
	gethostname(host,1024);	//returns name in array

	int len;
	char cwd[1024];	//This array contains the cwd string
	len=0;	//reference length for home directory in which program is executed
	if(getcwd(cwd,sizeof(cwd))!=NULL)	//returns home dir name in array cwd
		len=strlen(cwd);				//Finding lenth of home dir to compare as ~
	sigset(SIGINT,controlc);
	//Main shell loop starts from here
	while(1)
	{
		signal(SIGTSTP,&controlz);
		fg_call=0;
		int reset=0;
		char cwd1[1024],cwd3[1024];	//cwd1 for replacing in it with ~
		int i,swit=0;			//To check if pwd has home directory as subpath
		if(getcwd(cwd1,sizeof(cwd1))!=NULL)	//checking string for home path
		{
			for(i=0;i<len;i++)
			{
				if(cwd[i]!=cwd1[i])
				{
					swit=1;
					break;
				}
			}
		}
		if(swit==0)		//changing accordingly if subpart is home path
		{
			cwd1[0]='~';
			i=len;
			while(cwd1[i]!='\0')
			{
				cwd1[i-len+1]=cwd1[i];
				i++;
			}
			cwd1[i-len+1]='\0';
		}
		int returncode=sigsetjmp(mark,1);
		printf("<%s@%s:%s>",pw->pw_name,host,cwd1);	//printing prompt
		char comm[1024];		//stores command entered
		char *token;			//This contains the divided commands seperated by ;
		int inp_flag;	//This flag is for determining whether input is taken or not
		int pi;			//used to check whether pipelining is present or not
		char *line=fgets(comm,1024,stdin);
		if((line==NULL)&&(cntrlc==0))
		{
			printf("\n");
			break;
		}
		if(line)
			inp_flag=1;
		else
			inp_flag=0;
		token=strtok(comm,";");	
		int end=0;				//To break out of loops
		int j;
		while(token !=NULL)
		{
			if((num==1)&&(glob==1))
			{
				num=0;
				glob=0;
				reset=1;
			}
			if(cntrlc==1)
			{
				num=0;
				glob=0;
				reset=0;
				loop_flag=0;
				cntrlc=0;
			}
			if(num==0)
			{
				glob=0;
				for(i=0;i<20;i++)
					argv[i]=NULL;
			}
			for(i=0;i<50;i++)
				for(j=0;j<50;j++)
					cmnd[i][j]=NULL;
			if((num==0)&&((reset==0)||(comm[0]!='\0')))
			{
				int test=0;
				while(token[test]!='\0')
					test++;
				if(token[test-2]=='|')
				{
					fprintf(stderr,"ERROR: Give proper command after pipe\n");
					break;
				}
				pi=0;
				for(char *p=strtok(token,"|");p;p=strtok(NULL,"|"))
				{
					parse(p,argv);
					int j=0;
					while(argv[j]!=NULL)
					{
						cmnd[pi][j]=argv[j];
						j++;
					}
					pi++;
				}
			}
			if(pi>1)			//for piping
				runpipe();
			else if(argv[0]!=NULL)			//if piping is not there
			{
				if(loop_flag==1)
					loop_flag=0;
				else
				{
					if((strcmp(argv[0],"quit")==0)||(strcmp(argv[0],"exit")==0))	//exit
					{
						end=1;
						break;
					}
					if(strcmp(argv[0],"cd")==0)		//for cd command
						cd(cwd);
					else if(strcmp(argv[0],"pwd")==0) //for pwd command
						pwd();
					else if(strcmp(argv[0],"echo")==0)	//for echo command
						echo();
					else if(strcmp(argv[0],"pinfo")==0)	//For pinfo command
					{
						pinfo();
						break;
					}
					else if(strcmp(argv[0],"jobs")==0)
					{
						process *temp=head;
						int count=1;
						while(temp!=NULL)
						{
							if(temp->status==1)
							{
								printf("[%d] ",count);
								int arg_count=0;
								while(temp->argv[arg_count][0]!='\0')
								{
									printf("%s ",temp->argv[arg_count]);
									arg_count++;
								}
								printf("[%d]\n",temp->pid);
								count++;
							}
							temp=temp->next;
						}
					}
					else if(strcmp(argv[0],"killallbg")==0)
					{
						process *temp1=head;
						while(temp1!=NULL)
						{
							temp1->status=0;
							kill(temp1->pid,9);
							temp1=temp1->next;
						}
					}
					else if((strcmp(argv[0],"sendsig")==0)&&(inp_flag==1))
					{
						process *temp2=head;
						int l=strlen(argv[1])-1;
						int ans=0;
						int kk=0;
						while(kk<=l)
						{
							ans+=((ans*10)+argv[1][kk]-'0');
							kk++;
						}
						int move=ans;	// move is the position of bg process
						int kl=0;
						while(temp2!=NULL)
						{
							if(temp2->status==1)
								kl++;
							if(kl==move)
								break;
							temp2=temp2->next;
						}
						if(kl<move)
							fprintf(stderr,"ERROR: That many jobs don't exist\n");
						else
						{
							int l1=strlen(argv[2])-1;
							int ans1=0;
							int kk1=0;
							while(kk1<=l1)
							{
								ans1+=((ans1*10)+argv[2][kk1]-'0');
								kk1++;
							}
							if(ans1==9)
								temp2->status=0;
							kill(temp2->pid,ans1);
						}
					}
					else if((strcmp(argv[0],"fg")==0)&&(inp_flag==1))
					{
						fg_call=1;
						int pik=0;			// start of conversion of char to num
						int ans2=0;
						int len2=strlen(argv[1]);
						while(pik<len2)
						{
							ans2+=((ans2*10)+argv[1][pik]-'0');
							pik++;
						}					//ends here

						int cc=0;	//our rapid count
						process *temp3=head;
						while(temp3!=NULL)
						{
							if(temp3->status==1)
								cc++;
							if(cc==ans2)
								break;
							temp3=temp3->next;
						}
						if(cc<ans2)
							fprintf(stderr,"ERROR: That many background process don't exist\n");
						else
						{
							int st=0;
							waitpid(temp3->pid,&st,0);
							temp3->status=0;
							break;
						}
					}
					else			//Implementing Background process and other executable programs	
					{
						int i=1;
						while(argv[i]!=NULL)
							i++;
						if(strcmp(argv[i-1],"&")==0)
						{
							glob=1;						//Force it into Background Loop
							argv[i-1]=NULL;
						}
						if((num==0)&&(argv[0]!=NULL)&&(inp_flag==1))
						{
							execute(argv);
							reset=0;
						}
					}
				}
			}
			token=strtok(NULL,";");		//Move to the next subcommand in delimiter
		}
		if(end==1)
			break;
	}
	return 0;
}
