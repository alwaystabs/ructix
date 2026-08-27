#ifndef TIMER_H
#define TIMER_H

#define MTIME_ADDR  0x0200BFF8
#define MTIMECMP_ADDR 0x02004000
#define TIMER_INTERVAL (TIMER_FREQ / TICKS_PER_SEC)
#define MTIME ((volatile uint64_t *)0x0200BFF8)
#define MTIMECMP ((volatile uint64_t *)0x02004000)
#define TIMER_FREQ 10000000
#define TICKS_PER_SEC 60

#endif