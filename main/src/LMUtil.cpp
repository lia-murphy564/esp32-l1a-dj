#include "LMUtil.h"

namespace LMUtil
{

int add(int a, int b)
{
  return a + b;
}

bool Debouncer::debounce(bool in)
{
  uint32_t now = millis();
  if (in != raw) { raw = in; since = now; }     // any change restarts the timer
  if (in == stable || now - since < ms) return false;
  stable = in;
  return true;
}

}
