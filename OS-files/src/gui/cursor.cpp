#include <gui/cursor.h>

using namespace osfiles::common;
using namespace osfiles::drivers;
using namespace osfiles::gui;

static const uint8_t cursorBitmap[12][8] = {
    {1, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 0, 0, 0},
    {1, 2, 1, 0, 0, 0, 0, 0}, {1, 2, 2, 1, 0, 0, 0, 0},
    {1, 2, 2, 2, 1, 0, 0, 0}, {1, 2, 2, 2, 2, 1, 0, 0},
    {1, 2, 2, 2, 2, 2, 1, 0}, {1, 2, 2, 1, 1, 1, 0, 0},
    {1, 2, 1, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};

static const int CURSOR_WIDTH = 8;
static const int CURSOR_HEIGHT = 12;

MouseCursor::MouseCursor() {
  x = 0;
  y = 0;
  hotspotX = 0;
  hotspotY = 0;
  backgroundSaved = false;
  savedX = 0;
  savedY = 0;
  visible = true;
  screenWidth = 320;
  screenHeight = 200;

  for (int i = 0; i < 16 * 16; i++)
    savedBackground[i] = 0;
}

MouseCursor::~MouseCursor() {}

void MouseCursor::Init(int32_t screenW, int32_t screenH) {
  screenWidth = screenW;
  screenHeight = screenH;
  x = screenW / 2;
  y = screenH / 2;
}

void MouseCursor::SetPosition(int32_t newX, int32_t newY) {
  // Clamp to screen bounds
  if (newX < 0)
    newX = 0;
  if (newX >= screenWidth)
    newX = screenWidth - 1;
  if (newY < 0)
    newY = 0;
  if (newY >= screenHeight)
    newY = screenHeight - 1;

  x = newX;
  y = newY;
}

void MouseCursor::GetPosition(int32_t *outX, int32_t *outY) {
  *outX = x;
  *outY = y;
}

void MouseCursor::Hide(VideoGraphicsArray *vga) {
  if (!backgroundSaved || !visible)
    return;

  // Restore the saved background
  for (int row = 0; row < CURSOR_HEIGHT; row++) {
    for (int col = 0; col < CURSOR_WIDTH; col++) {
      int32_t px = savedX + col;
      int32_t py = savedY + row;

      if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
        if (cursorBitmap[row][col] != 0) {
          vga->PutPixel(px, py, savedBackground[row * CURSOR_WIDTH + col]);
        }
      }
    }
  }

  backgroundSaved = false;
}

void MouseCursor::Show(VideoGraphicsArray *vga) {
  if (!visible)
    return;

  Draw(vga);
}

void MouseCursor::Draw(VideoGraphicsArray *vga) {
  if (!visible)
    return;

  int32_t drawX = x - hotspotX;
  int32_t drawY = y - hotspotY;

  // Save background first
  for (int row = 0; row < CURSOR_HEIGHT; row++) {
    for (int col = 0; col < CURSOR_WIDTH; col++) {
      int32_t px = drawX + col;
      int32_t py = drawY + row;

      if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
        savedBackground[row * CURSOR_WIDTH + col] = vga->GetPixel(px, py);
      }
    }
  }
  savedX = drawX;
  savedY = drawY;
  backgroundSaved = true;

  // Draw cursor
  for (int row = 0; row < CURSOR_HEIGHT; row++) {
    for (int col = 0; col < CURSOR_WIDTH; col++) {
      int32_t px = drawX + col;
      int32_t py = drawY + row;

      if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
        uint8_t pixel = cursorBitmap[row][col];
        if (pixel == 1) {
          vga->PutPixel(px, py, VGA_BLACK);
        } else if (pixel == 2) {
          vga->PutPixel(px, py, VGA_WHITE);
        }
        // 0 = transparent, don't draw
      }
    }
  }
}

void MouseCursor::SetVisible(bool v) { visible = v; }

bool MouseCursor::IsVisible() { return visible; }
