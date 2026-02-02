#ifndef __OSFILES__GUI__CURSOR_H
#define __OSFILES__GUI__CURSOR_H

#include <common/types.h>
#include <drivers/vga.h>

namespace osfiles {
namespace gui {
class MouseCursor {
protected:
  common::int32_t x, y;
  common::int32_t hotspotX, hotspotY;
  common::uint8_t savedBackground[16 * 16];
  common::int32_t savedX, savedY;
  bool backgroundSaved;
  bool visible;
  common::int32_t screenWidth, screenHeight;

public:
  MouseCursor();
  ~MouseCursor();

  void Init(common::int32_t screenW, common::int32_t screenH);
  void SetPosition(common::int32_t newX, common::int32_t newY);
  void GetPosition(common::int32_t *outX, common::int32_t *outY);

  void Hide(drivers::VideoGraphicsArray *vga);
  void Show(drivers::VideoGraphicsArray *vga);
  void Draw(drivers::VideoGraphicsArray *vga);

  void SetVisible(bool v);
  bool IsVisible();
};
} // namespace gui
} // namespace osfiles

#endif
