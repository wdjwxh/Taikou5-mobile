#pragma once
#include <stdio.h>
#include <errno.h>
#include <string.h>

// Keep the current session and three previous sessions, including crash logs.
static inline int dm_rotate_runtime_log(const char *root,const char *name){
    char source[4096],destination[4096];
    for(int i=2;i>=0;--i){
        int a=i?snprintf(source,sizeof source,"%s/%s.%d",root,name,i):snprintf(source,sizeof source,"%s/%s",root,name);
        int b=snprintf(destination,sizeof destination,"%s/%s.%d",root,name,i+1);
        if(a<0 || b<0 || (size_t)a>=sizeof source || (size_t)b>=sizeof destination){errno=ENAMETOOLONG;return -1;}
        if(rename(source,destination) && errno!=ENOENT)return -1;
    }
    return 0;
}
