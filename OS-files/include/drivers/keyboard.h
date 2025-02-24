#ifndef __OSFILES__DRIVERS__KEYBOARD_H
#define __OSFILES__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace osfiles
{
    namespace drivers
    {

        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();
            ~KeyboardEventHandler();

            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };

        class KeyboardDriver : public osfiles::hardwarecommunication::InterruptHandler, public Driver
        {
            osfiles::hardwarecommunication::Port8Bit dataport;
            osfiles::hardwarecommunication::Port8Bit commandport;

            KeyboardEventHandler* handler;
            bool activated; // Add this line

        public:
            KeyboardDriver(osfiles::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler* handler);
            ~KeyboardDriver();
            virtual osfiles::common::uint32_t HandleInterrupt(osfiles::common::uint32_t esp);
            virtual void Activate();
            bool IsActivated(); // Add this line
        };
    }
}

#endif