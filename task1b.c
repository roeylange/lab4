#include "util.h"

#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19

#define STDIN 0
#define STDOUT 1
#define STDERR 2


extern int system_call();
int isLexChar(char letter);
void debugger(int id, int return_code);
void debuggerIO(char* type, char* toPrint);

int main (int argc , char* argv[], char* envp[])
{
	int deFlag = 0; char letter[1]; int counter = 0;
	int countHelper = 0; int check = 0; int i = 0;
	int input = STDIN; int output = STDOUT;
	char * inPath = "stdin"; char * outPath = "stdout";
	
	for (i=1; i<argc; i++){
		if(strcmp(argv[i], "-D")==0) deFlag=1;
	}
	for (i=1; i<argc; i++){
		if (strncmp(argv[i], "-i", 2)==0) {
			input = system_call(SYS_OPEN, argv[i]+2, 2, 0777);
			inPath = argv[i]+2;
			if (deFlag) debugger(SYS_OPEN, input);
		}
		else if(strncmp(argv[i], "-o", 2)==0) {
			output = system_call(SYS_OPEN, argv[i]+2, 2 | 64, 0777);
			outPath = argv[i]+2;
			if (deFlag) debugger(SYS_OPEN, input);
		}
	}
	if (deFlag) debuggerIO("input", inPath);
	if (deFlag) debuggerIO("output", outPath);
	while (*letter != 0x0a) {
		check = system_call(SYS_READ, input, letter, 1);
		if (deFlag) debugger(SYS_READ, check);
		if (countHelper == 0 && isLexChar(*letter) == 1) {
			counter++;
			countHelper = 1;
		}
		else if ( isLexChar(*letter) == 0 ) countHelper = 0;
	}
	check = system_call(SYS_WRITE,output,itoa(counter), counter);
	if (deFlag) debugger(SYS_WRITE, check);
	check = system_call(SYS_WRITE,output,"\n", 1);
	if (deFlag) debugger(SYS_WRITE, check);
  return 0;
}

int isLexChar(char letter) { 
	if( ('a' <= letter && letter <= 'z') || ('A' <= letter && letter <= 'Z') )
        return 1;
    return 0;
}

void debuggerIO(char* type, char* toPrint) {
	system_call(SYS_WRITE, STDERR, type, strlen(type));
	system_call(SYS_WRITE,STDERR,": ", 2);
	system_call(SYS_WRITE,STDERR,toPrint, strlen(toPrint));
	system_call(SYS_WRITE,STDERR,"\n", 1);
}

void debugger(int id, int return_code) {
	
	system_call(SYS_WRITE,STDERR,"ID: ", 4);
	system_call(SYS_WRITE,STDERR,itoa(id), 1);
	system_call(SYS_WRITE,STDERR,"\n", 1);
	system_call(SYS_WRITE,STDERR,"return code: ", 13);
	system_call(SYS_WRITE,STDERR,itoa(return_code), 1);
	system_call(SYS_WRITE,STDERR,"\n", 1);
}
