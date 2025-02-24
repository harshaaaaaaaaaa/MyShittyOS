#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

using namespace osfiles;
using namespace osfiles::common;
using namespace osfiles::drivers;
using namespace osfiles::hardwarecommunication;

void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x = 0, y = 0;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        switch (str[i])
        {
            case '\n':
                y++;
                x = 0;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if (x >= 80)
        {
            y++;
            x = 0;
        }

        if (y >= 25)
        {
            for (y = 0; y < 25; y++)
                for (x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = y = 0;
        }
    }
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0x0F];
    foo[1] = hex[key & 0x0F];
    printf(foo);
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char* foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:
    void OnMouseMove(int oldx, int oldy, int x, int y)
    {
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[80*oldy+oldx] = ((VideoMemory[80*oldy+oldx] & 0xF000) >> 4)
                                 | ((VideoMemory[80*oldy+oldx] & 0x0F00) << 4)
                                 | ((VideoMemory[80*oldy+oldx] & 0x00FF));

        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                            | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                            | ((VideoMemory[80*y+x] & 0x00FF));
    }
};





typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

extern "C" void __stack_chk_fail_local() {
    while (1);
}//to fix stack_chk_fail_local error fix

extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("BINGO!!! Shit's booted fr...\n");
    printf("Next video? naa!\n");

    GlobalDescriptorTable gdt;

    InterruptManager interrupts(0x20, &gdt);
    printf("Initializing h/w 1\n");

    DriverManager drvManager;

        PrintfKeyboardEventHandler kbhandler;
        KeyboardDriver keyboard(&interrupts, &kbhandler);
        drvManager.AddDriver(&keyboard);

        MouseDriver mouse(&interrupts);
        drvManager.AddDriver(&mouse);

        printf("Initializing h/w 2\n");
        drvManager.ActivateAll();
    
        if (keyboard.IsActivated())
        {
            printf("Keyboard driver is activated.\n");
        }
        else
        {
            printf("Keyboard driver failed to activate.\n");
        }
        
    printf("Initializing h/w 3\n");
    interrupts.Activate();
    
    while (1);
}
