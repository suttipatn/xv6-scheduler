#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int fd[2];
    pipe(fd);
    char buf[512];
    int pid=fork();
    if(pid==0){//child
        // close(fd[1]);
        //I think reading end will block the process until some data being available in the pipe to be read
        read(fd[0],buf,512);
        close(fd[0]);
        printf("%d: received ping\n",getpid());
        write(fd[1],buf,2);
        close(fd[1]);
        exit(0);
    }
    else if(pid>0){
        //parent
        // close(fd[0]);
        write(fd[1],"a\n",2);
        close(fd[1]);
        wait(0);
        read(fd[0],buf,512);
        printf("%d: received pong\n",getpid());
        close(fd[0]);
        exit(0);
    }
    exit(0);
}