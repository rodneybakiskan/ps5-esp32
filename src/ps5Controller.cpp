#include "ps5Controller.h"

#include <esp_bt_defs.h>
#include <esp_bt_main.h>

extern "C" {
#include "ps5.h"
}

#define ESP_BD_ADDR_HEX_PTR(addr) \
  (uint8_t*)addr + 0, (uint8_t*)addr + 1, (uint8_t*)addr + 2, \
  (uint8_t*)addr + 3, (uint8_t*)addr + 4, (uint8_t*)addr + 5

ps5Controller::ps5Controller() {}

bool ps5Controller::begin() {
  ps5SetEventObjectCallback(this, &ps5Controller::_event_callback);
  ps5SetConnectionObjectCallback(this, &ps5Controller::_connection_callback);

  if (!btStarted() && !btStart()) {
    log_e("btStart failed");
    return false;
  }

  esp_bluedroid_status_t btState = esp_bluedroid_get_status();
  if (btState == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
    if (esp_bluedroid_init()) {
      log_e("esp_bluedroid_init failed");
      return false;
    }
  }

  if (btState != ESP_BLUEDROID_STATUS_ENABLED) {
    if (esp_bluedroid_enable()) {
      log_e("esp_bluedroid_enable failed");
      return false;
    }
  }

  ps5Init();
  return true;
}

bool ps5Controller::begin(const char* mac) {
  esp_bd_addr_t addr;
    
  if (sscanf(mac, ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX_PTR(addr)) != ESP_BD_ADDR_LEN) {
    log_e("Could not convert %s\n to a MAC address", mac);
    return false;
  }

  ps5_l2cap_connect(addr);
  // ps5SetBluetoothMacAddress(addr);

  return begin();
}

void ps5Controller::end() {}

bool ps5Controller::isConnected() {
  auto connected = ps5IsConnected();
  static unsigned long tryReconnectAt = 0;
  if (!connected && millis() - tryReconnectAt > 5000UL) {
    tryReconnectAt = millis();
    ps5_l2cap_reconnect();
  }
  return connected;
}

void ps5Controller::setLed(uint8_t r, uint8_t g, uint8_t b) {
  output.r = r;
  output.g = g;
  output.b = b;
}

void ps5Controller::setRumble(uint8_t small, uint8_t large) {
  output.smallRumble = small;
  output.largeRumble = large;
}

void ps5Controller::setFlashRate(uint8_t onTime, uint8_t offTime) {
  output.flashOn = onTime / 10;
  output.flashOff = offTime / 10;
}

void ps5Controller::sendToController() { ps5SetOutput(output); }

void ps5Controller::attach(callback_t callback) { _callback_event = callback; }

void ps5Controller::attachOnConnect(callback_t callback) {
  _callback_connect = callback;
}

void ps5Controller::attachOnDisconnect(callback_t callback) {
  _callback_disconnect = callback;
}

void ps5Controller::_event_callback(
  void* object, ps5_t data, ps5_event_t event) {
  ps5Controller* This = (ps5Controller*)object;

  memcpy(&This->data, &data, sizeof(ps5_t));
  memcpy(&This->event, &event, sizeof(ps5_event_t));

  if (This->_callback_event) {
    This->_callback_event();
  }
}

void ps5Controller::_connection_callback(void* object, uint8_t isConnected) {
  ps5Controller* This = (ps5Controller*)object;

  if (isConnected) {
    delay(250);  // ToDo: figure out how to know when the channel is free again
                 // so this delay can be removed

    if (This->_callback_connect) {
      This->_callback_connect();
    }
  }
  else {
    if (This->_callback_disconnect) {
      This->_callback_disconnect();
    }
  }
}

#if !defined(NO_GLOBAL_INSTANCES)
ps5Controller ps5;
#endif
