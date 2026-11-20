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
    
    colour makeColour(byte r, byte g, byte b) const;

    bool drawPixel(u64 x, u64 y, colour c);
    // Inclusive mins, exclusive maxs
    void drawRect(u64 x1, u64 y1, u64 x2, u64 y2, colour c);
    void drawChar(char c, u64 x, u64 y, colour fg, colour bg, byte scale = 1);
    void drawString(const char* c, u64 x, u64 y, colour fg, colour bg, byte scale = 1);
    void clearScreen(colour c) { drawRect(0, 0, width, height, c); }

private:
    volatile u32* rowPtr(u64 y) const {
        return reinterpret_cast<volatile u32*>(address + y * pitch);
    }

};