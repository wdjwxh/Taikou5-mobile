// Included inside the replay namespace in integration-test builds only.
// No game inputs: clear a real Vulkan image, replace one guest byte, and verify
// every texel across three mip levels and two array layers, plus storage padding.
void cpu_image_merge_test(){
    static bool ran=false;if(ran)return;ran=true;
    tex_t texture{};texture.magic=MAGIC('T','E','X',0);texture.serial=UINT64_MAX-1;
    texture.d.target=4;texture.d.format=NVN_FORMAT_RGBA8;texture.d.width=17;texture.d.height=9;texture.d.depth=2;texture.d.levels=3;
    nvn_tex_layout(&texture.d,&texture.L);
    std::vector<uint8_t> storage(texture.L.size,0xa5);texture.mem=storage.data();
    Image &im=get_image(&texture,false);
    Commands command;im.transition(command.command,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VkClearColorValue color{};color.float32[0]=.2f;color.float32[1]=.4f;color.float32[2]=.6f;color.float32[3]=.8f;
    VkImageSubresourceRange range{VK_IMAGE_ASPECT_COLOR_BIT,0,im.levels,0,im.layers};
    vkCmdClearColorImage(command.command,im.image,im.layout,&color,1,&range);
    im.transition(command.command,VK_IMAGE_LAYOUT_GENERAL);command.finish();im.gpu_written=true;provenance_write(im,__LINE__);
    uint32_t row,rows,slices,bpb;uint64_t base;unsigned th,td;
    need(nvn_deswizzle_geom(&texture.d,0,0,&row,&rows,&slices,&base,&th,&td,&bpb)==0,"Merge fixture geometry failed");
    const size_t offset=base+nvn_bl_offset(3*4+2,1,0,row,rows,th,td);
    const uint64_t before=cpu_image_merges;
    invalidate_cpu_range(uintptr_t(storage.data()+offset),1);storage[offset]=0x7b;
    need(cpu_image_merges==before+1,"Merge fixture did not exercise exactly one GPU merge");
    std::vector<uint8_t> expected(texture.L.size,0xa5);
    for(uint32_t layer=0;layer<2;layer++)for(uint32_t level=0;level<3;level++){
        uint32_t w=std::max(1u,17u>>level),h=std::max(1u,9u>>level);std::vector<uint8_t> pixels(size_t(w)*h*4);
        for(size_t i=0;i<pixels.size();i++)pixels[i]=uint8_t(51*(i%4+1));
        if(layer==0&&level==0)pixels[(1*w+3)*4+2]=0x7b;
        need(nvn_swizzle(&texture.d,level,layer,pixels.data(),w*4,expected.data())==0,"Merge fixture swizzle failed");
    }
    need(storage==expected,"GPU merge lost pixels, mip/layer contents, patch bytes or padding");
    invalidate_cpu_range(uintptr_t(storage.data()+offset),1);
    need(cpu_image_merges==before+1,"CPU-authoritative write performed an unnecessary GPU merge");
    im.gpu_written=true;invalidate_cpu_range(uintptr_t(storage.data()),storage.size());
    need(cpu_image_merges==before+1,"Full overwrite performed an unnecessary GPU merge");
    // Also enter through deferred slot writeback retirement. The merge must not
    // borrow/reset that slot's arena or recursively resolve the pending list.
    im.gpu_written=true;
    Buffer patch=readback_buffer(1);static_cast<uint8_t*>(patch.mapped)[0]=0x39;patch.flush();
    const void *owner=&texture;
    pending_writebacks.push_back({uintptr_t(storage.data()+offset),1,patch,owner});
    resolve_slot_writebacks(owner);
    need(cpu_image_merges==before+2&&storage[offset]==0x39,"Deferred writeback merge failed");
    expected[offset]=0x39;
    need(storage==expected,"Deferred writeback merge changed untouched pixels or padding");
    images.erase(texture.serial);image_index_dirty=true;++images_epoch;++memo_check_gen;
    xbg_logf("NVN CPUWRITE integration PASS: one-byte overwrite, mip/layer preservation, padding, CPU-only/full-overwrite fast paths, deferred writeback retirement");
}
