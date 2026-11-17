#pragma once
#include <cstdint>

namespace fake_arduino { inline uint32_t now_ms = 0; }
inline uint32_t millis() { return fake_arduino::now_ms; }