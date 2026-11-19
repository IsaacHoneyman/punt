#include "serial.hpp"
#include "x86.hpp"

// --- UART (Universal Asynchronous Receiver/Transmitter) ---
// The serial port chip (a 16550-compatible UART). Its registers sit at
// I/O ports COM1 + offset.

namespace UART {

constexpr u16 COM1 = 0x3F8;

// Offset 0: write = send a byte, read = received byte.
// With DLAB set: low byte of the baud-rate divisor.
constexpr u16 DATA = 0;
// Interrupt Enable Register: which events raise an interrupt.
// With DLAB set: high byte of the baud-rate divisor.
constexpr u16 IER = 1;
// FIFO Control Register: turns the chip's send/receive
constexpr u16 FCR = 2;
// Line Control Register: data format (bits per character, parity, stop bits)
// and the DLAB bit.
constexpr u16 LCR = 3;
// Modem Control Register: control lines to the other end
constexpr u16 MCR = 4;
// Line Status Register
constexpr u16 LSR = 5;
// LSR bit 5 / 7
constexpr byte LSR_THR_EMPTY = 1 << 5;
constexpr byte LCR_DLAB = 1 << 7;

} // namespace UART

// Set up COM1 at 115200 baud (bits per second), 8N1 format
void serialInit() {
    // disable all UART interrupts
    outByte(UART::COM1 + UART::IER, 0x00);
    // set DLAB: offsets 0/1 are now the divisor
    outByte(UART::COM1 + UART::LCR, UART::LCR_DLAB);
    // divisor low byte = 3: 115200 / 3 = 38400 baud
    outByte(UART::COM1 + UART::DATA, 0x03);
    // divisor high byte = 0
    outByte(UART::COM1 + UART::IER, 0x00);
    // 8N1, and clears DLAB so offsets 0/1 are DATA/IER again
    outByte(UART::COM1 + UART::LCR, 0x01);
    // enable FIFOs, clear both, interrupt threshold 14 bytes
    outByte(UART::COM1 + UART::FCR, 0xC7);
    // DTR (Data Terminal Ready) + RTS (Request To Send):
    outByte(UART::COM1 + UART::MCR, 0x0B);
}

void serialWrite(const char c) {
    while ((inByte(UART::COM1 + UART::LSR) & UART::LSR_THR_EMPTY) == 0) {}
    outByte(UART::COM1 + UART::DATA, c);
    
}

void serialWrite(const char *s) {
    for (; *s; ++s) serialWrite(*s);
}
