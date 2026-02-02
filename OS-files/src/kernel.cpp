#include <common/types.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gdt.h>
#include <gui/cursor.h>
#include <gui/desktop.h>
#include <gui/graphics.h>
#include <hardwarecommunication/interrupts.h>

using namespace osfiles;
using namespace osfiles::common;
using namespace osfiles::drivers;
using namespace osfiles::hardwarecommunication;
using namespace osfiles::gui;

// C++ runtime stubs
void *operator new(unsigned int size) { return 0; }
void *operator new[](unsigned int size) { return 0; }
void operator delete(void *p) {}
void operator delete(void *p, unsigned int size) {}
void operator delete[](void *p) {}
void operator delete[](void *p, unsigned int size) {}
extern "C" void __cxa_pure_virtual() {
  while (1)
    ;
}

void printf(char *str) {
  static uint16_t *VideoMemory = (uint16_t *)0xb8000;

  static uint8_t x = 0, y = 0;

  for (int i = 0; str[i] != '\0'; ++i) {
    switch (str[i]) {
    case '\n':
      y++;
      x = 0;
      break;
    default:
      if (x < 80 && y < 25) // Add bounds checking
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
      x++;
      break;
    }

    if (x >= 80) {
      y++;
      x = 0;
    }

    if (y >= 25) {
      for (y = 0; y < 25; y++)
        for (x = 0; x < 80; x++)
          VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
      x = y = 0;
    }
  }
}

void printfHex(uint8_t key) {
  char *foo = "00";
  char *hex = "0123456789ABCDEF";
  foo[0] = hex[(key >> 4) & 0x0F];
  foo[1] = hex[key & 0x0F];
  printf(foo);
}

// Global pointers for event handlers
static VideoGraphicsArray *globalVga = 0;
static Desktop *globalDesktop = 0;
static TerminalWindow *globalTerminal = 0;
static TicTacToeWindow *globalTicTacToe = 0;
static bool graphicsModeActive = false;

// Open handlers
void openTerminal() {
  if (globalTerminal) {
    globalTerminal->SetVisible(true);
    globalTerminal->SetFocused(true);
    if (globalTicTacToe)
      globalTicTacToe->SetFocused(false);
  }
}

void openTicTacToe() {
  if (globalTicTacToe) {
    globalTicTacToe->SetVisible(true);
    globalTicTacToe->SetFocused(true);
    if (globalTerminal)
      globalTerminal->SetFocused(false);
  }
}

class GraphicalMouseHandler : public MouseEventHandler {
public:
  void OnMouseMove(int oldx, int oldy, int newx, int newy) {
    if (graphicsModeActive && globalDesktop) {
      // text coordinates to graphics coordinates
      int32_t gfxX = newx * 4; // 320/80 = 4
      int32_t gfxY = newy * 8; // 200/25 = 8
      globalDesktop->OnMouseMove(oldx * 4, oldy * 8, gfxX, gfxY);
    }
  }

  void OnMouseDown(uint8_t button) {
    if (graphicsModeActive && globalDesktop) {
      globalDesktop->OnMouseDown(button);
    }
  }

  void OnMouseUp(uint8_t button) {
    if (graphicsModeActive && globalDesktop) {
      globalDesktop->OnMouseUp(button);
    }
  }
};

class GraphicalKeyboardHandler : public KeyboardEventHandler {
public:
  void OnKeyDown(char c) {
    if (graphicsModeActive && globalDesktop) {
      globalDesktop->OnKeyDown(c);
    }
  }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors() {
  for (constructor *i = &start_ctors; i != &end_ctors; i++)
    (*i)();
}

extern "C" void __stack_chk_fail_local() {
  while (1)
    ;
} // to fix stack_chk_fail_local error fix

extern "C" void kernelMain(const void *multiboot_structure,
                           uint32_t /*multiboot_magic*/) {

  GlobalDescriptorTable gdt;

  InterruptManager interrupts(0x20, &gdt);

  DriverManager drvManager;

  GraphicalKeyboardHandler kbhandler;
  KeyboardDriver keyboard(&interrupts, &kbhandler);
  drvManager.AddDriver(&keyboard);

  GraphicalMouseHandler mousehandler;
  MouseDriver mouse(&interrupts, &mousehandler);
  drvManager.AddDriver(&mouse);

  drvManager.ActivateAll();

  // VGA graphics
  VideoGraphicsArray vga;
  globalVga = &vga;

  if (vga.SetMode(320, 200, 8)) {
    graphicsModeActive = true;
    // Cursor
    MouseCursor cursor;
    cursor.Init(320, 200);
    cursor.SetPosition(160, 100);
    Desktop desktop(&vga, &cursor);
    globalDesktop = &desktop;
    desktop.SetBackgroundColor(VGA_DESKTOP_BG);

    // Create windows
    TerminalWindow terminal(60, 30, 200, 140, "Terminal");
    globalTerminal = &terminal;
    terminal.SetVisible(false);
    desktop.SetActiveTerminal(&terminal);

    TicTacToeWindow tictactoe(150, 40);
    globalTicTacToe = &tictactoe;
    tictactoe.SetVisible(false);
    desktop.SetActiveTicTacToe(&tictactoe);

    // Create desktop icons
    DesktopIcon terminalIcon(10, 10, "Terminal", VGA_ICON_FOLDER);
    terminalIcon.SetDoubleClickHandler(openTerminal);
    desktop.AddIcon(&terminalIcon);

    DesktopIcon tttIcon(10, 70, "TicTacToe", VGA_LIGHT_BLUE);
    tttIcon.SetDoubleClickHandler(openTicTacToe);
    desktop.AddIcon(&tttIcon);

    interrupts.Activate();

    while (1) {
      vga.Clear(VGA_DESKTOP_BG);
      // Draw icons
      terminalIcon.Draw(&vga);
      tttIcon.Draw(&vga);
      // Draw windows
      if (terminal.IsVisible())
        terminal.Draw(&vga);
      if (tictactoe.IsVisible())
        tictactoe.Draw(&vga);
      // Draw cursor on top
      cursor.Draw(&vga);
      // Swap buffers
      vga.SwapBuffers();
    }
  } else {
    while (1)
      ;
  }
}
