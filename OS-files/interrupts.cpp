#include "interrupts.h"


void printf(const char* str);

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp)
{

    
    printf("INTERRUPT");

    return esp;
}