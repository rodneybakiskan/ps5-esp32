#include "ps5.h"

#include <esp_system.h>
#include <string.h>

#include "ps5_int.h"

/********************************************************************************/
/*                              C O N S T A N T S */
/********************************************************************************/

static const uint8_t hid_cmd_payload_ps5_enable[] = {0x43, 0x02};

/********************************************************************************/
/*                         L O C A L    V A R I A B L E S */
/********************************************************************************/

static ps5_connection_callback_t ps5_connection_cb = NULL;
static ps5_connection_object_callback_t ps5_connection_object_cb = NULL;
static void* ps5_connection_object = NULL;

static ps5_event_callback_t ps5_event_cb = NULL;
static ps5_event_object_callback_t ps5_event_object_cb = NULL;
static void* ps5_event_object = NULL;

static bool is_active = false;

/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S */
/********************************************************************************/

/*******************************************************************************
**
** Function         ps5Init
**
** Description      This initializes the bluetooth services to listen
**                  for an incoming ps5 controller connection.
**
**
** Returns          void
**
*******************************************************************************/
void ps5Init() {
  sppInit();
  ps5_l2cap_init_services();
}

/*******************************************************************************
**
** Function         ps5IsConnected
**
** Description      This returns whether a ps5 controller is connected, based
**                  on whether a successful handshake has taken place.
**
**
** Returns          bool
**
*******************************************************************************/
bool ps5IsConnected() { return is_active; }

/*******************************************************************************
**
** Function         ps5Enable
**
** Description      This triggers the ps5 controller to start continually
**                  sending its data.
**
**
** Returns          void
**
*******************************************************************************/
void ps5Enable() {
  uint16_t length = sizeof(hid_cmd_payload_ps5_enable);
  hid_cmd_t hidCommand;

  hidCommand.code = hid_cmd_code_set_report | hid_cmd_code_type_feature;
  hidCommand.identifier = hid_cmd_identifier_ps5_enable;

  memcpy(hidCommand.data, hid_cmd_payload_ps5_enable, length);

  ps5_l2cap_send_hid(&hidCommand, length);
  ps5SetLed(32, 32, 200);
}

/*******************************************************************************
**
** Function         ps5Cmd
**
** Description      Send a command to the ps5 controller.
**
**
** Returns          void
**
*******************************************************************************/
void ps5Cmd(ps5_cmd_t cmd) {
  hid_cmd_t hidCommand = {.data = {0x80, 0x00, 0xFF}};
  uint16_t length = sizeof(hidCommand.data);

  hidCommand.code = hid_cmd_code_set_report | hid_cmd_code_type_output;
  hidCommand.identifier = hid_cmd_identifier_ps5_control;

  hidCommand.data[ps5_control_packet_index_small_rumble] = cmd.smallRumble;  // Small Rumble
  hidCommand.data[ps5_control_packet_index_large_rumble] = cmd.largeRumble;  // Big rumble

  hidCommand.data[ps5_control_packet_index_red] = cmd.r;    // Red
  hidCommand.data[ps5_control_packet_index_green] = cmd.g;  // Green
  hidCommand.data[ps5_control_packet_index_blue] = cmd.b;   // Blue

  // Time to flash bright (255 = 2.5 seconds)
  hidCommand.data[ps5_control_packet_index_flash_on_time] = cmd.flashOn;
  // Time to flash dark (255 = 2.5 seconds)
  hidCommand.data[ps5_control_packet_index_flash_off_time] = cmd.flashOff;

  ps5_l2cap_send_hid(&hidCommand, length);
}

/*******************************************************************************
**
** Function         ps5SetLedOnly
**
** Description      Sets the LEDs on the ps5 controller.
**
**
** Returns          void
**
*******************************************************************************/
void ps5SetLed(uint8_t r, uint8_t g, uint8_t b) {
  ps5_cmd_t cmd = {0};

  cmd.r = r;
  cmd.g = g;
  cmd.b = b;

  ps5Cmd(cmd);
}

/*******************************************************************************
**
** Function         ps5SetOutput
**
** Description      Sets feedback on the ps5 controller.
**
**
** Returns          void
**
*******************************************************************************/
void ps5SetOutput(ps5_cmd_t prevCommand) { ps5Cmd(prevCommand); }

/*******************************************************************************
**
** Function         ps5SetConnectionCallback
**
** Description      Registers a callback for receiving ps5 controller
**                  connection notifications
**
**
** Returns          void
**
*******************************************************************************/
void ps5SetConnectionCallback(ps5_connection_callback_t cb) {
  ps5_connection_cb = cb;
}

/*******************************************************************************
**
** Function         ps5SetConnectionObjectCallback
**
** Description      Registers a callback for receiving ps5 controller
**                  connection notifications
**
**
** Returns          void
**
*******************************************************************************/
void ps5SetConnectionObjectCallback(void* object, ps5_connection_object_callback_t cb) {
  ps5_connection_object_cb = cb;
  ps5_connection_object = object;
}

/*******************************************************************************
**
** Function         ps5SetEventCallback
**
** Description      Registers a callback for receiving ps5 controller events
**
**
** Returns          void
**
*******************************************************************************/
void ps5SetEventCallback(ps5_event_callback_t cb) { ps5_event_cb = cb; }

/*******************************************************************************
**
** Function         ps5SetEventObjectCallback
**
** Description      Registers a callback for receiving ps5 controller events
**
**
** Returns          void
**
*******************************************************************************/
void ps5SetEventObjectCallback(void* object, ps5_event_object_callback_t cb) {
  ps5_event_object_cb = cb;
  ps5_event_object = object;
}

/*******************************************************************************
**
** Function         ps5SetBluetoothMacAddress
**
** Description      Writes a Registers a callback for receiving ps5 controller
*events
**
**
** Returns          void
**
*******************************************************************************/
void ps5SetBluetoothMacAddress(const uint8_t* mac) {
  // The bluetooth MAC address is derived from the base MAC address
  // https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/system.html#mac-address
  uint8_t baseMac[6];
  memcpy(baseMac, mac, 6);
  baseMac[5] -= 2;
  esp_base_mac_addr_set(baseMac);
}

/********************************************************************************/
/*                      L O C A L    F U N C T I O N S */
/********************************************************************************/

void ps5ConnectEvent(uint8_t is_connected) {
    if (is_connected) {
        ps5Enable();
    } else {
        is_active = false;
    }
}


void ps5PacketEvent(ps5_t ps5, ps5_event_t event) {
    // Trigger packet event, but if this is the very first packet
    // after connecting, trigger a connection event instead
    if (is_active) {
        if(ps5_event_cb != NULL) {
            ps5_event_cb(ps5, event);
        }

        if (ps5_event_object_cb != NULL && ps5_event_object != NULL) {
            ps5_event_object_cb(ps5_event_object, ps5, event);
        }
    } else {
        is_active = true;

        if(ps5_connection_cb != NULL) {
            ps5_connection_cb(is_active);
        }

        if (ps5_connection_object_cb != NULL && ps5_connection_object != NULL) {
            ps5_connection_object_cb(ps5_connection_object, is_active);
        }
    }
}
