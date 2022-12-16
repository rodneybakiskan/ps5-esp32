#include <ps5Controller.h>

void setup() {
  Serial.begin(115200);

  ps5.begin("1a:2b:3c:01:01:01"); //replace with MAC address of your controller
  Serial.println("Ready.");
}

void loop() {
  if (ps5.isConnected()) {
    Serial.println("Connected!");
  }

  delay(3000);
}
