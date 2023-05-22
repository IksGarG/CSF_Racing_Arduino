// Tests
#include <TimeLib.h>
#include <U8g2lib.h>
#include <SPI.h>

// Define the display object
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 11, /* data=*/ 12, /* CS=*/ 13, /* reset=*/ U8X8_PIN_NONE);

// Define the pulse count variables
volatile unsigned long totalPulseCount = 0;
volatile unsigned long lastPulseTime = 0;


void setup() {
  // Serial Begin
  Serial.begin(9600);

  // Initialize the LCD
  u8g2.begin();
  u8g2.setColorIndex(1);

  // Set time
  setTime(0, 0, 0, 15, 5,  2023);

  // Set the default screen to the time screen
  displayTime();  


}

// Define the total number of screens
const int NUM_SCREENS = 3;

// Define an array of function pointers to the display functions
void (*displayFunctions[NUM_SCREENS])() = {
  displayTime,
  displayTempAndSpeed,
  displayMileage
};

// Define the current screen index
int currentScreenIndex = 0;

void loop() {
  static unsigned long lastSwitchTime = 0;
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();
  unsigned long currentTime2 = millis();

  // Check if 1 second has elapsed since the last time update
  if (currentTime2 - lastUpdateTime >= 1000) {
    // Call the displayTime function
    displayTime();

    // Update the last update time
    lastUpdateTime = currentTime2;
  }

  // Check if 5 seconds have elapsed since the last function switch
  if (currentTime - lastSwitchTime >= 10000) {
    // Increment the screen index
    currentScreenIndex = (currentScreenIndex + 1) % NUM_SCREENS;

    // Call the display function for the current screen
    displayFunctions[currentScreenIndex]();

    // Update the last switch time
    lastSwitchTime = currentTime;
  }
}

// Interrupt service routine to handle pulse input
void handlePulseInterrupt() {
  totalPulseCount++;
}

// Displays the current time on the LCD.
void displayTime() {
  u8g2.clearBuffer();
  u8g2.setDisplayRotation(U8G2_R0);
  u8g2.setFlipMode(1);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Current time:");
  u8g2.setCursor(0, 50);
  u8g2.print(getCurrentTime());
  u8g2.sendBuffer();
}

String getCurrentTime() {
  // Use the TimeLib library to get the current time
  // Assumes that the time has been set with setTime() at some point
  time_t currentTime = now();
  char buffer[9];
  sprintf(buffer, "%02d:%02d:%02d", hour(currentTime), minute(currentTime), second(currentTime));
  return String(buffer);
}

void displayTempAndSpeed() {
  u8g2.clearBuffer();
  u8g2.setFlipMode(1);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Engine Temp:");
  u8g2.setCursor(0, 35);
  // u8g2.print(tempSensor.readObjectTempC());
  u8g2.print("OK");
  u8g2.setCursor(0, 50);
  u8g2.print("Speed:");
  u8g2.setCursor(0, 65);
  // u8g2.print(getCurrentSpeed());
  u8g2.print("OK");
  u8g2.sendBuffer();
}

// Displays the mileage on the LCD.
void displayMileage() {
  u8g2.clearBuffer();
  u8g2.setFlipMode(1);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Mileage:");
  u8g2.setCursor(0, 35);
  // u8g2.print(getCurrentMileage());
  u8g2.print("OK");
  u8g2.sendBuffer();
}



