## EMRY MK1

Done on an ESP32.

With: BMP-280, MPU-6050, OLED Display SSD1306 YB. 

ESP32                          OLED SSD1306
-------                        -----------
GPIO21 (SDA) ──────────────── SDA (pin)
GPIO22 (SCL) ──────────────── SCL (pin)
3.3V       ──────────────── VCC
GND        ──────────────── GND


ESP32                          BMP280
-------                        ------
GPIO21 (SDA) ──────────────── SDA (SDI)
GPIO22 (SCL) ──────────────── SCL (SCK)
3.3V       ──────────────── VCC
GND        ──────────────── GND
                              SDO ── GND  (da dirección 0x76)


ESP32                          MPU6050
-------                        -------
GPIO21 (SDA) ──────────────── SDA
GPIO22 (SCL) ──────────────── SCL
3.3V       ──────────────── VCC
GND        ──────────────── GND
                              AD0 ── GND  (da dirección 0x68)

                              