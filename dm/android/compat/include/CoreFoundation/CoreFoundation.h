#pragma once
#include <time.h>
typedef double CFAbsoluteTime;
static inline CFAbsoluteTime CFAbsoluteTimeGetCurrent(void) {
    struct timespec ts; clock_gettime(CLOCK_REALTIME,&ts);
    return (double)ts.tv_sec + ts.tv_nsec/1e9 - 978307200.0;
}

#include <stdlib.h>
#include <stdint.h>
typedef struct nx_cf_entry {const void *key;const void *value;struct nx_cf_entry *next;} nx_cf_entry;
typedef struct {nx_cf_entry *buckets[256];} *CFMutableDictionaryRef;
static inline CFMutableDictionaryRef CFDictionaryCreateMutable(void *a,int n,void *k,void *v){(void)a;(void)n;(void)k;(void)v;return calloc(1,sizeof(struct {nx_cf_entry *buckets[256];}));}
static inline unsigned nx_cf_hash(const void *p){uintptr_t x=(uintptr_t)p;x^=x>>17;x*=0xed5ad4bbU;return (unsigned)(x^(x>>16))&255;}
static inline const void *CFDictionaryGetValue(CFMutableDictionaryRef d,const void *p){for(nx_cf_entry *e=d->buckets[nx_cf_hash(p)];e;e=e->next)if(e->key==p)return e->value;return NULL;}
static inline void CFDictionarySetValue(CFMutableDictionaryRef d,const void *p,const void *v){unsigned h=nx_cf_hash(p);for(nx_cf_entry *e=d->buckets[h];e;e=e->next)if(e->key==p){e->value=v;return;}nx_cf_entry *e=malloc(sizeof *e);if(!e)abort();*e=(nx_cf_entry){p,v,d->buckets[h]};d->buckets[h]=e;}
static inline void CFDictionaryRemoveValue(CFMutableDictionaryRef d,const void *p){nx_cf_entry **e=&d->buckets[nx_cf_hash(p)];while(*e){if((*e)->key==p){nx_cf_entry *old=*e;*e=old->next;free(old);return;}e=&(*e)->next;}}
