#include "HAL.h"

void BB_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void LLE_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void BB_IRQHandler(void)
{
    BB_IRQLibHandler();
}

void LLE_IRQHandler(void)
{
    LLE_IRQLibHandler();
}
