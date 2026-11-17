#include "Data.h"

StateBase::StateBase(uint8_t id)
  : _id(id) {}

void ButtonState::set(bool pressed)
{
  _pressed = pressed;
  markChanged();
}
