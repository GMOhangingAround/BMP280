# BMP280
# BMP280

A custom library for the BMP280 temperature and pressure sensor communicating over I2C.

## Description

This library provides a simple interface to read temperature and pressure data from the BMP280 sensor. It handles sensor initialisation, calibration data retrieval, and applies the Bosch compensation formula to return accurate readings.

## Methods

- `BMP280(uint8_t address)` — constructor, accepts I2C address (default 0x76)
- `begin()` — initialises the sensor, verifies chip ID, reads calibration data and sets normal mode
- `temperature()` — returns temperature in degrees Celsius
- `pressure()` — returns pressure in hPa
