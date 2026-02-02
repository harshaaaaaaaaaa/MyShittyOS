#ifndef __OSFILES__GUI__GRAPHICS_H
#define __OSFILES__GUI__GRAPHICS_H

#include <common/types.h>
#include <drivers/vga.h>

namespace osfiles {
namespace gui {
class Font {
public:
  static void DrawChar(drivers::VideoGraphicsArray *vga, common::int32_t x,
                       common::int32_t y, char c, common::uint8_t color);
  static void DrawString(drivers::VideoGraphicsArray *vga, common::int32_t x,
                         common::int32_t y, const char *str,
                         common::uint8_t color);
  static common::uint32_t GetCharWidth();
  static common::uint32_t GetCharHeight();
  static common::uint32_t GetStringWidth(const char *str);
};

class Widget {
protected:
  common::int32_t x, y;
  common::int32_t width, height;
  common::uint8_t backgroundColor;
  bool visible;
  bool focused;

public:
  Widget(common::int32_t x, common::int32_t y, common::int32_t w,
         common::int32_t h, common::uint8_t bgColor);
  virtual ~Widget();

  virtual void Draw(drivers::VideoGraphicsArray *vga);
  virtual void OnMouseDown(common::int32_t mx, common::int32_t my,
                           common::uint8_t button);
  virtual void OnMouseUp(common::int32_t mx, common::int32_t my,
                         common::uint8_t button);
  virtual void OnMouseMove(common::int32_t mx, common::int32_t my);
  virtual void OnKeyDown(char c);

  bool Contains(common::int32_t px, common::int32_t py);
  void SetVisible(bool v);
  bool IsVisible();
  void SetPosition(common::int32_t newX, common::int32_t newY);
  void GetPosition(common::int32_t *outX, common::int32_t *outY);
  void GetSize(common::int32_t *outW, common::int32_t *outH);
};

// Button widget
class Button : public Widget {
protected:
  const char *label;
  common::uint8_t textColor;
  common::uint8_t hoverColor;
  bool hovered;
  bool pressed;
  void (*clickHandler)();

public:
  Button(common::int32_t x, common::int32_t y, common::int32_t w,
         common::int32_t h, const char *label);
  virtual ~Button();

  virtual void Draw(drivers::VideoGraphicsArray *vga);
  virtual void OnMouseDown(common::int32_t mx, common::int32_t my,
                           common::uint8_t button);
  virtual void OnMouseUp(common::int32_t mx, common::int32_t my,
                         common::uint8_t button);
  virtual void OnMouseMove(common::int32_t mx, common::int32_t my);

  void SetClickHandler(void (*handler)());
};

// Label widget
class Label : public Widget {
protected:
  const char *text;
  common::uint8_t textColor;

public:
  Label(common::int32_t x, common::int32_t y, const char *text,
        common::uint8_t color);
  virtual ~Label();

  virtual void Draw(drivers::VideoGraphicsArray *vga);
  void SetText(const char *newText);
};

// Desktop icon
class DesktopIcon : public Widget {
protected:
  const char *label;
  common::uint8_t iconColor;
  common::uint8_t labelColor;
  bool selected;
  void (*doubleClickHandler)();
  common::uint32_t lastClickTime;

public:
  DesktopIcon(common::int32_t x, common::int32_t y, const char *label,
              common::uint8_t iconColor);
  virtual ~DesktopIcon();

  virtual void Draw(drivers::VideoGraphicsArray *vga);
  virtual void OnMouseDown(common::int32_t mx, common::int32_t my,
                           common::uint8_t button);

  void SetDoubleClickHandler(void (*handler)());
  void DrawFolderIcon(drivers::VideoGraphicsArray *vga, common::int32_t ix,
                      common::int32_t iy);
};
} // namespace gui
} // namespace osfiles

#endif
