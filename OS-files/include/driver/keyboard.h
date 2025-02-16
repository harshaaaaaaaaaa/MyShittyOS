#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"
#include "driver.h"
#include "interrupts.h"
#include "port.h"

class KeyboardEventHandler
{
public:
    KeyboardEventHandler();
    ~KeyboardEventHandler();

    virtual void OnKeyDown(char);
    virtual void OnKeyUp(char);
};

class KeyboardDriver : public InterruptHandler, public Driver
{
    Port8Bit dataport;
    Port8Bit commandport;

    KeyboardEventHandler* handler;
    bool activated; // Add this line

public:
    KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler);
    ~KeyboardDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void Activate();
    bool IsActivated(); // Add this line
};

#endif