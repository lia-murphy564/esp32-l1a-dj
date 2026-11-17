#include "Communication.h"
#include <WiFi.h>

uint8_t broadcastAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // placeholder

ComLink_ESPNow::ComLink_ESPNow(ComRole role)
  : role(role) {}

void ComLink_ESPNow::begin()
{
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW init failed");
    return;
  }

  if (role == ComRole::Hub)
  {
    queue = xQueueCreate(32, sizeof(Event));
    esp_now_register_recv_cb(onReceive);
    // Serial.println("Hub ready");
  }

  if (role == ComRole::Module)
  {
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, broadcastAddr, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);
  }
}

void ComLink_ESPNow::update()
{
  // if (role == ComRole::Hub && queue)
  // {
  //   Event e;
  //   while (xQueueReceive(queue, &e, 0))
  //   {
  //     Serial.printf("module ..%02X:%02X  type %u  ctl %u  value %u\n",
  //     e.mac[4], e.mac[5], e.msg.type, e.msg.control, e.msg.value);
  //   }
  // }

  // if (role == ComRole::Module)
  // {
  //   send(10, 11, 12); // example
  //   delay(100);
  // }
  return;
}

void ComLink_ESPNow::send(uint8_t type, uint8_t control, uint16_t value)
{
  if (role != ComRole::Module) return;
  ControlMsg msg = {type, control, value};
  esp_now_send(broadcastAddr, (uint8_t*)&msg, sizeof(msg));
}

bool ComLink_ESPNow::receive(Event &e)
{
  if (role != ComRole::Hub || !queue)
    return false;
  return xQueueReceive(queue, &e, 0) == pdTRUE;
}

void ComLink_ESPNow::onReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len)
{
  if (len != sizeof(ControlMsg)) return;
  Event e;
  memcpy(e.mac, info->src_addr, 6);
  memcpy(&e.msg, data, sizeof(ControlMsg));
  xQueueSend(queue, &e, 0);
}
