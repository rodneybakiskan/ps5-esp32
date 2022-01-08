#include <ps5Controller.h>

String lastByteToBinary[64];


String byteToBinary(uint8_t byte) {
  String byteString(byte, BIN);

  for (int i = byteString.length(); i < 8; i++) {
    byteString = '0' + byteString;
  }

  return byteString;
}

/* Print bytes and bits in decreasing order where
   each byte is displayed in its group of 8 bits
   and 4 bytes are printed per line

   Example: Print 4 bytes

            Bit 8  Bit 1 Bit 8  Bit 1 Bit 8  Bit 1 Bit 8  Bit 1
            ˅      ˅     ˅      ˅     ˅      ˅     ˅      ˅
   BYTE 3 : 10101110     11010011     10101110     11010011 : BYTE 0
               ˄            ˄            ˄            ˄
            4th Byte     3rd Byte     2nd Byte     1st Byte
*/
void printBits(uint8_t* packet, int byteCount) {
  for (int byte = byteCount - 4; byte >= 0; byte -= 4) {
    lastByteToBinary[byte + 3] = byteToBinary(packet[byte + 3]);
    lastByteToBinary[byte + 2] = byteToBinary(packet[byte + 2]);
    lastByteToBinary[byte + 1] = byteToBinary(packet[byte + 1]);
    lastByteToBinary[byte] = byteToBinary(packet[byte]);

    Serial.printf("BYTE %d :\t%s %s %s %s\t: BYTE %d\n",
                  (byte + 3),
                  lastByteToBinary[byte + 3],
                  lastByteToBinary[byte + 2],
                  lastByteToBinary[byte + 1],
                  lastByteToBinary[byte],
                  byte);
  }
  Serial.println();
}

void compareBits(uint8_t* packet, int byteCount) {
  for (int byteNum = 0; byteNum < byteCount; byteNum++) {
    if (lastByteToBinary[byteNum] != byteToBinary(packet[byteNum])) {
      Serial.print("changed byte:");
      Serial.println(byteNum);
    }

  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  ps5.begin("1a:2b:3c:01:01:01"); //replace with your MAC address
  Serial.println("Ready.");
}

void loop() {
  while (ps5.isConnected() == false) {
    Serial.println("PS5 controller not found");
  }
  delay(1000); //must have this delay for reliable first connection?
  while (ps5.isConnected() == true) {

    printBits(ps5.LatestPacket(), 64);
    delay(300);
    compareBits(ps5.LatestPacket(), 64);
  }

}
