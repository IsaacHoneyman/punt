#include <stddef.h>
#include "frame_buffer.hpp"
#include "limine.h"
#include "serial.hpp"
#include "x86.hpp"

// --- Limine requests ---

[[gnu::used, gnu::section(".limine_requests_start")]]
static volatile u64 limineRequestsStart[] = LIMINE_REQUESTS_START_MARKER;

// Protocol revision we speak; Limine zeroes [2] if supported
[[gnu::used, gnu::section(".limine_requests")]]
static volatile u64 limineBaseRevision[] = LIMINE_BASE_REVISION(6);

[[gnu::used, gnu::section(".limine_requests")]]
static volatile limine_framebuffer_request limineFbRequest = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
    .response = nullptr
};

[[gnu::used, gnu::section(".limine_requests_end")]]
static volatile u64 limineRequestsEnd[] = LIMINE_REQUESTS_END_MARKER;

extern "C" void kmain() {
    if (!LIMINE_BASE_REVISION_SUPPORTED(limineBaseRevision)) halt();

    serialInit(); // init serial (debug connection)

    if (!limineFbRequest.response || limineFbRequest.response->framebuffer_count == 0)
        serialPanic("Framebuffer missing\n");

    FrameBuffer::init(limineFbRequest.response->framebuffers[0]);
    auto& fb = FrameBuffer::get();

    for (u64 i = 0; i < 200; ++i) {
        fb.putPixel(i,      i, fb.makeColour(255, 0, 0));   // red
        fb.putPixel(i + 20, i, fb.makeColour(0, 255, 0));   // green
        fb.putPixel(i + 40, i, fb.makeColour(0, 0, 255));   // blue
    }
    serialWrite("Hello, World!\n");


    halt();
}