#include "Display.h"
#include <cstdio>
#include <cstring>

void DisplayData::draw(Adafruit_GFX &gfx)
{
  if (_bmp)
    gfx.drawBitmap(0, 0, _bmp, gfx.width(), gfx.height(), 1);
}

void DisplayData::setLine(uint8_t i, const char *text, uint8_t size)
{
  if (i >= 4) return;
  strncpy(_lines[i], text, 22 - 1);
  _lines[i][22 - 1] = '\0';
  _sizes[i] = size;
}

DisplayDebug::DisplayDebug()
{
  setLine(0, "L1A - DEBUG");
}

void DisplayDebug::draw(Adafruit_GFX &gfx)
{
  gfx.setTextColor(1);
  int16_t y = 0;
  for (uint8_t i = 0; i < 4; i++) {
    if (_lines[i][0] == '\0') continue;
    gfx.setTextSize(_sizes[i]);
    gfx.setCursor(0, y);
    gfx.print(_lines[i]);
    y += 8 * _sizes[i];
  }
}

void DisplayDebug::setButtons(bool btn1, bool btn2)
{
  char buf[22];
  snprintf(buf, sizeof(buf), "BTN1:%d  BTN2:%d", btn1, btn2);
  setLine(1, buf);
}

void DisplayDebug::setPot(int value)
{
  char buf[22];
  snprintf(buf, sizeof(buf), "POT:%d", value);
  setLine(2, buf);
}

void DisplayDebug::setLed(bool on)
{
  setLine(3, on ? "LED:ON" : "LED:OFF");
}
