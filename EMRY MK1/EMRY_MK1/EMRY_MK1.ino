#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Initialize the display and sensors
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BMP280 bmp;
Adafruit_MPU6050 mpu;

//BMP data variables
float temp;
float pressure;
float altitude;

void setup() {
  Serial.begin(115200);
  delay(1500); // Esperar a que el Serial USB se conecte

  Serial.println("=== EMRY_MK1 iniciando ===");

  // Initialize I2C with specific SDA (21) and SCL (22) pins
  Wire.begin(21, 22);

  // I2C Scanner - antes de inicializar cualquier sensor
  Serial.println("Escaneando bus I2C...");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("  I2C encontrado en 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Escaneo I2C completo.");

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("ERROR: SSD1306 no encontrado en 0x3C");
  } else {
    Serial.println("OK: SSD1306 encontrado");
  }

  // Initialize BMP280 sensor
  if (!bmp.begin(0x76)) {
    Serial.println("ERROR: BMP280 no encontrado en 0x76, probando 0x77...");
    if (!bmp.begin(0x77)) {
      Serial.println("ERROR: BMP280 tampoco en 0x77");
    } else {
      Serial.println("OK: BMP280 encontrado en 0x77");
    }
  } else {
    Serial.println("OK: BMP280 encontrado en 0x76");
  }

  // Initialize MPU6050 sensor - probar ambas direcciones
  if (!mpu.begin()) {
    Serial.println("MPU6050 no encontrado en 0x68, probando 0x69...");
    if (!mpu.begin(0x69)) {
      Serial.println("ERROR: MPU6050 no encontrado en 0x68 ni 0x69");
      Serial.println("REVISA: AD0 a GND = 0x68, AD0 a 3.3V = 0x69");
      Serial.println("REVISA: pull-ups 4.7k en SDA/SCL");
      Serial.println("REVISA: alimentacion 3.3V y conexiones");
      while (1) {
        Serial.println("no MPU");
        delay(3000);
      }
    } else {
      Serial.println("OK: MPU6050 encontrado en 0x69");
    }
  } else {
    Serial.println("OK: MPU6050 encontrado en 0x68");
  }

  Serial.println("=== Todos los sensores OK ===");

  setupSensors();

  display.clearDisplay();
  display.display();
}

void setupSensors() {
  //setup BMP280 sensor
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  //setup MPU6050 sensor
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  readBMPData();
  readMPUData();

  // Clear display buffer
  display.clearDisplay();

  printScreen();

  delay(500);
}

void readBMPData() {
  temp = bmp.readTemperature();
  pressure = bmp.readPressure() / 100;
  altitude = bmp.readAltitude(1013.25); /* Adjusted to local forecast! */
}

void readMPUData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("Accel X: ");
  Serial.print(a.acceleration.x);
  Serial.print(" m/s^2, Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(" m/s^2, Z: ");
  Serial.print(a.acceleration.z);
  Serial.print(" | Gyro X: ");
  Serial.print(g.gyro.x);
  Serial.print(" rad/s, Y: ");
  Serial.print(g.gyro.y);
  Serial.print(" rad/s, Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");
}

void printScreen() {
  //Print header
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 0);
  display.println("DATA");

  //Print temperature
  display.setTextSize(1);
  display.setCursor(5, 20);
  display.print("Temp: ");
  display.print(temp, 1);
  display.println(" *C");

  //Print preassure
  display.setCursor(5, 30);
  display.print("Pressure: ");
  display.print(pressure, 1);
  display.println(" hPa");

  //Print altitude
  display.setCursor(5, 40);
  display.print("Altitude: ");
  display.print(altitude, 1);
  display.println(" m");

  // Update the display with the buffer contents
  display.display();
}
