// Updated Code for CSF Racing //
// Created with love and hope for the team by Iker Garcia German //
// This is the first final version of the code //
// I call it VOLT 1.0 //
// -- STILL IN DEVELOPMENT -- //
// LCD size incorrect NEEDS ADJUSTMENTS //
// 10/05/2023 //
/* 
  This code uses a simple switch case for each state of the LCD
  It shows: 
   - Time
   - Temperature
   - Speed in km/h (USES SPEEDOMETER, WILL CHANGE LATER TO USE WITH GPS)
   - ON/OFF switches (Lights,Speaker,Fan)
   - Mileage
  It will show:
   - Battery percentage
*/
#include <TimeLib.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Adafruit_MLX90614.h>

// Define the pins for the LCD
#define CS_PIN 10
#define DC_PIN 9
#define RESET_PIN 8

// Define the display object
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ RESET_PIN);

// Define the MLX90614 sensor object
Adafruit_MLX90614 tempSensor;

// Define the pulse count variables
volatile unsigned long totalPulseCount = 0;
volatile unsigned long lastPulseTime = 0;

// Define the wheel circumference in meters
const float WHEEL_CIRCUMFERENCE = 2 * PI * 0.3;

// Define the number of seconds per hour
const float SECONDS_PER_HOUR = 3600.0;

void setup() {
  // Initialize the LCD
  u8g2.begin();
  
  // Initialize the SPI interface
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  // Initialize the MLX90614 sensor
  tempSensor.begin();

  // Attach an interrupt to the pulse input pin
  attachInterrupt(digitalPinToInterrupt(2), handlePulseInterrupt, RISING);

  // Set the default screen to the time screen
  displayTime();
}
// Define the total number of screens
const int NUM_SCREENS = 4;

// Define an array of function pointers to the display functions
void (*displayFunctions[NUM_SCREENS])() = {
  displayTime,
  displaySwitches,
  displayTempAndSpeed,
  displayMileage
};

// Define the current screen index
int currentScreenIndex = 0;

void loop() {
  // Check for button press
  if (digitalRead(3) == LOW) {
    // Increment the screen index
    currentScreenIndex = (currentScreenIndex + 1) % NUM_SCREENS;

    // Call the display function for the current screen
    displayFunctions[currentScreenIndex]();
  }
}

// Interrupt service routine to handle pulse input
void handlePulseInterrupt() {
  totalPulseCount++;
}

// Displays the current time on the LCD.
void displayTime() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Current time:");
  u8g2.setCursor(0, 50);
  u8g2.print(getCurrentTime());
  u8g2.sendBuffer();
}

// Returns the current time as a string.
String getCurrentTime() {
  // Use the TimeLib library to get the current time
  // Assumes that the time has been set with setTime() at some point
  time_t currentTime = now();
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", hour(currentTime), minute(currentTime), second(currentTime));
  return String(buffer);
}

// Displays the state of the switches on the LCD.
void displaySwitches() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Switch 1: ");
  u8g2.print(digitalRead(6));
  u8g2.setCursor(0, 35);
  u8g2.print("Switch 2: ");
  u8g2.print(digitalRead(7));
  u8g2.setCursor(0, 50);
  u8g2.print("Switch 3: ");
  u8g2.print(digitalRead(8));
  u8g2.sendBuffer();
}

// Displays the temperature and speed on the LCD.
void displayTempAndSpeed() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Engine Temp:");
  u8g2.setCursor(0, 35);
  u8g2.print(tempSensor.readObjectTempC());
  u8g2.setCursor(0, 50);
  u8g2.print("Speed:");
  u8g2.setCursor(0, 65);
  u8g2.print(getCurrentSpeed());
  u8g2.sendBuffer();
}

// Calculates and returns the current speed in km/h.
float getCurrentSpeed() {
  unsigned long currentPulseCount = totalPulseCount;
  unsigned long deltaTime = millis() - lastPulseTime;
  float currentSpeed = (currentPulseCount * WHEEL_CIRCUMFERENCE) / (deltaTime / 1000.0);
  lastPulseTime = millis();
  return (currentSpeed * SECONDS_PER_HOUR) / 1000.0;
}

// Displays the mileage on the LCD.
void displayMileage() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Mileage:");
  u8g2.setCursor(0, 35);
  u8g2.print(getCurrentMileage());
  u8g2.sendBuffer();
}

// Calculates and returns the current mileage in km.
float getCurrentMileage() {
  return (totalPulseCount * WHEEL_CIRCUMFERENCE) / 1000.0;
}