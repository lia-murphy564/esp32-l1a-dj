#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplayData
{
public:
  virtual ~DisplayData() {}
  virtual void draw(Adafruit_GFX &gfx);

  void setBitmap(const uint8_t *bmp) { _bmp = bmp; }

  // TODO: Remove and write a better version
  void setLine(uint8_t i, const char *text, uint8_t size = 1);

protected:
  char    _lines[4][22] = {};
  uint8_t _sizes[22] = {1, 1, 1, 1};

private:
  const uint8_t *_bmp = nullptr;
};

class DisplayDebug : public DisplayData
{
public:
  DisplayDebug();

  void draw(Adafruit_GFX &gfx) override;

  void setButtons(bool btn1, uint8_t btn2);
  void setPot(int value);
  void setLed(bool on);
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
