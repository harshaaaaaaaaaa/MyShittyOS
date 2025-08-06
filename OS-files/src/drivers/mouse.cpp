#include <drivers/mouse.h>

using namespace osfiles::drivers;
using namespace osfiles::common;
using namespace osfiles::hardwarecommunication;

MouseEventHandler::MouseEventHandler()
{
}

MouseEventHandler::~MouseEventHandler()
{
}

void MouseEventHandler::OnActivate()
{
}

void MouseEventHandler::OnMouseDown(uint8_t button)
{
}

void MouseEventHandler::OnMouseUp(uint8_t button)
{
}

void MouseEventHandler::OnMouseMove(int oldx, int oldy, int newx, int newy)
{
}




MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
    : InterruptHandler(manager, 0x20 + 12),
    dataport(0x60),
    commandport(0x64)
    {
        this->handler = handler;
        x = 40; // Start at center of 80-column screen
        y = 12; // Start at center of 25-row screen
    }

    MouseDriver::~MouseDriver()
    {
    }
    
    void MouseDriver::Activate()
    {
        offset = 0;
        buttons = 0;

        if(handler != 0)
            handler->OnActivate();
        
        commandport.Write(0xA8);
        commandport.Write(0x20); // command 0x60 = read controller command byte
        uint8_t status = dataport.Read() | 2;
        commandport.Write(0x60); // command 0x60 = set controller command byte
        dataport.Write(status);

        commandport.Write(0xD4);
        dataport.Write(0xF4);
        dataport.Read();        
    }
    
    uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
    {
        uint8_t status = commandport.Read();
        if (!(status & 0x20))
            return esp;

        buffer[offset] = dataport.Read();
        
        if(handler == 0)
            return esp;
        
        offset = (offset + 1) % 3;

        if(offset == 0)
        {
            if(buffer[1] != 0 || buffer[2] != 0)
            {
                int8_t oldx = x, oldy = y;
                
                x += buffer[1];
                if(x < 0) x = 0;
                if(x >= 80) x = 79;
                
                y -= buffer[2]; // Mouse Y is inverted
                if(y < 0) y = 0;
                if(y >= 25) y = 24;
                
                handler->OnMouseMove(oldx, oldy, x, y);
            }

            for(uint8_t i = 0; i < 3; i++)
            {
                if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
                {
                    if(buttons & (0x1<<i))
                        handler->OnMouseUp(i+1);
                    else
                        handler->OnMouseDown(i+1);
                }
            }
            buttons = buffer[0];
        }
        
        return esp;
    }