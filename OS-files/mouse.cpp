#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager* manager)
: InterruptHandler(manager, 0x2C),
dataport(0x60),
commandport(0x64)
{
    offset = 0;
    buttons = 0;
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xF000) >> 4)
                          | ((VideoMemory[80*12+40] & 0x0F00) << 4)
                          | ((VideoMemory[80*12+40] & 0x0FFF));

    commandport.Write(0xA8); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();
}

MouseDriver::~MouseDriver()
{
}

void printf(char*);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if (!(status & 0x20))
        return esp;

    static int8_t x = 40, y = 12;

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;

    if (offset == 0)
    {
        if (buffer[1] != 0 || buffer[2] != 0)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;

            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                                | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                                | ((VideoMemory[80*y+x] & 0x0FFF));

            x += buffer[1];
            if (x < 0) x = 0;
            if (x >= 80) x = 79; //boundries x-axis

            y -= buffer[2];
            if (y < 0) y = 0;
            if (y >= 25) y = 24; //boundries y-axis


            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                                | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                                | ((VideoMemory[80*y+x] & 0x0FFF));


            //to slow down the mouse movement
            for (uint8_t i = 0; i < 3; i++)
            {
                if ((buffer[0] & (0x1 << i)) != (buttons & (0x1 << i)))
                {
                    VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xF000) >> 4)
                                          | ((VideoMemory[80*12+40] & 0x0F00) << 4)
                                          | ((VideoMemory[80*12+40] & 0x0FFF));
                }
            }
            buttons = buffer[0];
        }
    }

    //uint8_t key = dataport.Read();
    
    return esp;
}