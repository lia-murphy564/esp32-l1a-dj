#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


class DisplayData
{
public:
  virtual ~DisplayData() {};
  virtual void draw(Adafruit_GFX &gfx) 
  {
    if (_bmp)
        gfx.drawBitmap(0, 0, _bmp, gfx.width(), gfx.height(), 1);
  };

  void setBitmap(const uint8_t *bmp) { _bmp = bmp; }

  // TODO: Remove and write a better version
  void setLine(uint8_t i, const char *text, uint8_t size = 1)
  {
    if (i >= 4) return;
    strncpy(_lines[i], text, 22 - 1);
    _lines[i][22 - 1] = '\0';
    _sizes[i] = size;
  }

protected:
  char    _lines[4][22] = {};
  uint8_t _sizes[22] = {1, 1, 1, 1};


private:
  const uint8_t *_bmp = nullptr;
};

class DisplayDebug : public DisplayData
{
public:
  DisplayDebug() { setLine(0, "L1A - DEBUG"); }

  void draw(Adafruit_GFX &gfx) override
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

  void setButtons(bool btn1, bool btn2)
  {
    char buf[22];
    snprintf(buf, sizeof(buf), "BTN1:%d  BTN2:%d", btn1, btn2);
    setLine(1, buf);
  }

  void setPot(int value)
  {
    char buf[22];
    snprintf(buf, sizeof(buf), "POT:%d", value);
    setLine(2, buf);
  }

  void setLed(bool on)
  {
    setLine(3, on ? "LED:ON" : "LED:OFF");
  }
};

// class BitmapDisplay : public DisplayData
// {
// public:
//   explicit BitmapDisplay(const uint8_t *bmp) : _bmp(bmp) {}

// //   void setBitmap(const uint8_t *bmp) { _bmp = bmp; }

//   void draw(Adafruit_GFX &gfx) override
//   {
//     gfx.drawBitmap(0, 0, _bmp, gfx.width(), gfx.height(), 1);
//   }

// };