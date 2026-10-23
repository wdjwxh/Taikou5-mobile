// 安卓构建：malloc_zone_statistics（宿主只用 size_in_use 作内存读数）= bionic mallinfo 的在用字节。
// Android build: malloc_zone_statistics (the host only uses size_in_use as a memory reading) = the in-use bytes from bionic mallinfo.
#pragma once
#include <malloc.h>
#include <stddef.h>
typedef struct { unsigned blocks_in_use; size_t size_in_use, max_size_in_use, size_allocated; } malloc_statistics_t;
static inline void malloc_zone_statistics(void *zone, malloc_statistics_t *s) {
    (void)zone; struct mallinfo mi = mallinfo();
    s->blocks_in_use = 0; s->size_in_use = mi.uordblks; s->max_size_in_use = 0; s->size_allocated = mi.arena;
}

#include <stdlib.h>
typedef int malloc_zone_t;
static inline void *malloc_zone_malloc(malloc_zone_t *z,size_t n){(void)z;return malloc(n);}
static inline void *malloc_zone_memalign(malloc_zone_t *z,size_t a,size_t n){(void)z;void *p=NULL;return posix_memalign(&p,a,n)?NULL:p;}
static inline void malloc_zone_free(malloc_zone_t *z,void *p){(void)z;free(p);}
static inline void *malloc_zone_realloc(malloc_zone_t *z,void *p,size_t n){(void)z;return realloc(p,n);}
#define malloc_size malloc_usable_size
static inline size_t malloc_good_size(size_t n){return n;}
static inline malloc_zone_t *malloc_create_zone(size_t n,unsigned flags){(void)n;(void)flags;static int zone;return &zone;}
static inline void malloc_set_zone_name(malloc_zone_t *z,const char *name){(void)z;(void)name;}
