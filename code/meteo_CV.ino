#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <limits.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BMP280 bmp;

unsigned long previousTime = 0;     // variable for setting timer for screens
unsigned long mainLoopCounter = 0;  // main iterator for loop cycle, created for logging pressure and temperature
unsigned long prevMainLoopCounter = 0;
int screenIterator = 0;             // iterator for loop cycle, mainly created for changing screens
int previousScreenIterator = 0;     // variable to save previous 'i' for void loop
int horizonOffset = 27;             // horizon offset by pixels for graphics
int pressureArraySize = 0;          // size of pressures array
int temperatureArraySize = 0;       // size of temperatures array
int screenUpdateInterval = 50;      // time interval in milliseconds to update information on screen
int pressureLogInterval = 18000;    // interval for logging pressure into array(this interval keeps logging of pressure every 15 minutes).
int temperatureLogInterval = 9000;  // interval for logging temperature into array(this interval keeps logging of temperature every 7,5 minutes).
bool forecastReady = false;         // forecast will be started only when in pressure array will be at least 11 elements(logs)
bool showSecondScreen = false;
bool showThirdScreen = false;
bool temperatureOverflow = false;  // overflow of temperatures array
bool pressureOverflow = false;     // overflow of pressures array
float pressureLog[50];
float temperatureLog[50];

const unsigned char epd_bitmap_cloudy[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0,
  0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x3e, 0xf8, 0x00, 0x00, 0x00, 0x38, 0x1c, 0x00,
  0x00, 0x00, 0x70, 0x0e, 0xfe, 0x00, 0x07, 0xe0, 0x0f, 0xff, 0x00, 0x0f, 0xe0, 0x07, 0xc7, 0x80,
  0x1c, 0x20, 0x07, 0x01, 0xc0, 0x18, 0x00, 0x3e, 0x00, 0xc0, 0x38, 0x00, 0xfe, 0x00, 0xe0, 0x78,
  0x01, 0xee, 0x00, 0x60, 0x70, 0x01, 0x80, 0x00, 0x70, 0xe0, 0x03, 0x80, 0x00, 0xfc, 0xc0, 0x03,
  0x80, 0x00, 0xfe, 0xe0, 0x07, 0x80, 0x00, 0x07, 0xe0, 0x0e, 0x00, 0x00, 0x07, 0x7f, 0xfc, 0x00,
  0x00, 0x03, 0x3f, 0xfc, 0x00, 0x00, 0x07, 0x00, 0x0c, 0x00, 0x00, 0x07, 0x00, 0x0f, 0x00, 0x00,
  0x1e, 0x00, 0x07, 0xff, 0xff, 0xfc, 0x00, 0x03, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_sunny[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00,
  0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x40, 0x18, 0x02, 0x00, 0x00, 0xe0, 0x18,
  0x07, 0x00, 0x00, 0x70, 0x00, 0x0e, 0x00, 0x00, 0x38, 0x00, 0x1c, 0x00, 0x00, 0x1c, 0x00, 0x38,
  0x00, 0x00, 0x08, 0x7f, 0x10, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00,
  0x00, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x0f,
  0xc7, 0xff, 0xe3, 0xf0, 0x0f, 0xc7, 0xff, 0xe3, 0xf0, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x07,
  0xff, 0xe0, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x01, 0xff,
  0x80, 0x00, 0x00, 0x08, 0xff, 0x10, 0x00, 0x00, 0x1c, 0x00, 0x38, 0x00, 0x00, 0x38, 0x00, 0x1c,
  0x00, 0x00, 0x70, 0x00, 0x0e, 0x00, 0x00, 0xe0, 0x18, 0x07, 0x00, 0x00, 0x40, 0x18, 0x02, 0x00,
  0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  Serial.begin(115200);
  for (int k = 0; k < 50; k++) {
    pressureLog[k] = 0;        // in pressure we can put '0' while initializing cause atmospheric pressure == 0 only in space :)
    temperatureLog[k] = -100;  // in temperature we have to initialize by -100, i hope this sensor will never meet this temperature
  }
  if (!bmp.begin()) {
    Serial.println("Error: BMP280 not connected or wrong address (0x76).");
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Error: OLED display not connected or wrong address (0x3C).");
  }
  display.clearDisplay();
}

void displayForecast() {
  int pressureChange = 0;
  if (pressureArraySize >= 12) {  // by the pressureChange variable we can predict future weather for 8~10 hours
    pressureChange = pressureLog[pressureArraySize - 12] - pressureLog[pressureArraySize - 1];
  } else if (pressureArraySize > 0) {  // an 'else' for safety
    pressureChange = pressureLog[0] - pressureLog[pressureArraySize - 1];
  } else {  // an 'else' for safety
    return;
  }
  display.setCursor(0, 57);  // all this text will be replaced by bitmaps and small messages for weather
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  if (pressureChange > 0)  // if the pressureChange is >0 it means that the pressure was decreasing and it means that the weather is going to be stormy
    display.drawBitmap(0, 20, epd_bitmap_cloudy, 40, 64, SSD1306_WHITE);
  else  // if it is <0 it means that pressure increased and weather will be better
    display.drawBitmap(0, 20, epd_bitmap_sunny, 40, 64, SSD1306_WHITE);

  display.drawRect(44, 20, 84, 44, SSD1306_WHITE);
  display.setCursor(48, 28);
  display.print("THE FORECAST");
  display.setCursor(48, 38);
  display.print("MAY NOT BE");
  display.setCursor(48, 48);
  display.print("ACCURATE");
}

void drawGraphTable() {  // function to draw a table for graphic
  display.drawRect(25, 20, 98, 44, SSD1306_WHITE);
  for (int x = 38; x < 128; x += 12) {
    display.drawLine(x, 20, x, 64, SSD1306_WHITE);
  }
}

void displayEstimatedTime(int interval) {  // function to show an estimated time
  // after starting a microcontroller we have to wait some time (30 mins for pressure and 15 minutes for temperature)
  // cause there will be no data on graphic and no logs in arrays
  display.drawRect(0, 20, 128, 44, SSD1306_WHITE);
  display.setCursor(5, 26);
  display.print("Please wait for data");
  display.setCursor(22, 40);
  display.print("ESTIMATED TIME");
  unsigned long currentTime = millis();
  display.setCursor(34, 52);
  display.print(((((interval) * (screenUpdateInterval + 10) * 2) - currentTime) / 1000) / 60);
  display.print(" minutes");
  display.display();
}

void drawTemperatureGraph() {       // function to show graphic of temperatures
  if (temperatureLog[2] == -100) {  // check if we have logs for the graphic
    displayEstimatedTime(temperatureLogInterval);
    return;
  }
  int previousY = 0;  // previous dot on graphic
  int remainingEntries = temperatureArraySize - 1;
  int index = temperatureOverflow ? 25 : 0;  // on the graphics is only new information so by handling overflow second part of array will be rewritten to the first part
  while ((remainingEntries - index) > 0) {
    int y = 48 - (((int)temperatureLog[index]) / 2);  // a formula to display temperature dot on graphic table
    if (previousY != 0) {
      // the line between dots has a width of 2 pixels
      display.drawLine(horizonOffset - 5, previousY, horizonOffset, y, SSD1306_WHITE);
      display.drawLine(horizonOffset - 5, previousY + 1, horizonOffset, y + 1, SSD1306_WHITE);
    }
    // the dot is a square 2x2 pixels
    display.drawPixel(horizonOffset, y, SSD1306_WHITE);
    display.drawPixel(horizonOffset + 1, y, SSD1306_WHITE);
    display.drawPixel(horizonOffset, y + 1, SSD1306_WHITE);
    display.drawPixel(horizonOffset + 1, y + 1, SSD1306_WHITE);
    horizonOffset += 4;
    index += 2;
    previousY = y;
  }
  drawGraphTable();
  display.setCursor(0, 57);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("-20");  // a lowest border of temperature on graphic
  display.setCursor(4, 23);
  display.print("40");  // a highest border
  horizonOffset = 27;
  previousY = 0;
}

void drawPressureGraph() {    // function to show graphic of pressures
  if (pressureLog[2] == 0) {  // check if we have logs for the graphic
    displayEstimatedTime(pressureLogInterval);
    return;
  }
  int previousY = 0;  // previous dot on graphic
  int remainingEntries = pressureArraySize - 1;
  int index = pressureOverflow ? 25 : 0;  // on the graphics is only new information so by handling overflow second part of array will be rewritten to the first part
  while ((remainingEntries - index) > 0) {
    int y = 64 - (((int)pressureLog[index] - 940) / 2);  // a formula to display pressure dot on graphic table
    if (previousY != 0) {
      // the line between dots has a width of 2 pixels
      display.drawLine(horizonOffset - 5, previousY, horizonOffset, y, SSD1306_WHITE);
      display.drawLine(horizonOffset - 5, previousY + 1, horizonOffset, y + 1, SSD1306_WHITE);
    }
    // the dot is a square 2x2 pixels
    display.drawPixel(horizonOffset, y, SSD1306_WHITE);
    display.drawPixel(horizonOffset + 1, y, SSD1306_WHITE);
    display.drawPixel(horizonOffset, y + 1, SSD1306_WHITE);
    display.drawPixel(horizonOffset + 1, y + 1, SSD1306_WHITE);
    horizonOffset += 4;
    index += 2;
    previousY = y;
  }
  drawGraphTable();
  display.setCursor(0, 57);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("950");  // a lowest border of pressure on graphic
  display.setCursor(0, 23);
  display.print("1015");  // a highest border
  horizonOffset = 27;
  previousY = 0;
}

void handleArrayOverflow(float array[50]) {
  // in this function we are overriding the logs from end of array to the first part so we can place new logs
  for (int j = 25; j < 49; j++) {
    array[j - 25] = array[j];
    array[j] = 0;
  }
  if (array[0] < 900)  // checking if that was an array of pressures or temperatures
    temperatureOverflow = true;
  else
    pressureOverflow = true;
}

void logPressure(float pressure) {
  pressureLog[pressureArraySize] = pressure;
  pressureArraySize++;
  if (pressureArraySize == 50) {
    handleArrayOverflow(pressureLog);
    pressureArraySize = 25;
  }
}

void logTemperature(float temperature) {
  temperatureLog[temperatureArraySize] = temperature;
  temperatureArraySize++;
  if (temperatureArraySize == 50) {
    handleArrayOverflow(temperatureLog);
    temperatureArraySize = 25;
  }
}

void loop() {
  unsigned long currentTime = millis();
  // current time variable used to check time intervals for showing windows and logging pressure and temperature
  float temperature = bmp.readTemperature();
  float pressure = (bmp.readPressure() / 100.0F);
  if (screenIterator != previousScreenIterator) {
    // cause the loop function is looping every millisecond there is an 'if' that will work only when 'screenIterator' was changed
    if (screenIterator >= 300) {
      screenIterator = 0;
    }
  }
  if (mainLoopCounter != prevMainLoopCounter) {
    Serial.println(mainLoopCounter);
    // handling overflow
    if (mainLoopCounter >= ULONG_MAX - 3) {
      mainLoopCounter = 0;
    }
  }
  prevMainLoopCounter = mainLoopCounter;
  previousScreenIterator = screenIterator;
  if (currentTime - previousTime >= screenUpdateInterval && !showSecondScreen && !showThirdScreen) {
    // first screen with pressure graphic
    previousTime = currentTime;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Temp -> ");
    display.print(temperature);
    display.println(" C");
    display.print("Pres -> ");
    display.print(pressure);
    display.println(" hPa");
    screenIterator++;
    drawPressureGraph();
    display.display();
    if (mainLoopCounter % pressureLogInterval == 0) {  // logging pressure in array
      logPressure(pressure);
      if (pressureArraySize % 12 == 0 && pressureArraySize) {
        forecastReady = true;
      }
    }
    if (mainLoopCounter % temperatureLogInterval == 0) {  // logging temperature in array
      logTemperature(temperature);
    }
    if (screenIterator == 100) {  // moving to second screen after 5 seconds
      showSecondScreen = true;
    }
    mainLoopCounter++;
  }

  if (currentTime - previousTime >= screenUpdateInterval && showSecondScreen) {
    // second screen with temperature graphic
    previousTime = currentTime;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Temp -> ");
    display.print(temperature);
    display.println(" C");
    display.print("Pres -> ");
    display.print(pressure);
    display.println(" hPa");
    drawTemperatureGraph();
    display.display();
    screenIterator++;
    if (mainLoopCounter % pressureLogInterval == 0) {
      logPressure(pressure);
      if (pressureArraySize % 12 == 0 && pressureArraySize) {
        forecastReady = true;
      }
    }
    if (mainLoopCounter % temperatureLogInterval == 0) {
      logTemperature(temperature);
    }
    if (screenIterator == 200) {  // moving to third screen after 5 seconds
      showSecondScreen = false;
      if (forecastReady) {
        showThirdScreen = true;
      } else {
        screenIterator = 0;
      }
    }
    mainLoopCounter++;
  }

  if (currentTime - previousTime >= screenUpdateInterval && showThirdScreen && forecastReady) {
    // third screen with forecast
    previousTime = currentTime;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Temp -> ");
    display.print(temperature);
    display.println(" C");
    display.print("Pres -> ");
    display.print(pressure);
    display.println(" hPa");
    displayForecast();
    display.display();
    screenIterator++;
    if (screenIterator == 300) {
      showThirdScreen = false;
      showSecondScreen = false;
    }
    if (mainLoopCounter % pressureLogInterval == 0) {
      logPressure(pressure);
      if (pressureArraySize % 12 == 0 && pressureArraySize) {
        forecastReady = true;
      }
    }
    if (mainLoopCounter % temperatureLogInterval == 0) {
      logTemperature(temperature);
    }
    mainLoopCounter++;
  }
}
