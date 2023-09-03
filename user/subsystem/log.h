#ifndef LOG_H
#define LOG_H

/* #include "tiny_printf.h" */
#include <stdio.h>
#include "board.h"
//#include <stdint.h>
#include <stddef.h>

#define log_printf(macropar_message, ...)                                      \
  printf(macropar_message, ##__VA_ARGS__)

#define ASSERT(x)  do{if(!(x)) log_printf("assert failed at %s:%d\n", __FILE__, __LINE__);}while(0)

void DumpHex(const void *data, size_t size);
void DumpBinary(uint8_t *buf, size_t length);

#endif
