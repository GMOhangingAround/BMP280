#include "BMP280.h"

BMP280 bmp(0x76);

void setup() {
  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("BMP280 not found!");
    while(1); 
  }
  Serial.println("BMP280 is ready!");
}

void loop() {
  Serial.print(bmp.temperature());
  Serial.print(",");
  Serial.println(bmp.pressure());
  delay(1000);
}
