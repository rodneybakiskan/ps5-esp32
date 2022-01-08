# ps5-esp32
Use a ps5 controller with an ESP32.

This is heavily based on the work of Jeffery Pernis to connect a PS3 controller to an ESP32, and the PS4-esp32 library. 

PS3 controller: https://github.com/jvpernis/esp32-ps3

PS4 controller: https://github.com/aed3/PS4-esp32

[Here's a video](https://youtu.be/BmlKBs27pgE) about how the ps4 library was made.

This repo can be downloaded as a zip file and imported into the Arduino IDE as a library.

## Installation
The instructions on how to do this are base off what can be found [here](https://github.com/jvpernis/esp32-ps3/issues/3#issuecomment-517141523)
1. You can add the ESP32 boards to your Arduino IDE by adding them to the Boards Manager:
    1. Open `File -> Preferences`
    1. Paste the following URL in the `Additional Boards Manager URLs` field:
    `https://dl.espressif.com/dl/package_esp32_index.json`
    1. Open the Boards Manager with `Tools -> Board: "xxx" -> Boards Manager`
    1. Look for `esp32` (probably the last one in the list), and click `install`
    1. Select the ESP32 board you have with `Tools -> Board: "xxx"` under the section `ESP32 Arduino`
1. To install this library into your Arduino IDE:
    1. Click on the "Code" button in the top right of this page
    1. Select "Download Zip" (It's always a good idea to look through the code on this page first to make sure you know what you're downloading)
    1. In the Arduino IDE, navigate to `Sketch -> Include Library -> Add .ZIP Library`, then select the file you just downloaded 

## Pairing the ps5 Controller:
When a ps5 controller is 'paired' to a ps5 console, it just means that it has stored the console's Bluetooth MAC address, which is the only device the controller will connect to. Usually, this pairing happens when you connect the controller to the ps5 console using a USB cable, and press the PS button. This initiates writing the console's MAC address to the controller.

Therefore, if you want to connect your ps5 controller to the ESP32, you either need to figure out what the Bluetooth MAC address of your ps5 console is and set the ESP32's address to it, or change the MAC address stored in the ps5 controller.

Whichever path you choose, you might want a tool to read and/or write the currently paired MAC address from the ps5 controller. You can try using [sixaxispairer](https://github.com/user-none/sixaxispairer) for this purpose.

If you opted to change the ESP32's MAC address, you'll need to include the ip address in the ```ps5.begin()``` function during within the ```setup()``` Arduino function like below where ```1a:2b:3c:01:01:01``` is the MAC address (**note that MAC address must be unicast**):
```
void setup()
{
    ps5.begin("1a:2b:3c:01:01:01");
    Serial.println("Ready.");
}
```

**TLDR:** Connect ps5 controller with phone through bluetooth. Get phone's bluetooth MAC address (look in About settings of phone). Replace '1a:2b:3c:01:01:01' with your phones bluetooh MAC. Same can be done with your ps5 console if more convenient.
##
**Note: Only controller inputs have been tested. Other functions like rumble or lights might not work, but could be implemented using the 'ps5ViewIncomingBits.ino' example.**
