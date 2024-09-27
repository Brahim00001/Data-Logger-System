//STM32 code for the same project
#include "stm32f4xx_hal.h"
#include "fatfs.h"  // For SD card handling
#include "i2c.h"    // For I2C (OLED)
#include "spi.h"    // For SPI (SD card)
#include "gpio.h"   // For GPIO (DHT11)

// DHT11 pin configuration
#define DHT11_PORT GPIOC
#define DHT11_PIN GPIO_PIN_7

// Function prototypes
void SystemClock_Config(void);
void read_dht11(float *temperature, float *humidity);
void log_data_to_sd(float temperature, float humidity);
void display_data_on_oled(float temperature, float humidity);

int main(void) {
    // Initialize hardware and peripherals
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_FATFS_Init();

    // Variables for sensor data
    float temperature = 0;
    float humidity = 0;

    // Main loop
    while (1) {
        // Read temperature and humidity from DHT11 sensor
        read_dht11(&temperature, &humidity);

        // Log data to SD card
        log_data_to_sd(temperature, humidity);

        // Display data on OLED
        display_data_on_oled(temperature, humidity);

        // Delay for 2 seconds
        HAL_Delay(2000);
    }
}

// Function to read temperature and humidity from DHT11 sensor
void read_dht11(float *temperature, float *humidity) {
    // Implement the logic for reading data from DHT11 sensor
    // This is typically done by sending a start signal and receiving
    // 5 bytes of data that include humidity and temperature values
}

// Function to log data to SD card using SPI
void log_data_to_sd(float temperature, float humidity) {
    char logBuffer[50];
    sprintf(logBuffer, "Temperature: %.2f, Humidity: %.2f\n", temperature, humidity);
    
    // Mount the SD card
    if (f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) == FR_OK) {
        // Open file for appending data
        if (f_open(&SDFile, "datalog.txt", FA_OPEN_APPEND | FA_WRITE) == FR_OK) {
            // Write data to the file
            f_write(&SDFile, logBuffer, strlen(logBuffer), NULL);
            f_close(&SDFile);  // Close the file after writing
        }
        f_mount(NULL, "", 1);  // Unmount SD card
    }
}

// Function to display data on OLED using I2C
void display_data_on_oled(float temperature, float humidity) {
    char buffer[20];
    
    // Clear the OLED display and set cursor
    ssd1306_Fill(SSD1306_COLOR_BLACK);
    ssd1306_SetCursor(0, 0);

    // Display temperature
    sprintf(buffer, "Temp: %.2f C", temperature);
    ssd1306_WriteString(buffer, SSD1306_COLOR_WHITE);

    // Display humidity
    ssd1306_SetCursor(0, 20);
    sprintf(buffer, "Humidity: %.2f %%", humidity);
    ssd1306_WriteString(buffer, SSD1306_COLOR_WHITE);

    // Refresh the OLED screen
    ssd1306_UpdateScreen();
}
