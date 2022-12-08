#ifndef ps5_H
#define ps5_H

#include <stdbool.h>
#include <stdint.h>

/********************************************************************************/
/*                                  T Y P E S */
/********************************************************************************/

/********************/
/*    A N A L O G   */
/********************/

typedef struct {
  int8_t lx;
  int8_t ly;
  int8_t rx;
  int8_t ry;
} ps5_analog_stick_t;

typedef struct {
  uint8_t l2;
  uint8_t r2;
} ps5_analog_button_t;

typedef struct {
  ps5_analog_stick_t stick;
  ps5_analog_button_t button;
} ps5_analog_t;

/*********************/
/*   B U T T O N S   */
/*********************/

typedef struct {
  uint8_t right : 1;
  uint8_t down : 1;
  uint8_t up : 1;
  uint8_t left : 1;

  uint8_t square : 1;
  uint8_t cross : 1;
  uint8_t circle : 1;
  uint8_t triangle : 1;

  uint8_t upright : 1;
  uint8_t downright : 1;
  uint8_t upleft : 1;
  uint8_t downleft : 1;

  uint8_t l1 : 1;
  uint8_t r1 : 1;
  uint8_t l2 : 1;
  uint8_t r2 : 1;

  uint8_t share : 1;
  uint8_t options : 1;
  uint8_t l3 : 1;
  uint8_t r3 : 1;

  uint8_t ps : 1;
  uint8_t touchpad : 1;
} ps5_button_t;

/*******************************/
/*   S T A T U S   F L A G S   */
/*******************************/

typedef struct {
  uint8_t battery;
  uint8_t charging : 1;
  uint8_t audio : 1;
  uint8_t mic : 1;
} ps5_status_t;

/********************/
/*   S E N S O R S  */
/********************/

typedef struct {
  int16_t z;
} ps5_sensor_gyroscope_t;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} ps5_sensor_accelerometer_t;

typedef struct {
  ps5_sensor_accelerometer_t accelerometer;
  ps5_sensor_gyroscope_t gyroscope;
} ps5_sensor_t;

/*******************/
/*    O T H E R    */
/*******************/

typedef struct {
  uint8_t smallRumble;
  uint8_t largeRumble;
  uint8_t r, g, b;
  uint8_t flashOn;
  uint8_t flashOff;  // Time to flash bright/dark (255 = 2.5 seconds)
} ps5_cmd_t;

typedef struct {
  ps5_button_t button_down;
  ps5_button_t button_up;
  ps5_analog_t analog_move;
} ps5_event_t;

typedef struct {
  ps5_analog_t analog;
  ps5_button_t button;
  ps5_status_t status;
  ps5_sensor_t sensor;
  uint8_t* latestPacket;
} ps5_t;

/***************************/
/*    C A L L B A C K S    */
/***************************/

typedef void (*ps5_connection_callback_t)(uint8_t isConnected);
typedef void (*ps5_connection_object_callback_t)(void* object, uint8_t isConnected);

typedef void (*ps5_event_callback_t)(ps5_t ps5, ps5_event_t event);
typedef void (*ps5_event_object_callback_t)(void* object, ps5_t ps5, ps5_event_t event);

/********************************************************************************/
/*                             F U N C T I O N S */
/********************************************************************************/

bool ps5IsConnected();
void ps5Init();
void ps5Enable();
void ps5Cmd(ps5_cmd_t ps5_cmd);
void ps5SetConnectionCallback(ps5_connection_callback_t cb);
void ps5SetConnectionObjectCallback(void* object, ps5_connection_object_callback_t cb);
void ps5SetEventCallback(ps5_event_callback_t cb);
void ps5SetEventObjectCallback(void* object, ps5_event_object_callback_t cb);
void ps5SetLed(uint8_t r, uint8_t g, uint8_t b);
void ps5SetOutput(ps5_cmd_t prev_cmd);
void ps5SetBluetoothMacAddress(const uint8_t* mac);
long ps5_l2cap_connect(uint8_t addr[6]);
long ps5_l2cap_reconnect(void);

#endif
