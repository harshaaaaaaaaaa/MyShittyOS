#ifndef __OSFILES__GUI__DESKTOP_H
#define __OSFILES__GUI__DESKTOP_H

#include <common/types.h>
#include <drivers/vga.h>
#include <gui/cursor.h>
#include <gui/graphics.h>

namespace osfiles {
namespace gui {
#define MAX_DESKTOP_ICONS 8
#define TERMINAL_BUFFER_SIZE 512
#define TERMINAL_MAX_LINES 10
#define TERMINAL_LINE_WIDTH 24

// Forward declarations
class TerminalWindow;
class TicTacToeWindow;

class Desktop {
protected:
  drivers::VideoGraphicsArray *vga;
  MouseCursor *cursor;

  common::uint8_t backgroundColor;
  common::int32_t width, height;

  // icons
  DesktopIcon *icons[MAX_DESKTOP_ICONS];
  common::int32_t iconCount;

  // Mouse state
  common::int32_t mouseX, mouseY;
  common::uint8_t mouseButtons;

  // Window dragging
  bool draggingWindow;
  common::int32_t dragOffsetX, dragOffsetY;

  // Icon dragging
  bool draggingIcon;
  common::int32_t draggingIconIndex;

  // Active windows
  TerminalWindow *activeTerminal;
  TicTacToeWindow *activeTicTacToe;

public:
  Desktop(drivers::VideoGraphicsArray *vga, MouseCursor *cursor);
  ~Desktop();

  void SetBackgroundColor(common::uint8_t color);
  void AddIcon(DesktopIcon *icon);
  void SetActiveTerminal(TerminalWindow *terminal);
  void SetActiveTicTacToe(TicTacToeWindow *ttt);

  void Draw();
  void Redraw();

  void OnMouseDown(common::uint8_t button);
  void OnMouseUp(common::uint8_t button);
  void OnMouseMove(common::int32_t oldX, common::int32_t oldY,
                   common::int32_t newX, common::int32_t newY);
  void OnKeyDown(char c);

  common::int32_t GetMouseX();
  common::int32_t GetMouseY();

  drivers::VideoGraphicsArray *GetVGA();
  MouseCursor *GetCursor();
};

class TerminalWindow {
protected:
  common::int32_t x, y;
  common::int32_t width, height;
  common::int32_t titleBarHeight;

  common::uint8_t bgColor;
  common::uint8_t fgColor;
  common::uint8_t titleBarColor;

  const char *title;

  char outputLines[TERMINAL_MAX_LINES][TERMINAL_LINE_WIDTH];
  common::int32_t outputLineCount;

  char inputBuffer[64];
  common::int32_t inputPos;

  bool visible;
  bool focused;

  void AddOutputLine(const char *text);
  void ProcessCommand(const char *cmd);

  // terminal
public:
  TerminalWindow(common::int32_t x, common::int32_t y, common::int32_t w,
                 common::int32_t h, const char *title);
  ~TerminalWindow();

  void Draw(drivers::VideoGraphicsArray *vga);
  void OnKeyDown(char c);
  void Clear();

  void SetVisible(bool v);
  bool IsVisible();
  void SetFocused(bool f);
  bool IsFocused();

  void SetPosition(common::int32_t newX, common::int32_t newY);
  void GetPosition(common::int32_t *outX, common::int32_t *outY);

  bool Contains(common::int32_t px, common::int32_t py);
  bool ContainsTitleBar(common::int32_t px, common::int32_t py);
  bool ContainsCloseButton(common::int32_t px, common::int32_t py);
};
// TicTacToe
class TicTacToeWindow {
protected:
  common::int32_t x, y;
  common::int32_t width, height;
  common::int32_t titleBarHeight;

  common::uint8_t board[9];
  common::uint8_t currentPlayer;
  common::uint8_t winner;
  bool gameOver;

  bool visible;
  bool focused;

  void CheckWin();

public:
  TicTacToeWindow(common::int32_t x, common::int32_t y);
  ~TicTacToeWindow();

  void Draw(drivers::VideoGraphicsArray *vga);
  void OnMouseDown(common::int32_t mx, common::int32_t my);
  void Reset();

  void SetVisible(bool v);
  bool IsVisible();
  void SetFocused(bool f);
  bool IsFocused();

  void SetPosition(common::int32_t newX, common::int32_t newY);
  void GetPosition(common::int32_t *outX, common::int32_t *outY);

  bool Contains(common::int32_t px, common::int32_t py);
  bool ContainsTitleBar(common::int32_t px, common::int32_t py);
  bool ContainsCloseButton(common::int32_t px, common::int32_t py);
};
} // namespace gui
} // namespace osfiles

#endif
