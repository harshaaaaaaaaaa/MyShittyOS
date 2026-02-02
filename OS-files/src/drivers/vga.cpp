#include <drivers/vga.h>

using namespace osfiles::common;
using namespace osfiles::drivers;
using namespace osfiles::hardwarecommunication;

// Mode 13h register values (320x200, 256 colors)
static uint8_t g_320x200x256[] = {
    /*MISC*/
    0x63,
    /*SEQ*/
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    /*CRTC*/
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 0x41, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    /*GC*/
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
    /*AC*/
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00, 0x0F, 0x00, 0x00};

// Static backbuffer allocation (320*200 = 64000 bytes)
static uint8_t staticBackbuffer[320 * 200];

VideoGraphicsArray::VideoGraphicsArray()
    : miscPort(0x3C2), crtcIndexPort(0x3D4), crtcDataPort(0x3D5),
      sequencerIndexPort(0x3C4), sequencerDataPort(0x3C5),
      graphicsControllerIndexPort(0x3CE), graphicsControllerDataPort(0x3CF),
      attributeControllerIndexPort(0x3C0), attributeControllerReadPort(0x3C1),
      attributeControllerWritePort(0x3C0), attributeControllerResetPort(0x3DA) {
  framebuffer = 0;
  backbuffer = staticBackbuffer;
  screenWidth = 0;
  screenHeight = 0;
  colorDepth = 0;
  graphicsMode = false;
}

VideoGraphicsArray::~VideoGraphicsArray() {}

void VideoGraphicsArray::WriteRegisters(uint8_t *registers) {
  // MISC
  miscPort.Write(*(registers++));

  // SEQ
  for (uint8_t i = 0; i < 5; i++) {
    sequencerIndexPort.Write(i);
    sequencerDataPort.Write(*(registers++));
  }

  // Unlock CRTC registers
  crtcIndexPort.Write(0x03);
  crtcDataPort.Write(crtcDataPort.Read() | 0x80);
  crtcIndexPort.Write(0x11);
  crtcDataPort.Write(crtcDataPort.Read() & ~0x80);

  // Make sure they remain unlocked
  registers[0x03] = registers[0x03] | 0x80;
  registers[0x11] = registers[0x11] & ~0x80;

  // CRTC
  for (uint8_t i = 0; i < 25; i++) {
    crtcIndexPort.Write(i);
    crtcDataPort.Write(*(registers++));
  }

  // GC
  for (uint8_t i = 0; i < 9; i++) {
    graphicsControllerIndexPort.Write(i);
    graphicsControllerDataPort.Write(*(registers++));
  }

  // AC
  for (uint8_t i = 0; i < 21; i++) {
    attributeControllerResetPort.Read();
    attributeControllerIndexPort.Write(i);
    attributeControllerWritePort.Write(*(registers++));
  }

  attributeControllerResetPort.Read();
  attributeControllerIndexPort.Write(0x20);
}

uint8_t *VideoGraphicsArray::GetFrameBufferSegment() {
  graphicsControllerIndexPort.Write(0x06);
  uint8_t segmentNumber = graphicsControllerDataPort.Read() & (3 << 2);
  switch (segmentNumber) {
  default:
  case 0 << 2:
    return (uint8_t *)0x00000;
  case 1 << 2:
    return (uint8_t *)0xA0000;
  case 2 << 2:
    return (uint8_t *)0xB0000;
  case 3 << 2:
    return (uint8_t *)0xB8000;
  }
}

void VideoGraphicsArray::SetPalette() {
  Port8Bit paletteIndexPort(0x3C8);
  Port8Bit paletteDataPort(0x3C9);

  // Set up custom palette
  paletteIndexPort.Write(0);

  // Standard 16 colors (indices 0-15)
  // Black
  paletteDataPort.Write(0);
  paletteDataPort.Write(0);
  paletteDataPort.Write(0);
  // Blue
  paletteDataPort.Write(0);
  paletteDataPort.Write(0);
  paletteDataPort.Write(42);
  // Green
  paletteDataPort.Write(0);
  paletteDataPort.Write(42);
  paletteDataPort.Write(0);
  // Cyan
  paletteDataPort.Write(0);
  paletteDataPort.Write(42);
  paletteDataPort.Write(42);
  // Red
  paletteDataPort.Write(42);
  paletteDataPort.Write(0);
  paletteDataPort.Write(0);
  // Magenta
  paletteDataPort.Write(42);
  paletteDataPort.Write(0);
  paletteDataPort.Write(42);
  // Brown
  paletteDataPort.Write(42);
  paletteDataPort.Write(21);
  paletteDataPort.Write(0);
  // Light Gray
  paletteDataPort.Write(42);
  paletteDataPort.Write(42);
  paletteDataPort.Write(42);
  // Dark Gray
  paletteDataPort.Write(21);
  paletteDataPort.Write(21);
  paletteDataPort.Write(21);
  // Light Blue
  paletteDataPort.Write(21);
  paletteDataPort.Write(21);
  paletteDataPort.Write(63);
  // Light Green
  paletteDataPort.Write(21);
  paletteDataPort.Write(63);
  paletteDataPort.Write(21);
  // Light Cyan
  paletteDataPort.Write(21);
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);
  // Light Red
  paletteDataPort.Write(63);
  paletteDataPort.Write(21);
  paletteDataPort.Write(21);
  // Light Magenta
  paletteDataPort.Write(63);
  paletteDataPort.Write(21);
  paletteDataPort.Write(63);
  // Yellow
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);
  paletteDataPort.Write(21);
  // White
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);

  // Custom GUI colors (indices 16-26)
  // VGA_DESKTOP_BG (16) - Dark blue-gray
  paletteDataPort.Write(10);
  paletteDataPort.Write(15);
  paletteDataPort.Write(25);
  // VGA_WINDOW_BG (17) - Light gray
  paletteDataPort.Write(50);
  paletteDataPort.Write(50);
  paletteDataPort.Write(50);
  // VGA_WINDOW_BORDER (18) - Dark gray
  paletteDataPort.Write(25);
  paletteDataPort.Write(25);
  paletteDataPort.Write(25);
  // VGA_TITLEBAR (19) - Blue
  paletteDataPort.Write(15);
  paletteDataPort.Write(25);
  paletteDataPort.Write(50);
  // VGA_TITLEBAR_TEXT (20) - White
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);
  // VGA_BUTTON (21) - Gray
  paletteDataPort.Write(40);
  paletteDataPort.Write(40);
  paletteDataPort.Write(40);
  // VGA_BUTTON_HOVER (22) - Light gray
  paletteDataPort.Write(48);
  paletteDataPort.Write(48);
  paletteDataPort.Write(48);
  // VGA_TERMINAL_BG (23) - Dark terminal background
  paletteDataPort.Write(5);
  paletteDataPort.Write(5);
  paletteDataPort.Write(8);
  // VGA_TERMINAL_FG (24) - Green terminal text
  paletteDataPort.Write(0);
  paletteDataPort.Write(55);
  paletteDataPort.Write(0);
  // VGA_ICON_FOLDER (25) - Folder yellow
  paletteDataPort.Write(60);
  paletteDataPort.Write(55);
  paletteDataPort.Write(20);
  // VGA_CURSOR_COLOR (26) - Cursor white
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);
  paletteDataPort.Write(63);
}

bool VideoGraphicsArray::SupportsMode(uint32_t width, uint32_t height,
                                      uint32_t colordepth) {
  return width == 320 && height == 200 && colordepth == 8;
}

bool VideoGraphicsArray::SetMode(uint32_t width, uint32_t height,
                                 uint32_t colordepth) {
  if (!SupportsMode(width, height, colordepth))
    return false;

  WriteRegisters(g_320x200x256);
  framebuffer = GetFrameBufferSegment();

  screenWidth = width;
  screenHeight = height;
  colorDepth = colordepth;
  graphicsMode = true;

  SetPalette();
  Clear(VGA_BLACK);

  return true;
}

void VideoGraphicsArray::SetTextMode() {
  // This would reset to text mode - not implemented for now
  graphicsMode = false;
}

uint32_t VideoGraphicsArray::GetWidth() { return screenWidth; }

uint32_t VideoGraphicsArray::GetHeight() { return screenHeight; }

void VideoGraphicsArray::PutPixel(int32_t x, int32_t y, uint8_t color) {
  if (x < 0 || (uint32_t)x >= screenWidth || y < 0 ||
      (uint32_t)y >= screenHeight)
    return;

  backbuffer[screenWidth * y + x] = color;
}

uint8_t VideoGraphicsArray::GetPixel(int32_t x, int32_t y) {
  if (x < 0 || (uint32_t)x >= screenWidth || y < 0 ||
      (uint32_t)y >= screenHeight)
    return 0;

  return backbuffer[screenWidth * y + x];
}

void VideoGraphicsArray::DrawLine(int32_t x1, int32_t y1, int32_t x2,
                                  int32_t y2, uint8_t color) {
  // Bresenham's line algorithm
  int32_t dx = x2 - x1;
  int32_t dy = y2 - y1;

  int32_t ax = (dx < 0) ? -dx : dx;
  int32_t ay = (dy < 0) ? -dy : dy;

  int32_t sx = (dx < 0) ? -1 : 1;
  int32_t sy = (dy < 0) ? -1 : 1;

  if (ax > ay) {
    int32_t err = ax / 2;
    while (x1 != x2) {
      PutPixel(x1, y1, color);
      err -= ay;
      if (err < 0) {
        y1 += sy;
        err += ax;
      }
      x1 += sx;
    }
  } else {
    int32_t err = ay / 2;
    while (y1 != y2) {
      PutPixel(x1, y1, color);
      err -= ax;
      if (err < 0) {
        x1 += sx;
        err += ay;
      }
      y1 += sy;
    }
  }
  PutPixel(x2, y2, color);
}

void VideoGraphicsArray::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h,
                                  uint8_t color) {
  DrawLine(x, y, x + w - 1, y, color);                 // Top
  DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color); // Bottom
  DrawLine(x, y, x, y + h - 1, color);                 // Left
  DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color); // Right
}

void VideoGraphicsArray::FillRect(int32_t x, int32_t y, int32_t w, int32_t h,
                                  uint8_t color) {
  for (int32_t j = y; j < y + h; j++)
    for (int32_t i = x; i < x + w; i++)
      PutPixel(i, j, color);
}

void VideoGraphicsArray::Clear(uint8_t color) {
  for (uint32_t i = 0; i < screenWidth * screenHeight; i++)
    backbuffer[i] = color;
}

void VideoGraphicsArray::SwapBuffers() {
  for (uint32_t i = 0; i < screenWidth * screenHeight; i++)
    framebuffer[i] = backbuffer[i];
}

bool VideoGraphicsArray::IsGraphicsMode() { return graphicsMode; }
