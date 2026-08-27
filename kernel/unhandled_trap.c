#include "include/unhandled_trap.h"
void unhandled_trap_c(uint64_t cause, uint64_t epc) {
  print("\033[2J\033[H"); // очистить экран и вернуть курсор в (0,0)

  // Рамка фиксированного размера
  print("\033[1;31m"); // красный цвет

  print("\033[1;"
        "1H╔══════════════════════════════════════════════════════════════════╗"
        "\n");
  print("\033[2;1H║                        FATAL KERNEL TRAP                   "
        "      ║\n");
  print("\033[3;"
        "1H╠══════════════════════════════════════════════════════════════════╣"
        "\n");

  // Строка 4: TYPE
  print("\033[4;1H║  TYPE: ");
  if (cause & (1ULL << 63)) {
    print("Hardware Interrupt");
  } else {
    print("Exception");
  }
  // добиваем пробелами до 70-й позиции
  print("\033[4;68H║");

  // Строка 5: CAUSE
  print("\033[5;1H║  CAUSE: ");
  switch (cause) {
  case 0:
    print("Instruction Address Misaligned");
    break;
  case 2:
    print("Illegal Instruction");
    break;
  case 4:
    print("Load Address Misaligned");
    break;
  case 6:
    print("Store Address Misaligned");
    break;
  case 13:
    print("Load Page Fault");
    break;
  default:
    print("Unknown Exception");
    break;
  }
  print("\033[5;68H║");

  // Строка 6: регистры
  print("\033[6;1H║  mcause: ");
  print_hex(cause);
  print("  |  mepc: ");
  print_hex(epc);
  print("\033[6;68H║");

  // Нижняя рамка
  print("\033[7;"
        "1H╚══════════════════════════════════════════════════════════════════╝"
        "\n");

  print("\033[0m"); // сброс цвета
  panic("Kernel Trap");
}