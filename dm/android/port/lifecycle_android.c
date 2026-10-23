#include "hle.h"
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <unistd.h>
void hlecount_flush(void);
static void *shutdown_timeout(void *unused) {
    (void)unused; usleep(1500000); _exit(0); return NULL;
}
void dm_host_shutdown(const char *why) {
    static _Atomic int once;
    if(atomic_exchange(&once,1))return;
    LOG("received %s: graceful exit",why);
    pthread_t timer; if(!pthread_create(&timer,NULL,shutdown_timeout,NULL))pthread_detach(timer);
    hlecount_flush(); dm_audio_suspend(1); dm_gfx_suspend(1); _exit(0);
}
static void *term_wait(void *unused) {
    (void)unused; sigset_t set;sigemptyset(&set);sigaddset(&set,SIGTERM);
    int sig;if(!sigwait(&set,&sig))dm_host_shutdown("SIGTERM");return NULL;
}
void dm_term_install(void) {
    sigset_t set;sigemptyset(&set);sigaddset(&set,SIGTERM);pthread_sigmask(SIG_BLOCK,&set,NULL);
    pthread_t thread;if(!pthread_create(&thread,NULL,term_wait,NULL))pthread_detach(thread);
}
