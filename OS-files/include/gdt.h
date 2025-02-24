#ifndef __OSFILES__GDT_H
#define __OSFILES__GDT_H

#include <common/types.h>

namespace
{
    class GlobalDescriptorTable
    {
        public:
            class SegmentDescriptor
            {
                private:
                osfiles::common::uint16_t limit_lo;
                osfiles::common::uint16_t base_lo;
                osfiles::common::uint8_t base_hi;
                osfiles::common::uint8_t type;
                osfiles::common::uint8_t limit_hi;
                osfiles::common::uint8_t base_vhi;
                public:
                    SegmentDescriptor(osfiles::common::uint32_t base, osfiles::common::uint32_t limit, osfiles::common::uint8_t type);
                    osfiles::common::uint32_t Base();
                    osfiles::common::uint32_t Limit();
            } __attribute__((packed));

        private:
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;

        public:
            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            osfiles::common::uint16_t CodeSegmentSelector();
            osfiles::common::uint16_t DataSegmentSelector();

    };
}

#endif