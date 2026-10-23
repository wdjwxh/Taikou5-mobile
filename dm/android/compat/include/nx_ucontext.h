#pragma once
#include <stdint.h>
#include <sys/ucontext.h>
#if defined(__ANDROID__) && defined(__aarch64__)
static inline uint64_t nx_ucontext_esr(const ucontext_t *uc) {
    const unsigned char *p=(const unsigned char *)uc->uc_mcontext.__reserved;
    const unsigned char *end=p+sizeof uc->uc_mcontext.__reserved;
    while(p+8<=end) {
        const uint32_t *h=(const uint32_t *)p;
        if(!h[0] || h[1]<8 || p+h[1]>end) break;
        if(h[0]==0x45535201u && h[1]>=16) return *(const uint64_t *)(p+8);
        p+=h[1];
    }
    return 0;
}
#endif
