#ifndef __OSFILES__HARDWARECOMMUNICATION__PORT_H
#define __OSFILES__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>

namespace osfiles
{
    namespace hardwarecommunication
    {
    class Port {
        protected:
        osfiles::common::uint16_t portnumber;
            Port(osfiles::common::uint16_t portnumber);
            ~Port();
    };

    class Port8Bit : public Port {
        public:
            Port8Bit(osfiles::common::uint16_t portnumber);
            ~Port8Bit();
            virtual void Write(osfiles::common::uint8_t data);
            virtual osfiles::common::uint8_t Read();
    };

    class Port8BitSlow : public Port8Bit {
        public:
            Port8BitSlow(osfiles::common::uint16_t portnumber);
            ~Port8BitSlow();
            virtual void Write(osfiles::common::uint8_t data);
    };

    class Port16Bit : public Port {
        public:
            Port16Bit(osfiles::common::uint16_t portnumber);
            ~Port16Bit();
            virtual void Write(osfiles::common::uint16_t data);
            virtual osfiles::common::uint16_t Read();
    };

    class Port32Bit : public Port {
        public:
            Port32Bit(osfiles::common::uint16_t portnumber);
            ~Port32Bit();
            virtual void Write(osfiles::common::uint32_t data);
            virtual osfiles::common::uint32_t Read();
    };
    }
}

#endif