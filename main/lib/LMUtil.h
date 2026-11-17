#ifndef LMUTIL_H
#define LMUTIL_H

#include <type_traits>
#include <Arduino.h>

namespace LMUtil
{
  int add(int a, int b);

  template <typename T>
  constexpr T clamp(T v, T l, T h) {
    return v < l ? l : (h < v ? h : v);
  }

  template <typename T>
  constexpr int32_t lerp(T a, T b, T t) {
    static_assert(std::is_integral_v<T>, "clamp_midi needs an integer type");
    return static_cast<T>(a + (((b - a) * t) >> 8));
  }

  constexpr float lerpf(float a, float b, float t) {
    return a + t * (b - a);
  }

  template <typename T>
  constexpr T ramp(T curr, T target, T step) {
      if (curr < target) {
          return (target - curr > step) ? curr + step : target;
      }
      if (curr > target) {
          return (curr - target > step) ? curr - step : target;
      }
      return curr;
  }

  template <typename T>
  constexpr T gamma(T v)
  {
    return (T)v * v * v / (255 * 255);
  }

  // Cubic brightness curve for 8-bit values (0..255 -> 0..255)
  constexpr uint8_t gamma8(uint8_t v) {
    return static_cast<uint8_t>((uint32_t(v) * v * v + 32512) / 65025);  // +half for rounding
  }

  // Returns true once, when the input has held a new state for `ms`.
  struct Debouncer
  {
    uint16_t ms = 20;
    bool raw = false;
    bool stable = false;
    uint32_t since = 0;

    bool debounce(bool in);
  };
}

#endif // LMUTIL_H
