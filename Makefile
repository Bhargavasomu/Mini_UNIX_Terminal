shell: shell.o parse.o execute.o cd.o pwd.o echo.o pinfo.o runpipe.o
		gcc shell.o parse.o execute.o cd.o pwd.o echo.o pinfo.o runpipe.o -o shell -lm
shell.o: global.h execute.h parse.h pwd.h cd.h echo.h pinfo.h runpipe.h shell.c
		gcc shell.c -c -o shell.o -lm
parse.o: parse.c
		gcc parse.c -c -o parse.o -lm
execute.o: global.h execute.c
		gcc execute.c  -c -o execute.o -lm 
cd.o: global.h cd.c
		gcc cd.c -c -o cd.o -lm
pwd.o: global.h pwd.c
		gcc pwd.c -c -o pwd.o -lm
echo.o: global.h echo.c
		gcc echo.c -c -o echo.o -lm
pinfo.o: global.h pinfo.c
		gcc pinfo.c -c -o pinfo.o -lm
runpipe.o: global.h runpipe.c
		gcc runpipe.c -c -o runpipe.o -lm 
