#include <stddef.h>
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
    .revision = 0
};

[[gnu::used, gnu::section(".limine_requests_end")]]
static volatile u64 limineRequestsEnd[] = LIMINE_REQUESTS_END_MARKER;

extern "C" void kmain() {
    if (!LIMINE_BASE_REVISION_SUPPORTED(limineBaseRevision)) halt();

    serialInit();
    serialWrite("Hello, World!\n");
    halt();
}