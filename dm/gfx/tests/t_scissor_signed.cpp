#include "../vulkan/engine/xbg_clear.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <algorithm>
int main(){
    const int32_t values[]={INT32_MIN,-100,-1,0,1,24,720,1280,INT32_MAX};
    // Compare converted/clipped rectangles to signed NVN interval intersection.
    // This covers reversed, empty, offscreen and overflowing 32-bit endpoints.
    for(auto x:values)for(auto y:values)for(auto w:values)for(auto h:values){
        VkRect2D raw{},clipped{};xbg_scissor_state(x,y,w,h,&raw);
        assert(xbg_clear_rectangle(1280,720,x,y,raw.extent.width,raw.extent.height,&clipped)==0);
        int64_t left=std::max<int64_t>(0,x),top=std::max<int64_t>(0,y);
        int64_t right=std::min<int64_t>(1280,int64_t(x)+w),bottom=std::min<int64_t>(720,int64_t(y)+h);
        bool empty=right<=left || bottom<=top;
        assert((!clipped.extent.width || !clipped.extent.height)==empty);
        if(!empty){assert(clipped.offset.x==left && clipped.offset.y==top);
            assert(clipped.extent.width==right-left && clipped.extent.height==bottom-top);}
    }
    puts("signed scissor: signed intersections, empty bounds and overflow boundaries passed");
}
