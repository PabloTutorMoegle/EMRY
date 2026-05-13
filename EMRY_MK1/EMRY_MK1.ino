#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>


// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Initialize the display and sensors
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BMP280 bmp;


//BMP data variables
float temp;
float pressure;
float altitude;

// Ajusta este valor para calibrar la altitud:
// - Sube el numero si la altitud lee muy baja (negativa)
// - Baja el numero si la altitud lee muy alta
// Referencia: cada +1 hPa ≈ -8.5m de cambio
#define SEALEVEL_PRESSURE 1015.8  // hPa

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

  Serial.println("=== Todos los sensores OK ===");

  setupSensors();

  display.clearDisplay();
  display.display();
}

void setupSensors() {
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
}

void loop() {
  readBMPData();

  display.clearDisplay();

  printScreen();

  delay(500);
}

void readBMPData() {
  temp = bmp.readTemperature();
  pressure = bmp.readPressure() / 100;
  altitude = bmp.readAltitude(SEALEVEL_PRESSURE);
}

void printScreen() {
  //Print header
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println("EMRY DATA");

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
