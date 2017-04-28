int glob;		//Not initially bg process and glob is flag for backgorund process
int num;		//Flag indicating whether it is again going back into main loop
int par;		//Flag to make sure the parent of background process only takes the childhandler(0 means true,1 means false)
int pro;		//To get the process id of background process
char *argv[21];	//This matrix of char contains strings which are arguments
char *cmnd[50][50];
typedef struct p{
	int pid;
	int status;			//to know whether it is still in bg
	char argv[21][100];
	struct p *next;
}process;
process *head;	//for linked list
int cntrlc;		//flag to know that it went into cntrlc signal handler
int fg_call;
int reset;
int loop_flag;
