#include "frame_buffer.hpp"
#include "limine.h"
#include "serial.hpp"
#include "types.hpp"

FrameBuffer& FrameBuffer::get() {
    static FrameBuffer instance{};
    return instance;
}

void FrameBuffer::init(limine_framebuffer* lfb) {
    auto& fb = FrameBuffer::get();
    fb.address = static_cast<byte*>(lfb->address);
    fb.width = lfb->width;
    fb.height = lfb->height;
    fb.pitch = lfb->pitch;
    fb.bpp = lfb->bpp;
    
    if (lfb->memory_model != LIMINE_FRAMEBUFFER_RGB)
        serialPanic("Framebuffer not RGB\n");
    fb.memoryModel = lfb->memory_model;

    fb.redSize = lfb->red_mask_size;
    fb.greenSize = lfb->green_mask_size;
    fb.blueSize = lfb->blue_mask_size;

    fb.redShift = lfb->red_mask_shift;
    fb.greenShift = lfb->green_mask_shift;
    fb.blueShift = lfb->blue_mask_shift;
}

colour FrameBuffer::makeColour(byte r, byte g, byte b) {
    return r >> ((8 - redSize)) << redShift  |
        g >> ((8 - greenSize)) << greenShift |
        b >> ((8 - blueSize)) << blueShift;
}

bool FrameBuffer::putPixel(u64 x, u64 y, colour c) {
    if (x >= width || y >= height) return false;

    *reinterpret_cast<volatile u32*>(address + y * pitch + x * 4) = c;
    return true;
}