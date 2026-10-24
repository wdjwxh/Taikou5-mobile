// Android bring-up shell; original guest runtime and Vulkan replay run on one host thread.
#include "dm.h"
#include "front.h"
#include "dm_input.h"
#include "../vulkan_present.h"
#include <jni.h>
#include <android/native_window_jni.h>
#include <stdatomic.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <android/log.h>
#include "runtime_log.h"
static _Atomic(void *) window;
static _Atomic int refresh_mhz;
int dm_main(int argc,char **argv);
void front_run(void (*guest)(void)){guest();}
void *front_metal_layer(void){return atomic_load(&window);}
int front_screen_max_fps(void){return atomic_load(&refresh_mhz)/1000;}
double front_display_refresh(char *name,size_t n){if(name&&n)*name=0;return atomic_load(&refresh_mhz)/1000.0;}
int front_text_input(const char *t,const char *s,const char *g,const char *i,int m,char *out,int cap){(void)t;(void)s;(void)g;(void)i;(void)m;(void)out;(void)cap;LOG("[RED] Text entry is not connected in the bring-up shell");return -1;}
JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_surface(JNIEnv *e,jclass c,jobject surface,jfloat hz){(void)c;ANativeWindow *w=surface?ANativeWindow_fromSurface(e,surface):NULL;atomic_store(&window,w);atomic_store(&refresh_mhz,(int)(hz*1000));dm_android_vk_set_window(w);if(w)ANativeWindow_release(w);}
static char root[2048];
static _Atomic int guest_started;
static void *run(void *unused){(void)unused;char p[4096];
#define ENV(name,sub) snprintf(p,sizeof p,"%s/" sub,root);setenv(name,p,1)
setenv("DM_TOUCH_LOG","0",1);setenv("DM_LANG","zh-CN",1);setenv("DM_OPMODE","0",1);ENV("DM_ROMFS","RomFS");ENV("DM_SAVE","../save");ENV("DM_TEMP","temp");ENV("DM_SHADERS","shaders");
// Preserve the previous crash before opening a new session. If rotation fails,
// append instead of truncating the existing evidence.
int log_rotated=dm_rotate_runtime_log(root,"log.txt")==0;
int log_error=log_rotated?0:errno;
snprintf(p,sizeof p,"%s/log.txt",root);
if(!freopen(p,log_rotated?"w":"a",stderr)){
    __android_log_print(ANDROID_LOG_ERROR,"TaikouDiagnostics","Cannot open runtime log: %s",strerror(errno));return NULL;
}
setvbuf(stderr,NULL,_IOLBF,0);
fprintf(stderr,"[session] pid=%d unix_seconds=%lld build=%s %s previous_logs=3 rotation_error=%d\n",(int)getpid(),(long long)time(NULL),__DATE__,__TIME__,log_error);
int crash_rotated=dm_rotate_runtime_log(root,"crash.txt")==0;
snprintf(p,sizeof p,"%s/crash.txt",root);
int crash_fd=open(p,O_WRONLY|O_CREAT|O_APPEND|O_CLOEXEC|(crash_rotated?O_TRUNC:0),0600);
if(crash_fd>=0){dprintf(crash_fd,"[session] pid=%d unix_seconds=%lld\n",(int)getpid(),(long long)time(NULL));crash_set_report_fd(crash_fd);}
else fprintf(stderr,"[diagnostics] Cannot open emergency crash log: %s\n",strerror(errno));
snprintf(p,sizeof p,"%s/shaders",root);char error[1024];if(dm_android_vk_initialize(p,error,sizeof error)){fprintf(stderr,"[RED] Vulkan initialization: %s\n",error);return NULL;}
snprintf(p,sizeof p,"%s/out",root);char *argv[]={"taikou",p,"main","subsdk0","sdk",NULL};dm_main(5,argv);return NULL;}
JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_start(JNIEnv *e,jclass c,jstring path){(void)c;
// Activity recreation replaces the Surface, but the guest and renderer belong to
// the process. Reuse them instead of starting a second guest/Vulkan instance.
if(atomic_exchange(&guest_started,1)){LOG("[lifecycle] reused running guest after Activity recreation");return;}
const char *p=(*e)->GetStringUTFChars(e,path,NULL);if(!p){atomic_store(&guest_started,0);return;}snprintf(root,sizeof root,"%s",p);(*e)->ReleaseStringUTFChars(e,path,p);pthread_t t;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,8<<20);int rc=pthread_create(&t,&a,run,NULL);pthread_attr_destroy(&a);if(rc){atomic_store(&guest_started,0);LOG("[RED] guest thread creation failed: %d",rc);return;}pthread_detach(t);}
JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_touch(JNIEnv *e,jclass c,jint n,jfloatArray coords){(void)c;static unsigned notes;if(notes++<32)LOG("[input] Android touch points=%d",n);dm_touch t[16];if(n==-1){dm_touch_cancel();return;}if(n<0||n>16)return;jfloat a[48];if(n)(*e)->GetFloatArrayRegion(e,coords,0,3*n,a);for(int i=0;i<n;i++)t[i]=(dm_touch){(uint32_t)a[3*i],a[3*i+1],a[3*i+2]};dm_touch_set(t,n);}

JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_pad(JNIEnv *e,jclass c,jlong buttons){(void)e;(void)c;dm_pad_set((uint64_t)buttons,0,0,0,0);}

void dm_audio_suspend(int on);
uint64_t dm_audio_render_calls(void);
JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_foreground(JNIEnv *e,jclass c,jboolean active){
 (void)e;(void)c;dm_audio_suspend(!active);
 LOG("[lifecycle] foreground=%d hardwareAudioCallbacks=%llu",!!active,(unsigned long long)dm_audio_render_calls());
}

JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_virtualPad(JNIEnv *e,jclass c,jlong buttons,jfloat lx,jfloat ly){(void)e;(void)c;dm_input_set((uint64_t)buttons,(int32_t)(lx*32767),(int32_t)(ly*32767),0,0);}
