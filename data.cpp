#include <SPI.h>             // For SD card communication
#include <SD.h>              // SD card library
#include <Wire.h>            // For OLED communication (I2C)
#include <Adafruit_GFX.h>    // OLED graphics library
#include <Adafruit_SSD1306.h> // OLED display library
#include <DHT.h>             // For DHT11 sensor

#define OLED_RESET -1  // OLED reset pin (not used)
Adafruit_SSD1306 display(OLED_RESET);

// SD card CS pin
const int chipSelect = 10;

// DHT11 sensor
#define DHTPIN 8
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Variables for sensor data
float temperature;
float humidity;

// SD card file handler
File dataFile;

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);
  while (!Serial) { } // Wait for serial to be ready

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the OLED display
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Loop forever if display fails
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Print a welcome message on OLED
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Data Logger System");
  display.display();

  // Initialize the SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    display.setCursor(0, 16);
    display.println("SD Init Failed!");
    display.display();
    return;
  }
  Serial.println("Initialization done.");
  display.setCursor(0, 16);
  display.println("SD Init Success!");
  display.display();

  // Create/open a file for writing
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Temperature,Humidity"); // Write header
    dataFile.close();
    Serial.println("File opened and header written.");
  } else {
    Serial.println("Error opening datalog.txt");
  }
}

void loop() {
  // Read temperature and humidity data from DHT11 sensor
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    display.setCursor(0, 32);
    display.println("DHT Error");
    display.display();
    return;
  }

  // Display data on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Data Logger");
  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(temperature);
  display.print(" C");
  display.setCursor(0, 26);
  display.print("Humidity: ");
  display.print(humidity);
  display.print(" %");
  display.display();

  // Log data to SD card
  logDataToSD(temperature, humidity);

  // Wait 2 seconds before the next loop
  delay(2000);
}

void logDataToSD(float temp, float hum) {
  // Open file to append data
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (dataFile) {
    // Write data in CSV format
    dataFile.print(temp);
    dataFile.print(",");
    dataFile.println(hum);
    dataFile.close();
    Serial.println("Data written to SD card.");
  } else {
    Serial.println("Error opening datalog.txt");
  }
}
