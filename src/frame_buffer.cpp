#include "frame_buffer.hpp"
#include "font.hpp"
#include "limine.h"
#include "serial.hpp"
#include "types.hpp"
#include "util.hpp"

FrameBuffer& FrameBuffer::get() {
    static constinit FrameBuffer instance{};
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

colour FrameBuffer::makeColour(byte r, byte g, byte b) const {
    return static_cast<colour>(r) >> ((8 - redSize)) << redShift  |
        static_cast<colour>(g) >> ((8 - greenSize)) << greenShift |
        static_cast<colour>(b) >> ((8 - blueSize)) << blueShift;
}

bool FrameBuffer::drawPixel(u64 x, u64 y, colour c) {
    if (x >= width || y >= height) return false;

    rowPtr(y)[x] = c;
    return true;
}

void FrameBuffer::drawChar(char c, u64 x, u64 y, colour fg, colour bg, byte scale) {
    byte b = static_cast<byte>(c);
    const Glyph& g = b >= 128 ? MISSING : font.glyphs[b];
    if (x >= width || y >= height) return;

    for (int row = 0; row < 8; ++row) {
        byte bits = g.rows[row];
        for (int col = 0; col < 8; ++col) {
            const u64 px = x + col * scale, py = y + row * scale;
            drawRect(px, py, px + scale, py + scale, (bits >> (7 - col)) & 1 ? fg : bg);
        }
    }
}

void FrameBuffer::drawString(const char* s, u64 x, u64 y, colour fg, colour bg, byte scale) {
    u64 i{};
    for (; *s; ++s, ++i) {
        drawChar(*s, x + i * 8 * scale, y, fg, bg, scale);
    }
}

void FrameBuffer::drawRect(u64 x1, u64 y1, u64 x2, u64 y2, colour c){
    x2 = min(x2, width);
    y2 = min(y2, height);
    
    if (x1 >= x2 || y1 >= y2) return;

    for (u64 y = y1; y < y2; ++y) {
        auto* const r = rowPtr(y);
        for (u64 x = x1; x < x2; ++x) {
            r[x] = c;
        }
    }    
}