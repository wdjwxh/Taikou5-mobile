#include "hle.h"
#include "../../host/hle_movie_avc.h"
mavc *mavc_new(void){die("[RED] AVC movies require an Android MediaCodec adapter");}
void mavc_free(mavc *a){(void)a;}
void mavc_param_sets(mavc *a,const uint8_t *p,size_t n){(void)a;(void)p;(void)n;die("[RED] AVC is unavailable");}
void mavc_avcc(mavc *a,const uint8_t *p,size_t n){mavc_param_sets(a,p,n);}
int mavc_decode(mavc *a,const uint8_t *p,int n,int64_t pts){(void)pts;mavc_param_sets(a,p,n);return -1;}
int mavc_take(mavc *a,int flush,uint8_t *p,int w,int h,int64_t *pts){(void)a;(void)flush;(void)p;(void)w;(void)h;(void)pts;die("[RED] AVC is unavailable");}
int mavc_queued(const mavc *a){(void)a;die("[RED] AVC is unavailable");}
void mavc_set_rgba(mavc *a){(void)a;die("[RED] AVC is unavailable");}
void mavc_drop(mavc *a){(void)a;die("[RED] AVC is unavailable");}
