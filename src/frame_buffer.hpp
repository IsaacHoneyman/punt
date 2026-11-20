#pragma once

#include "types.hpp"

struct limine_framebuffer;

struct FrameBuffer {
    byte* address;
    u64 width, height;
    u64 pitch; // bytes per row
    u16 bpp;   // bits per pixel
    byte memoryModel;
    byte redSize, greenSize, blueSize;
    byte redShift, greenShift, blueShift;

    static void init(limine_framebuffer* lfb);
    static FrameBuffer& get();
    
    colour makeColour(byte r, byte g, byte b);
    bool putPixel(u64 x, u64 y, colour c);
};