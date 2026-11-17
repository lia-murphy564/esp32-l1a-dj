#pragma once
#include <Arduino.h>
#include <esp_now.h>

uint8_t broadcastAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // placeholder

struct __attribute__((packed)) ControlMsg {
  uint8_t  type;
  uint8_t  control;
  uint16_t value;
};

struct Event {
  uint8_t    mac[6]; // sender
  ControlMsg msg;
};

enum class ComRole { Hub, Module };

class ComLink
{
public:
  virtual ~ComLink() = default;

  virtual void begin() = 0;
  virtual void update() = 0;
  virtual void send(uint8_t type, uint8_t control, uint16_t value) = 0;
  virtual bool receive(Event &e) = 0;

  // void send(uint8_t type, uint8_t control, uint16_t value) {
  //   return send(ControlMsg{type, control, value});
  // }
};

class ComLink_ESPNow : public ComLink
{
public:
  explicit ComLink_ESPNow(ComRole role);

  void begin() override;
  void update() override;
  void send(uint8_t type, uint8_t control, uint16_t value) override;
  bool receive(Event &e) override;

private:
  static void onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len);

  ComRole role;
  inline static QueueHandle_t queue = nullptr;
};
