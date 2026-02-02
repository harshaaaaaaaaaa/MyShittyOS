#ifndef __OSFILES__DRIVERS__VGA_H
#define __OSFILES__DRIVERS__VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>

namespace osfiles {
namespace drivers {
// Standard 16 colors for compatibility
enum VGAColor {
  VGA_BLACK = 0,
  VGA_BLUE = 1,
  VGA_GREEN = 2,
  VGA_CYAN = 3,
  VGA_RED = 4,
  VGA_MAGENTA = 5,
  VGA_BROWN = 6,
  VGA_LIGHT_GRAY = 7,
  VGA_DARK_GRAY = 8,
  VGA_LIGHT_BLUE = 9,
  VGA_LIGHT_GREEN = 10,
  VGA_LIGHT_CYAN = 11,
  VGA_LIGHT_RED = 12,
  VGA_LIGHT_MAGENTA = 13,
  VGA_YELLOW = 14,
  VGA_WHITE = 15,

  // colors for desktop/GUI
  VGA_DESKTOP_BG = 16,
  VGA_WINDOW_BG = 17,
  VGA_WINDOW_BORDER = 18,
  VGA_TITLEBAR = 19,
  VGA_TITLEBAR_TEXT = 20,
  VGA_BUTTON = 21,
  VGA_BUTTON_HOVER = 22,
  VGA_TERMINAL_BG = 23,
  VGA_TERMINAL_FG = 24,
  VGA_ICON_FOLDER = 25,
  VGA_CURSOR_COLOR = 26
};

class VideoGraphicsArray {
protected:
  hardwarecommunication::Port8Bit miscPort;
  hardwarecommunication::Port8Bit crtcIndexPort;
  hardwarecommunication::Port8Bit crtcDataPort;
  hardwarecommunication::Port8Bit sequencerIndexPort;
  hardwarecommunication::Port8Bit sequencerDataPort;
  hardwarecommunication::Port8Bit graphicsControllerIndexPort;
  hardwarecommunication::Port8Bit graphicsControllerDataPort;
  hardwarecommunication::Port8Bit attributeControllerIndexPort;
  hardwarecommunication::Port8Bit attributeControllerReadPort;
  hardwarecommunication::Port8Bit attributeControllerWritePort;
  hardwarecommunication::Port8Bit attributeControllerResetPort;

  common::uint8_t *framebuffer;
  common::uint8_t *backbuffer;

  common::uint32_t screenWidth;
  common::uint32_t screenHeight;
  common::uint8_t colorDepth;

  bool graphicsMode;

  void WriteRegisters(common::uint8_t *registers);
  common::uint8_t *GetFrameBufferSegment();
  void SetPalette();

public:
  VideoGraphicsArray();
  ~VideoGraphicsArray();

  bool SupportsMode(common::uint32_t width, common::uint32_t height,
                    common::uint32_t colordepth);
  bool SetMode(common::uint32_t width, common::uint32_t height,
               common::uint32_t colordepth);
  void SetTextMode();

  common::uint32_t GetWidth();
  common::uint32_t GetHeight();

  void PutPixel(common::int32_t x, common::int32_t y, common::uint8_t color);
  common::uint8_t GetPixel(common::int32_t x, common::int32_t y);

  void DrawLine(common::int32_t x1, common::int32_t y1, common::int32_t x2,
                common::int32_t y2, common::uint8_t color);
  void DrawRect(common::int32_t x, common::int32_t y, common::int32_t w,
                common::int32_t h, common::uint8_t color);
  void FillRect(common::int32_t x, common::int32_t y, common::int32_t w,
                common::int32_t h, common::uint8_t color);

  void Clear(common::uint8_t color);
  void SwapBuffers();

  bool IsGraphicsMode();
};
} // namespace drivers
} // namespace osfiles

#endif
