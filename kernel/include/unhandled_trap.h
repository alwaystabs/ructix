#ifndef UNHANDLED_TRAP_H
#define UNHANDLED_TRAP_H

#include "kstring.h"
#include "panic.h"
#include "uart.h"
#include <stdint.h>
void unhandled_trap_c(uint64_t cause, uint64_t epc);

#endif