#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H
#include "types.h"
#include "port.h"

    class InterruptManager {


        protected:
            struct GateDescriptor
            {
                uint16_t handlerAddressLowBits;
                uint16_t gdt_codeSegmentSelector;
                uint8_t reserved;
                uint8_t access;
                uint16_t handlerAddressHighBits;
                
            } __attribute__((packed));

            static GateDescriptor interruptDescriptorTable[256];



        public:
            static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);
            

            static void HandleInterruptRequest0x00();
            static void HandleInterruptRequest0x01();
    };
    


#endif