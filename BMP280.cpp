#include "BMP280.h"

BMP280:: BMP280(uint8_t address) {
  _address = address;
}

bool BMP280::begin() {

  Wire.begin();
  Wire.beginTransmission(_address);
  Wire.write(0xD0); // Check chip ID
  Wire.endTransmission();
  Wire.requestFrom(_address, 1);
  if (Wire.read() != 0x58) return false;
  delay(100);
  
  Wire.beginTransmission(_address);
  Wire.write(0xE0); // Reset
  Wire.write(0xB6);
  Wire.endTransmission();
  delay(200);

  readCalibrationData(); 

  Wire.beginTransmission(_address);
  Wire.write(0xF4);
  Wire.write(0x27); // Normal mode x1 oversampling 
  Wire.endTransmission();
  
  return true; 
}

float BMP280::temperature() {
  int32_t rawTemp = rawTemperatureData();
  int32_t compTemp = compensateTemp(rawTemp);
  return compTemp / 100.0; 
} 

float BMP280::pressure() {
  int32_t rawP = rawPressureData();
  int32_t comP = compensatePressure(rawP);
  return comP; 
}

int32_t BMP280::rawTemperatureData() {
  Wire.beginTransmission(_address);
  Wire.write(0xFA);
  Wire.endTransmission();
  Wire.requestFrom(_address, 3); 

  // Raw data is made of 20-bits from three registers 
  int32_t raw = ((int32_t) Wire.read() << 12) | ((int32_t) Wire.read() << 4) | ((int32_t) Wire.read() >> 4);
  return raw; 
}

int32_t BMP280::rawPressureData() {
  Wire.beginTransmission(_address);
  Wire.write(0xF7);
  Wire.endTransmission();
  Wire.requestFrom(_address, 3);

  int32_t raw = Wire.read(); // MSB
  raw = (raw << 8) | Wire.read(); // LSB
  raw = (raw << 8) | Wire.read(); // XLSB
  raw >>= 4; // Discard bottom 4 bits
  return raw;
}

void BMP280::readCalibrationData() {
  Wire.beginTransmission(_address);
  Wire.write(0x88); // Point to first of calibration register
  Wire.endTransmission();
  Wire.requestFrom(_address, 24); // Request 24 bytes

  // Temperature 0x88-0x8D
  _dig_T1 = (uint16_t) (Wire.read() | (Wire.read() << 8)); // bytes at adress 0x88 and 0x89 unsigned
  _dig_T2 = (int16_t) (Wire.read() | (Wire.read() << 8)); // bytes at adress 0x8A and 0x8B signed
  _dig_T3 = (int16_t) (Wire.read() | (Wire.read() << 8)); // bytes at adress 0x8C and 0x8D signed

  // Pressure 0x8E-0x9F
  _dig_P1 = (uint16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P2 = (int16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P3 = (int16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P4 = (int16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P5 = (int16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P6 = (int16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P7 = (int16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P8 = (int16_t) (Wire.read() | (Wire.read() << 8));
  _dig_P9 = (int16_t) (Wire.read() | (Wire.read() << 8));
}

int32_t BMP280::compensateTemp(int32_t adc_T) {
    int32_t var1, var2, T;

    var1 = (((adc_T >> 3) - ((int32_t)_dig_T1 << 1)) * ((int32_t)_dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)_dig_T1)) * ((adc_T >> 4) - ((int32_t)_dig_T1))) >> 12) * ((int32_t)_dig_T3)) >> 14;

    _t_fine = var1 + var2;
    T = (_t_fine * 5 + 128) >> 8;  
    return T;
}


float BMP280::compensatePressure(int32_t adc_P) {
    double var1, var2, p;

    var1 = ((double)_t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)_dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)_dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)_dig_P4) * 65536.0);
    var1 = (((double)_dig_P3) * var1 * var1 / 524288.0 + ((double)_dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)_dig_P1);

    if (var1 == 0.0) return 0; // avoid division by zero

    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)_dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)_dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)_dig_P7)) / 16.0;

    return (float)(p / 100.0); // convert Pa to hPa
}
