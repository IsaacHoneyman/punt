#pragma once
#include "types.hpp"

inline void outByte(u16 port, byte val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline byte inByte(u16 port) {
    byte ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

[[noreturn]]
inline void halt() {
    for (;;) asm volatile("cli; hlt");
}
