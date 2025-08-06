#ifndef __OSFILES__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H
#define __OSFILES__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <gdt.h>

namespace osfiles
{
    namespace hardwarecommunication
    {

        class InterruptManager;

        class InterruptHandler
        {
        protected:
            osfiles::common::uint8_t InterruptNumber;
            InterruptManager* interruptManager;
            InterruptHandler(InterruptManager* interruptManager, osfiles::common::uint8_t InterruptNumber);
            ~InterruptHandler();
        public:
            virtual osfiles::common::uint32_t HandleInterrupt(osfiles::common::uint32_t esp);
        };



        class InterruptManager
        {
            friend class InterruptHandler;
            protected:
                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256];

                struct GateDescriptor
                {
                    osfiles::common::uint16_t handlerAddressLowBits;
                    osfiles::common::uint16_t gdt_codeSegmentSelector;
                    osfiles::common::uint8_t reserved;
                    osfiles::common::uint8_t access;
                    osfiles::common::uint16_t handlerAddressHighBits;

                } __attribute__((packed));

                static GateDescriptor interruptDescriptorTable[256];

                struct InterruptDescriptorTablePointer
                {
                    osfiles::common::uint16_t size;
                    osfiles::common::uint32_t base;
                } __attribute__((packed));

                osfiles::common::uint16_t hardwareInterruptOffset;
                static void SetInterruptDescriptorTableEntry(
                    osfiles::common::uint8_t interrupt,
                    osfiles::common::uint16_t codeSegmentSelectorOffset,
                    void (*handler)(),
                    osfiles::common::uint8_t DescriptorPrivilegeLevel,
                    osfiles::common::uint8_t DescriptorType);


                static void InterruptIgnore();

                static void HandleInterruptRequest0x00();
                static void HandleInterruptRequest0x01();
                static void HandleInterruptRequest0x02();
                static void HandleInterruptRequest0x03();
                static void HandleInterruptRequest0x04();
                static void HandleInterruptRequest0x05();
                static void HandleInterruptRequest0x06();
                static void HandleInterruptRequest0x07();
                static void HandleInterruptRequest0x08();
                static void HandleInterruptRequest0x09();
                static void HandleInterruptRequest0x0A();
                static void HandleInterruptRequest0x0B();
                static void HandleInterruptRequest0x0C();
                static void HandleInterruptRequest0x0D();
                static void HandleInterruptRequest0x0E();
                static void HandleInterruptRequest0x0F();
                static void HandleInterruptRequest0x31();

                static void HandleException0x00();
                static void HandleException0x01();
                static void HandleException0x02();
                static void HandleException0x03();
                static void HandleException0x04();
                static void HandleException0x05();
                static void HandleException0x06();
                static void HandleException0x07();
                static void HandleException0x08();
                static void HandleException0x09();
                static void HandleException0x0A();
                static void HandleException0x0B();
                static void HandleException0x0C();
                static void HandleException0x0D();
                static void HandleException0x0E();
                static void HandleException0x0F();
                static void HandleException0x10();
                static void HandleException0x11();
                static void HandleException0x12();
                static void HandleException0x13();

                static osfiles::common::uint32_t HandleInterrupt(
                    osfiles::common::uint8_t interrupt,
                    osfiles::common::uint32_t esp);
                    osfiles::common::uint32_t DoHandleInterrupt(osfiles::common::uint8_t interrupt,
                        osfiles::common::uint32_t esp);

                Port8BitSlow picMasterCommand;
                Port8BitSlow picMasterData;
                Port8BitSlow picSlaveCommand;
                Port8BitSlow picSlaveData;

            public:
                InterruptManager(osfiles::common::uint16_t hardwareInterruptOffset, osfiles::GlobalDescriptorTable* globalDescriptorTable);
                ~InterruptManager();
                osfiles::common::uint16_t HardwareInterruptOffset();
                void Activate();
                void Deactivate();
        };
    }
}
#endif