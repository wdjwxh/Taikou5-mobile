// Standalone POSIX/Android fixture; argv[1] must be a new private directory.
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "../crash_note.h"
#include "../../android/port/runtime_log.h"
#include <fcntl.h>

static int crash_fd;
static void signal_note(int signal){
    dm_crash_note_fd(crash_fd,signal,-6,0x1234,0x5678,0x9abc,0xdef0);
    _exit(128+signal);
}
int main(int argc,char **argv){
    assert(argc==2 && mkdir(argv[1],0700)==0);char path[4096],buffer[512];
    for(int session=0;session<5;++session){
        assert(dm_rotate_runtime_log(argv[1],"log.txt")==0);
        snprintf(path,sizeof path,"%s/log.txt",argv[1]);FILE *f=fopen(path,"w");assert(f);
        fprintf(f,"session%d\n",session);assert(fclose(f)==0);
    }
    for(int previous=0;previous<4;++previous){
        if(previous)snprintf(path,sizeof path,"%s/log.txt.%d",argv[1],previous);
        else snprintf(path,sizeof path,"%s/log.txt",argv[1]);
        FILE *f=fopen(path,"r");assert(f && fgets(buffer,sizeof buffer,f));int session=-1;
        assert(sscanf(buffer,"session%d",&session)==1 && session==4-previous);fclose(f);
    }
    snprintf(path,sizeof path,"%s/crash.txt",argv[1]);
    crash_fd=open(path,O_WRONLY|O_CREAT|O_APPEND,0600);assert(crash_fd>=0);
    pid_t child=fork();assert(child>=0);
    if(!child){signal(SIGABRT,signal_note);raise(SIGABRT);_exit(1);}
    int status;assert(waitpid(child,&status,0)==child && WIFEXITED(status) && WEXITSTATUS(status)==134);
    close(crash_fd);FILE *f=fopen(path,"r");assert(f && fgets(buffer,sizeof buffer,f));fclose(f);
    assert(strstr(buffer,"signal=0x6") && strstr(buffer,"code=0xfffffffffffffffa") && strstr(buffer,"pc=0x5678"));
    assert(dm_rotate_runtime_log(argv[1],"crash.txt")==0);
    snprintf(path,sizeof path,"%s/crash.txt.1",argv[1]);assert(access(path,R_OK)==0);
    puts("runtime log: rotation, retained sessions and SIGABRT emergency record passed");
}
