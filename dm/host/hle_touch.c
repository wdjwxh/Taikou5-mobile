// Touchscreen: the front end pushes the whole set of touch points (dm_touch_set, front.h), and the game reads them every frame through nn::hid::GetTouchScreenState(s) (hle_hid.c → touch_fill_n).
// How the game reads (main 0x1b65260): it clears 16 states, asks for 16, and looks only at state 0 -- the touch count and each touch point's finger_id / x / y; the panel coordinate system is 1280x720
// (it writes the constant 0x2d0_00000500 = 720 << 32 | 1280 in the same function). Therefore:
//   · front-end normalized coordinates [0,1) (origin at the top left of the output picture) → x = nx·1280, y = ny·720, clamped to [0,1279] / [0,719];
//   · short-press latch: the game only looks at the newest state, so a finger pressed and lifted between two reads would be lost entirely without a latch -- in this table every finger is first reported once as Start,
//     and is released only after being reported once more as End after lifting (pressed and lifted between two reads = Start in this report, End in the next).
// TouchState (0x28 bytes, = libnx HidTouchState): +0 delta_time (ns, i64), +8 attributes (bit0 Start / bit1 End), +0xc finger_id,
// +0x10 x, +0x14 y, +0x18 diameter x, +0x1c diameter y, +0x20 rotation angle, +0x24 reserved. TouchScreenState<16>: +0 sample number, +8 touch count, 16 TouchState from +0x10.
#include "dm.h"
#include "front.h"
#include <pthread.h>
#include <time.h>
enum { F_EMPTY, F_DOWN, F_LIFTED };
static struct { int st, reported, again; float x, y, ax, ay; } g_f[16];   // again/ax/ay: the same id was pressed again before End went out (the next read reports End first, then the new Start)
static pthread_mutex_t g_tl = PTHREAD_MUTEX_INITIALIZER;
static int64_t g_tsamp; static uint64_t g_tlast_ns;

// Horizon GestureState ABI (0x60 bytes). Keep a newest-first history: callers
// may query twice per frame, so reads must not consume Tap/Pan events.
// Taikou v196608 main+0x42f7cc reads raw touches for position but dispatches
// menu activation from GestureType_Tap (5), not from TouchState Start/End.
typedef struct {
    uint64_t sample, context;
    uint32_t type, direction;
    int32_t x, y, dx, dy;
    float vx, vy;
    uint32_t attributes;
    float scale, rotation;
    int32_t count;
    struct { int32_t x, y; } points[4];
} HostGesture;
_Static_assert(sizeof(HostGesture)==0x60,"GestureState ABI");
static HostGesture g_gestures[16];
static int g_gesture_count, g_gesture_active, g_gesture_drag, g_gesture_press, g_gesture_blocked;
static int g_gesture_id, g_gesture_x, g_gesture_y, g_gesture_start_x, g_gesture_start_y;
static uint64_t g_gesture_sample, g_gesture_context, g_gesture_start_ns;
static uint64_t gesture_time(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t);
    return (uint64_t)t.tv_sec*1000000000ull+(uint64_t)t.tv_nsec;
}
static int gesture_coord(float v,int hi) {
    int r=(int)(v*(hi+1)); return r<0?0:r>hi?hi:r;
}
static void gesture_emit(uint32_t type,int x,int y,int dx,int dy,int points,uint32_t attributes) {
    memmove(g_gestures+1,g_gestures,15*sizeof(HostGesture));
    HostGesture *g=&g_gestures[0];memset(g,0,sizeof(*g));
    g->sample=++g_gesture_sample;g->context=g_gesture_context;g->type=type;
    g->x=x;g->y=y;g->dx=dx;g->dy=dy;g->count=points;
    g->attributes=attributes;g->scale=1.0f;
    if(points){g->points[0].x=x;g->points[0].y=y;}
    if(g_gesture_count<16)++g_gesture_count;
}
// Called under g_tl; multitouch cancels this single-finger recognizer rather
// than turning a pinch or a control finger into an accidental menu tap.
static void gesture_update(const dm_touch *t,int n) {
    if(n>1){
        if(g_gesture_active)gesture_emit(2,g_gesture_x,g_gesture_y,0,0,0,0);
        g_gesture_active=0;g_gesture_blocked=1;return;
    }
    if(g_gesture_blocked){if(!n)g_gesture_blocked=0;return;}
    uint64_t ns=gesture_time();
    if(n==1){
        int x=gesture_coord(t[0].x,1279),y=gesture_coord(t[0].y,719);
        if(g_gesture_active&&g_gesture_id!=(int)t[0].id){
            gesture_emit(2,g_gesture_x,g_gesture_y,0,0,0,0);g_gesture_active=0;
        }
        if(!g_gesture_active){
            g_gesture_active=1;g_gesture_drag=g_gesture_press=0;g_gesture_id=t[0].id;
            g_gesture_start_x=x;g_gesture_start_y=y;g_gesture_start_ns=ns;++g_gesture_context;
            gesture_emit(3,x,y,0,0,1,1u<<4);
        }else{
            int sx=x-g_gesture_start_x,sy=y-g_gesture_start_y;
            if(sx*sx+sy*sy>100)g_gesture_drag=1;
            if(g_gesture_drag&&(x!=g_gesture_x||y!=g_gesture_y))
                gesture_emit(6,x,y,x-g_gesture_x,y-g_gesture_y,1,0);
        }
        g_gesture_x=x;g_gesture_y=y;
    }else if(g_gesture_active){
        if(!g_gesture_drag&&!g_gesture_press&&ns-g_gesture_start_ns<=500000000ull)
            gesture_emit(5,g_gesture_x,g_gesture_y,0,0,1,0);
        gesture_emit(1,g_gesture_x,g_gesture_y,0,0,0,0);g_gesture_active=0;
    }
}
int gesture_fill(void *out,int n) {
    if(n<=0)return 0;if(n>16)n=16;
    pthread_mutex_lock(&g_tl);
    if(g_gesture_active&&!g_gesture_drag&&!g_gesture_press&&gesture_time()-g_gesture_start_ns>500000000ull){
        gesture_emit(4,g_gesture_x,g_gesture_y,0,0,1,0);g_gesture_press=1;
    }
    int count=n<g_gesture_count?n:g_gesture_count;
    memcpy(out,g_gestures,(size_t)count*sizeof(HostGesture));
    pthread_mutex_unlock(&g_tl);return count;
}

void dm_touch_cancel(void) {
    pthread_mutex_lock(&g_tl);
    if(g_gesture_active)gesture_emit(2,g_gesture_x,g_gesture_y,0,0,0,0);
    g_gesture_active=g_gesture_blocked=0;memset(g_f,0,sizeof(g_f));
    pthread_mutex_unlock(&g_tl);
}

void dm_touch_set(const dm_touch *t, int n) {
    pthread_mutex_lock(&g_tl);
    gesture_update(t,n);
    int seen[16] = {0};
    for (int i = 0; i < n; i++) {
        uint32_t id = t[i].id; if (id >= 16) continue;
        seen[id] = 1;
        if (g_f[id].st == F_LIFTED && g_f[id].reported) { g_f[id].again = 1; g_f[id].ax = t[i].x; g_f[id].ay = t[i].y; continue; }
        if (g_f[id].st == F_EMPTY) { g_f[id].st = F_DOWN; g_f[id].reported = 0; }
        else if (g_f[id].st == F_LIFTED) g_f[id].st = F_DOWN;              // Pressed again before the lift was reported: treat it as held down all along
        g_f[id].x = t[i].x; g_f[id].y = t[i].y;
    }
    for (int id = 0; id < 16; id++) {
        if (!seen[id] && g_f[id].st == F_DOWN) g_f[id].st = F_LIFTED;
        if (!seen[id] && g_f[id].again) g_f[id].again = 0;                   // Pressed again and then lifted again (End not even reported yet): drop the re-press, still report End for the original lift
    }
    pthread_mutex_unlock(&g_tl);
}

static int32_t clampi(float v, int hi) { int32_t r = (int32_t)(v * (float)(hi + 1)); return r < 0 ? 0 : r > hi ? hi : r; }
// Fill one TouchScreenState<max> (out is 0x10 + 0x28 · max bytes; max 1–16); now_ns is used for delta_time. Returns the number of touch points written.
// Fingers beyond max are left for a later read (their Start / End is not used up)
int touch_fill_n(void *out, int max, uint64_t now_ns) {
    if (max < 1) max = 1; else if (max > 16) max = 16;
    uint8_t *o = out; memset(o, 0, 0x10 + 0x28 * (size_t)max);
    pthread_mutex_lock(&g_tl);
    int64_t samp = ++g_tsamp; int64_t dt = g_tlast_ns ? (int64_t)(now_ns - g_tlast_ns) : 0; g_tlast_ns = now_ns;
    int k = 0;
    for (int id = 0; id < 16 && k < max; id++) {
        if (g_f[id].st == F_EMPTY) continue;
        uint32_t attr = 0;
        if (!g_f[id].reported) { attr |= 1; g_f[id].reported = 1; }          // Start (reported for the first time this time)
        else if (g_f[id].st == F_LIFTED) attr |= 2;                         // End (Start already reported, already lifted)
        uint8_t *s = o + 0x10 + 0x28 * k++;
        int32_t x = clampi(g_f[id].x, 1279), y = clampi(g_f[id].y, 719), d = 30, zero = 0, fid = id;
        memcpy(s + 0x00, &dt, 8); memcpy(s + 0x08, &attr, 4); memcpy(s + 0x0c, &fid, 4);
        memcpy(s + 0x10, &x, 4); memcpy(s + 0x14, &y, 4); memcpy(s + 0x18, &d, 4); memcpy(s + 0x1c, &d, 4); memcpy(s + 0x20, &zero, 4);
        if (attr & 2) {                                                      // End has been reported: release; if the same id was pressed again in the meantime, turn it into a new press
            if (g_f[id].again) { g_f[id].st = F_DOWN; g_f[id].reported = 0; g_f[id].again = 0; g_f[id].x = g_f[id].ax; g_f[id].y = g_f[id].ay; }
            else g_f[id].st = F_EMPTY;
        }
    }
    pthread_mutex_unlock(&g_tl);
    int32_t cnt = k; memcpy(o + 0, &samp, 8); memcpy(o + 8, &cnt, 4);
    // DM_TOUCH_LOG=1 (acceptance diagnostics): each read with non-zero touch points logs one line -- sample number, touch count, and for each point finger_id / panel x,y / attributes (1 Start, 2 End); capped at 500 lines
    static int lg = -1, nl; if (lg < 0) lg = getenv("DM_TOUCH_LOG") && atoi(getenv("DM_TOUCH_LOG")) != 0;
    if (lg && k && nl < 500) {
        char b[256]; int m = snprintf(b, sizeof b, "touch read sample %lld points %d", (long long)samp, k);
        for (int i = 0; i < k && m < (int)sizeof b - 40; i++) { int32_t v[4]; memcpy(v, o + 0x10 + 0x28 * i + 0x08, 16);
            m += snprintf(b + m, sizeof b - m, " | #%d (%d,%d) attribute %d", v[1], v[2], v[3], v[0]); }
        LOG("%s", b); if (++nl == 500) LOG("DM_TOUCH_LOG reached 500 lines, logging stopped");
    }
    return k;
}
int touch_fill(void *out, uint64_t now_ns) { return touch_fill_n(out, 16, now_ns); }   // TouchScreenState<16>, 0x290 bytes
