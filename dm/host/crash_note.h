#pragma once
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

// Emergency signal record: no stdio locks, allocation, symbol lookup or clock.
static inline char *dm_crash_note_hex(char *out,uint64_t value){
    static const char digits[]="0123456789abcdef";char reverse[16];unsigned n=0;
    *out++='0';*out++='x';do{reverse[n++]=digits[value&15];value>>=4;}while(value);
    while(n)*out++=reverse[--n];return out;
}
static inline void dm_crash_note_fd(int fd,int sig,int code,uint64_t address,uint64_t pc,uint64_t lr,uint64_t sp){
    if(fd<0)return;
    int saved=errno;char buffer[256],*out=buffer;
#define NOTE_TEXT(s) do{const char *p=(s);while(*p)*out++=*p++;}while(0)
#define NOTE_VALUE(label,value) do{NOTE_TEXT(label);out=dm_crash_note_hex(out,(uint64_t)(value));}while(0)
    NOTE_TEXT("CRASH");NOTE_VALUE(" signal=",sig);NOTE_VALUE(" code=",(int64_t)code);
    NOTE_VALUE(" address=",address);NOTE_VALUE(" pc=",pc);NOTE_VALUE(" lr=",lr);NOTE_VALUE(" sp=",sp);*out++='\n';
    // One bounded record; the regular stderr report supplies symbols and stack.
    ssize_t result;do{result=write(fd,buffer,(size_t)(out-buffer));}while(result<0 && errno==EINTR);
    (void)result;errno=saved;
#undef NOTE_VALUE
#undef NOTE_TEXT
}
