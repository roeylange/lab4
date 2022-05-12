#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUF_SIZE 8192

typedef struct linux_dirent {
    int inode;
    int offset;
    short len;
    char buf[0];
}linux_dirent;

char *types[]= {"0","DT_FIFO","DT_CHR","3","DT_DIR","5","DT_BLK","7","DT_REG","9",
                "DT_LNK","11","DT_SOCK","13","DT_WHT"};

void debugger(int ID, int return_code);
void checkIfError(int num);
extern int system_call();

int main (int argc , char* argv[], char* envp[])
{
    int check;   /*checker for system calls*/
    int bytesToRead;    /*holds the bytes of the files names*/
    int bytePosition;    /*position of the current byte*/
    int d_type;      /*holds the file type*/
    int i;     /*for 'for' loop*/
    int error;    /*checker for system calls*/
    int debFlag = 0;    /*flag for debug*/
    int prefixFlag = 0;    /*flag for prefix*/
    char buf[BUF_SIZE];
    linux_dirent *dirent;   /*pointer to the start of the directory*/
    char *prefixVal;     /*string of prefix*/
    for (i=1; i<argc; i++){       /*for debug mode*/
        if(strcmp(argv[i], "-D")==0) debFlag=1;
    }
    for (i=1; i<argc; i++){       /*for prefix mode*/
        if (strncmp(argv[i], "-p", 2)==0) {
            prefixVal = argv[i]+2;
            prefixFlag = 1;
        }
    }
    check = system_call(SYS_OPEN, ".", 0, 0);   /*open the directory*/
    checkIfError(check);
    if (debFlag) debugger(SYS_OPEN, check);
    bytesToRead = system_call(SYS_GETDENTS, check, buf, BUF_SIZE);   /*using getdent instead of read*/
    checkIfError(bytesToRead);
    if (debFlag) debugger(SYS_GETDENTS, bytesToRead);
    /*loop all over the files*/
    for (bytePosition = 0; bytePosition < bytesToRead;) {
        /*get the next file*/
        dirent = (linux_dirent *) (buf + bytePosition);
        /*checks if we are not in -p mode, otherwise we compare the prefix*/
        if (prefixFlag == 0 || strncmp(dirent->buf, prefixVal, strlen(prefixVal)) == 0) {
            error = system_call(SYS_WRITE, STDOUT, "Name: ", 6);
            checkIfError(error);
            if (debFlag) debugger(SYS_WRITE, error);
            error = system_call(SYS_WRITE, STDOUT, dirent->buf, strlen(dirent->buf));
            checkIfError(error);
            if (debFlag) debugger(SYS_WRITE, error);
            error = system_call(SYS_WRITE,STDOUT,"\n",1);
            checkIfError(error);
            if (debFlag) debugger(SYS_WRITE, error);
        }
        if (prefixFlag && strncmp(dirent->buf, prefixVal, strlen(prefixVal)) == 0) {
            d_type = (int)(*(buf + bytePosition + (dirent->len - 1)));
            error = system_call(SYS_WRITE,STDOUT," Type: ",7);
            checkIfError(error);
            if (debFlag) debugger(SYS_WRITE, error);
            error = system_call(SYS_WRITE, STDOUT, types[d_type], strlen(types[d_type]));
            checkIfError(error);
            if (debFlag) debugger(SYS_WRITE, error);
            error = system_call(SYS_WRITE,STDOUT,"\n",1);
            checkIfError(error);
            if (debFlag) debugger(SYS_WRITE, error);

        }
        bytePosition += dirent->len;
    }
    return 0;
}


void debugger(int id, int return_code) {
    system_call(SYS_WRITE,STDERR,"id: ", 4);
    system_call(SYS_WRITE,STDERR,itoa(id), 1);
    system_call(SYS_WRITE,STDERR,"\n", 1);
    system_call(SYS_WRITE,STDERR,"return code: ", 13);
    system_call(SYS_WRITE,STDERR,itoa(return_code), 1);
    system_call(SYS_WRITE,STDERR,"\n", 1);
}


void checkIfError(int num) {
    if (num < 0) system_call(SYS_EXIT, 0x55, 1, 1);
}