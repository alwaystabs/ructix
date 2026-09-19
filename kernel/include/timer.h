#ifndef TIMER_H
#define TIMER_H

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

#define TIMER_FREQ 10000000
#define TICKS_PER_SEC 100
#define TIMER_INTERVAL (TIMER_FREQ / TICKS_PER_SEC)

#define MTIMECMP_ADDR 0x02004000
#define MTIME_ADDR  0x0200BFF8

#ifndef __ASSEMBLER__
#define MTIME ((volatile uint64_t *)MTIME_ADDR)
#define MTIMECMP ((volatile uint64_t *)MTIMECMP_ADDR)
#endif

#endif
