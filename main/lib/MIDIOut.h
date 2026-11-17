#pragma once
#include <Arduino.h>
// #include "MIDIUSB.h"

//Todo: add port selection, for now its just serial
//Todo: add ble, usbmidi, serial select
class MIDIOut
{
public:
  MIDIOut() {}
  virtual ~MIDIOut() {}

  void controlChange(uint8_t channel, uint8_t cc, uint8_t value);
  void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
  void noteOff(uint8_t channel, uint8_t note, uint8_t velocity);

private:
  void send(uint8_t status, uint8_t data1, uint8_t data2);
};
