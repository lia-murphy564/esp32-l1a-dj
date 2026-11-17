#include "MIDIControl.h"

void MIDIController::addControl(Peripheral::PeripheralBase &control)
{
  return add(&control);
}

void MIDIController::removeControl(Peripheral::PeripheralBase &control)
{
  return remove(&control);
}

std::size_t MIDIController::controlCount() const
{
  return count();
}

void MIDIController::sendMidi()
{
  if (!_midiOut) return;
  forEach([this](Peripheral::PeripheralBase &c)
  {
    c.send(*_midiOut);
  });
}

void MIDIController::setup()
{
  forEach([](Peripheral::PeripheralBase &c) { c.setup(); });
}

void MIDIController::update()
{
  forEach([](Peripheral::PeripheralBase &c) { c.update(); });
}
