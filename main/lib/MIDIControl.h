#pragma once
#include <Arduino.h>
// #include "MIDIUSB.h"
#include <algorithm>
#include <mutex>
#include <vector>
#include "Peripheral.h"
#include "MIDIOut.h"

#define LOCK_GUARD std::lock_guard<std::mutex> lock(mutex());

template <typename T>
class RegistryBase
{
public:
  RegistryBase() {}
  ~RegistryBase() {}

  static void add(T *obj)
  {
    LOCK_GUARD;
    instances().push_back(obj);
  }

  static void remove(T *obj)
  {
    LOCK_GUARD;
    auto &v = instances();
    v.erase(std::remove(v.begin(), v.end(), obj), v.end());
  }

  // call any function on each element
  template <typename Func>
  static void forEach(Func&& f)
  {
    LOCK_GUARD;
    for (T* obj : instances()) f(*obj);
  }

  static std::size_t count()
  {
    LOCK_GUARD;
    return instances().size();
  }

private:
  static std::vector<T*>& instances()
  {
    static std::vector<T*> v;
    return v;
  }

  static std::mutex& mutex()
  {
    static std::mutex m;
    return m;
  }
};

class MIDIController : private RegistryBase<Peripheral::PeripheralBase>
{
public:
  // Todo: duplicate checking
  void addControl(Peripheral::PeripheralBase &control);
  void removeControl(Peripheral::PeripheralBase &control);
  std::size_t controlCount() const;

  // PeripheralHandle::PeripheralBase getControl(PeripheralHandle::PeripheralID id) const
  // {
  //   for (PeripheralHandle::PeripheralBase *c : instances())
  //   {
  //     if (c->id() == id) return *c;
  //   }
  //   return nullptr;
  // }

  void setOutput(MIDIOut &midiOut) { _midiOut = &midiOut; }

  void sendMidi();
  void setup();
  void update();

private:
  MIDIOut *_midiOut = nullptr;
};
