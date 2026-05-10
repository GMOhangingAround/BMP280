#ifndef BMP280_h
#define BMP280_h

#include "Arduino.h"
#include <Wire.h>

class BMP280 {

  public:
  BMP280(uint8_t address); 
  bool begin();
  float temperature(); 
  float pressure();

  private: 
  uint8_t _address;
  int32_t _t_fine; // For temeperature compensation
  // Temperature variables
  uint16_t _dig_T1;
  int16_t _dig_T2;
  int16_t _dig_T3;
  // Pressure vriables
  uint16_t _dig_P1;
  int16_t _dig_P2, _dig_P3, _dig_P4, _dig_P5, _dig_P6, _dig_P7, _dig_P8, _dig_P9;
  void readCalibrationData();
  int32_t rawTemperatureData();
  int32_t compensateTemp(int32_t data);
  // Pressure methods
  int32_t rawPressureData();
  float compensatePressure(int32_t data);

};

#endif

