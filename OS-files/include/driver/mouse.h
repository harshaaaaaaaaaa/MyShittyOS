#ifndef __MOUSE_H
#define __MOUSE_H

#include "types.h"
#include "driver.h"
#include "interrupts.h"
#include "port.h"

class MouseEventHandler
{
public:
    MouseEventHandler();
    ~MouseEventHandler();

    virtual void OnActivate();
    virtual void OnMouseDown(uint8_t button);
    virtual void OnMouseUp(uint8_t button);
    virtual void OnMouseMove(int oldx, int oldy, int x, int y);
};

class MouseDriver : public InterruptHandler, public Driver
{
    Port8Bit dataport;
    Port8Bit commandport;

    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;
    
    MouseEventHandler* handler;
    int8_t x, y;

public:
    MouseDriver(InterruptManager* manager, MouseEventHandler* handler);
    ~MouseDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void Activate();
};

#endif