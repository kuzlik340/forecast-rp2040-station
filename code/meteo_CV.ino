#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BMP280 bmp;
int i = 0;
int horizon_offset = 0;
int h = 27;
int hr = 0;
int previous = 0;
int m = 0;
int a = 0;
int over = 0;
bool flag_for_forecast = 0;
bool showSecondScreen = 0;
float pressure_graf[1000];
float temperature_graf[1000];
unsigned long sec_previous_for_graphic = 0;
unsigned long sec_previous_for_terminal = 0;
int interval = 50; //5000

void setup() {
  Serial.begin(115200); 
  for (int k = 0; k < 999; k++) {
    pressure_graf[k] = 0;
  }
  for (int k = 0; k < 999; k++) {
    temperature_graf[k] = 0;
  }
  bmp.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
}

int show_forecast(float pressure_graf[50]){
  return pressure_graf[m] - pressure_graf[m-12];
}


void show_graph_temperature(float tempearture_graf[50]){
  
}

void show_graph_pressure(float pressure_graf[50]) {
  unsigned long current_sec = millis();
  int v = hr;
  while (pressure_graf[v] != 0) {
    int y = 64 - (((int)pressure_graf[v] - 940) / 2);
    if (previous != 0 && v != hr) {
      display.drawLine(h - 5, previous, h, y, SSD1306_WHITE);
      display.drawLine(h - 5, previous + 1, h, y + 1, SSD1306_WHITE);
    }
    display.drawPixel(h, y, SSD1306_WHITE);
    display.drawPixel(h + 1, y, SSD1306_WHITE);
    display.drawPixel(h, y + 1, SSD1306_WHITE);
    display.drawPixel(h + 1, y + 1, SSD1306_WHITE);
    h += 4;
    if (h >= 120) {
      hr = v;
      previous = 0;
      h = 27;       
      break;
    } 
    else {
      v++;
      previous = y;
    }
  }
  display.drawLine(25, 20, 25, 64, SSD1306_WHITE);
  display.drawLine(26, 20, 26, 64, SSD1306_WHITE);
  display.drawLine(25, 20, 123, 20, SSD1306_WHITE);
  display.drawLine(25, 21, 123, 21, SSD1306_WHITE);
  display.drawLine(25, 63, 123, 63, SSD1306_WHITE);
  display.drawLine(25, 64, 123, 64, SSD1306_WHITE);
  display.drawLine(123, 20, 123, 64, SSD1306_WHITE);

  for (int g = 38; g < 128; g += 12) {
    display.drawLine(g, 20, g, 64, SSD1306_WHITE);
  }

  display.setCursor(0, 57);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("950");
  display.setCursor(0, 23);
  display.print("1020");
  h = 27;
}

void screen1(float temperature, float pressure){
    
}

void loop() {
  unsigned long current_sec = millis();
  float temperature = bmp.readTemperature();
  float pressure = (bmp.readPressure() / 100.0F);  
  int forecast = 0;
  if (current_sec - sec_previous_for_terminal >= interval && !showSecondScreen) {
    sec_previous_for_terminal = current_sec;
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
    i++;
    show_graph_pressure(pressure_graf);
    display.display();
    if (i % 10 == 0) { //???
      pressure_graf[m] = pressure;
      m++;
    }
    if (i % 50 == 0) {
      temperature_graf[m] = temperature;
      a++;
    }
  }
  

  if (a == 50) {
    Serial.println("Overflow array");
    for (int j = 25; j < 49; j++) {
      temperature_graf[j - 25] = temperature_graf[j];
      temperature_graf[j] = 0;
    }
    a = 25;
  }
  if(m % 12 == 0){
    flag_for_forecast = 1; 
    forecast =  show_forecast(pressure_graf);   
  }
  if (m == 46) {
    Serial.println("Overflow array");
    for (int j = 25; j < 46; j++) {
      pressure_graf[j - 25] = pressure_graf[j];
      pressure_graf[j] = 0;
    }
    hr = 20;
    m = 25;
  }
}
