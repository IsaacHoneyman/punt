#include <stddef.h>
#include "limine.h"
#include "types.hpp"

// --- Limine requests ---

[[gnu::used, gnu::section(".limine_requests_start")]]
static volatile u64 limine_requests_start[] = LIMINE_REQUESTS_START_MARKER;

// Protocol revision we speak; Limine zeroes [2] if supported
[[gnu::used, gnu::section(".limine_requests")]]
static volatile u64 limine_base_revision[] = LIMINE_BASE_REVISION(6);

[[gnu::used, gnu::section(".limine_requests_end")]]
static volatile u64 limine_requests_end[] = LIMINE_REQUESTS_END_MARKER;

// --- UART ---

namespace UART {
    constexpr u16 DATA = 0; // DLAB=1: divisor low
    constexpr u16 IER  = 1; // DLAB=1: divisor high
    constexpr u16 FCR  = 2;
    constexpr u16 LCR  = 3;
    constexpr u16 MCR  = 4;
    constexpr u16 LSR  = 5;

    constexpr byte LSR_THR_EMPTY = 1 << 5;
    constexpr byte LCR_DLAB      = 1 << 7;
} // namespace UART

namespace {

constexpr u16 COM1 = 0x3F8;

[[noreturn]]
void halt() {
    for (;;) asm volatile("cli; hlt");
}

void outByte(u16 port, byte val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

byte inByte(u16 port) {
    byte ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// 38400 baud, 8N1, polled
void serialInit() {
    outByte(COM1 + UART::IER,  0x00);           // no interrupts
    outByte(COM1 + UART::LCR,  UART::LCR_DLAB);
    outByte(COM1 + UART::DATA, 0x03);           // divisor 3 -> 38400 baud
    outByte(COM1 + UART::IER,  0x00);
    outByte(COM1 + UART::LCR,  0x03);           // 8N1, DLAB off
    outByte(COM1 + UART::FCR,  0xC7);           // FIFOs on + cleared
    outByte(COM1 + UART::MCR,  0x0B);           // DTR, RTS, OUT2
}

void serialWrite(const char *s) {
    for (; *s; ++s) {
        while ((inByte(COM1 + UART::LSR) & UART::LSR_THR_EMPTY) == 0) {}
        outByte(COM1 + UART::DATA, *s);
    }
}

} // namespace

extern "C" void kmain() {
    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) halt();

    serialInit();
    serialWrite("Hello, World!\n");
    halt();
}