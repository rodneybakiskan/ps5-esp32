#include <ps5Controller.h>

void setup() {
  Serial.begin(115200);

  // Replace the "1a:2b:3c:01:01:01" with the MAC address
  // the controller wants to pair to
  // Note: MAC address must be unicast
  ps5.begin("1a:2b:3c:01:01:01");
  Serial.println("Ready.");
}

void loop() {
  if (ps5.isConnected()) {
    Serial.println("Connected!");
  }

  delay(3000);
}
