#include "include/unhandled_trap.h"
#include "include/ansi.h"
#include "include/panic.h"
#include "include/uart.h"

void unhandled_trap_c(uint64_t cause, uint64_t epc) {
    print("\n\n\n");
    print("\033[2J\033[H");
    print(ANSI_BG_RED ANSI_WHITE ANSI_BOLD);
    print("╔══════════════════════════════════════════════════════════════════╗\n");
    print("║                        FATAL KERNEL TRAP                         ║\n");
    print("╠══════════════════════════════════════════════════════════════════╣\n");
    print(ANSI_RESET);

    print(ANSI_RED ANSI_BOLD "\033[4;1H║  TYPE: ");
    if (cause & (1ULL << 63)) {
        print("Hardware Interrupt");
    } else {
        print("Exception");
    }
    print("\033[4;68H║");

    print("\033[5;1H║  CAUSE: ");
    switch (cause) {
    case 0:  print("Instruction Address Misaligned"); break;
    case 1:  print("Instruction Access Fault");       break;
    case 2:  print("Illegal Instruction");            break;
    case 3:  print("Breakpoint");                     break;
    case 4:  print("Load Address Misaligned");        break;
    case 5:  print("Load Access Fault");              break;
    case 6:  print("Store Address Misaligned");       break;
    case 7:  print("Store Access Fault");             break;
    case 8:  print("User Environment Call");          break;
    case 9:  print("Supervisor Environment Call");    break;
    case 11: print("Machine Environment Call");       break;
    case 12: print("Instruction Page Fault");         break;
    case 13: print("Load Page Fault");                break;
    case 15: print("Store Page Fault");               break;
    default: print("Unknown Exception");              break;
}
    print("\033[5;68H║");

    print("\033[6;1H║  mcause: ");
    print_hex(cause);
    print("  |  mepc: ");
    print_hex(epc);
    print("\033[6;68H║");

    print("\033[7;1H╚══════════════════════════════════════════════════════════════════╝\n");

    print(ANSI_RESET);
    panic("Kernel Unhandled Trap");
}