#ifndef __OSFILES__DRIVERS__MOUSE_H
#define __OSFILES__DRIVERS__MOUSE_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace osfiles
{
    namespace drivers
    {

        class MouseEventHandler
        {
        public:
            MouseEventHandler();
            ~MouseEventHandler();


            virtual void OnActivate();
            virtual void OnMouseDown(osfiles::common::uint8_t button);
            virtual void OnMouseUp(osfiles::common::uint8_t button);
            virtual void OnMouseMove(int oldx, int oldy, int newx, int newy);
        };

        class MouseDriver : public osfiles::hardwarecommunication::InterruptHandler, public Driver
        {
            osfiles::hardwarecommunication::Port8Bit dataport;
            osfiles::hardwarecommunication::Port8Bit commandport;

            osfiles::common::uint8_t buffer[3];
            osfiles::common::uint8_t offset;
            osfiles::common::uint8_t buttons;
            
            osfiles::common::int8_t x, y;
            
            MouseEventHandler* handler;

        public:
            MouseDriver(osfiles::hardwarecommunication::InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();

            virtual osfiles::common::uint32_t HandleInterrupt(osfiles::common::uint32_t esp);
            virtual void Activate();
        };
    }
}

#endif