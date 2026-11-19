#pragma once 

// Initalises COM1 (115200 buad, 8N1)
void serialInit(); 
void serialWrite(const char c);
void serialWrite(const char *s);