// Android bring-up shell; original guest runtime and Vulkan replay run on one host thread.
#include "dm.h"
#include "front.h"
#include "dm_input.h"
#include "../vulkan_present.h"
#include <jni.h>
#include <android/native_window_jni.h>
#include <stdatomic.h>
#include <unistd.h>
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
static void *run(void *unused){(void)unused;char p[4096];
#define ENV(name,sub) snprintf(p,sizeof p,"%s/" sub,root);setenv(name,p,1)
setenv("DM_TOUCH_LOG","0",1);setenv("DM_LANG","zh-CN",1);setenv("DM_OPMODE","0",1);ENV("DM_ROMFS","RomFS");ENV("DM_SAVE","../save");ENV("DM_TEMP","temp");ENV("DM_SHADERS","shaders");
snprintf(p,sizeof p,"%s/log.txt",root);freopen(p,"w",stderr);setvbuf(stderr,NULL,_IOLBF,0);
snprintf(p,sizeof p,"%s/shaders",root);char error[1024];if(dm_android_vk_initialize(p,error,sizeof error)){fprintf(stderr,"[RED] Vulkan initialization: %s\n",error);return NULL;}
snprintf(p,sizeof p,"%s/out",root);char *argv[]={"taikou",p,"main","subsdk0","sdk",NULL};dm_main(5,argv);return NULL;}
JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_start(JNIEnv *e,jclass c,jstring path){(void)c;const char *p=(*e)->GetStringUTFChars(e,path,NULL);snprintf(root,sizeof root,"%s",p);(*e)->ReleaseStringUTFChars(e,path,p);pthread_t t;pthread_attr_t a;pthread_attr_init(&a);pthread_attr_setstacksize(&a,8<<20);pthread_create(&t,&a,run,NULL);pthread_detach(t);pthread_attr_destroy(&a);}
JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_touch(JNIEnv *e,jclass c,jint n,jfloatArray coords){(void)c;static unsigned notes;if(notes++<32)LOG("[input] Android touch points=%d",n);dm_touch t[16];if(n<0||n>16)return;jfloat a[48];if(n)(*e)->GetFloatArrayRegion(e,coords,0,3*n,a);for(int i=0;i<n;i++)t[i]=(dm_touch){(uint32_t)a[3*i],a[3*i+1],a[3*i+2]};dm_touch_set(t,n);}

JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_pad(JNIEnv *e,jclass c,jlong buttons){(void)e;(void)c;dm_pad_set((uint64_t)buttons,0,0,0,0);}

JNIEXPORT void JNICALL Java_org_nxnative_taikou_MainActivity_virtualPad(JNIEnv *e,jclass c,jlong buttons,jfloat lx,jfloat ly){(void)e;(void)c;dm_input_set((uint64_t)buttons,(int32_t)(lx*32767),(int32_t)(ly*32767),0,0);}
