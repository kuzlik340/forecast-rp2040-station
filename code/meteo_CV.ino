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


unsigned long previous_time = 0; //variable for setting timer for screens
unsigned long int step_loop = 0; //main iterator for loop cycle, created for logging pressure and temperature
unsigned long int step_loop_prev = 0;  
int i = 0;  //iterator for loop cycle, mainly created for changing screens
int previous_iterator = 0;  //variable to save previous 'i' for void loop
int horizon_offset_for_graphics = 27; //horizon offset by pixels for graphics
int volume_pressure_graph = 0;  //size of pressures array
int volume_temperature_graph = 0;  //size of temperatures array
int interval = 50; //time interval in miliseconds to update information on screen
int interval_for_pressure = 36000; //interval for logging pressure into array(this interval keeps logging of pressure every 30 minutes). //36000
int interval_for_temperature = 18000; //interval for logging temperature into array(this interval keeps logging of temperature every 15 minutes). //18000
bool flag_for_forecast = 0;  //forecast will be started only when in pressure array will be at least 11 elements(logs)
bool showSecondScreen = 0;  
bool showThirdScreen = 0;
bool flag_overflow_temp = 0; //overflow of temperatures arrray
bool flag_overflow_pres = 0; //overflow of pressures arrray
float pressure_graph[50];   
float temperature_graph[50];

const unsigned char epd_bitmap_cloudy [] PROGMEM = {
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


const unsigned char epd_bitmap_sunny [] PROGMEM = {
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
  for (int k = 0; k < 50; k++) { //initializing arrays
    pressure_graph[k] = 0; //in pressure we can put '0' while initializing cause atmospheric pressure == 0 only in space :)
  }
  for (int k = 0; k < 50; k++) {
    temperature_graph[k] = -100;  //in temperature we have to initialize by -100, i hope this sensor will never meet this temperature
  }
  if(!bmp.begin()){
    Serial.println("FATERR: bmp280 is not connected or address of BMP is not 0x76");
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println("FATERR: display is not connected or address of diaplay is not 0x3C");
  }
  display.clearDisplay();
}


void show_forecast() {
  int temp = 0;
  if(volume_pressure_graph >= 12){  //by the temp variable we can predict future weather for 8~10 hours
    temp = pressure_graph[volume_pressure_graph - 12] - pressure_graph[volume_pressure_graph-1];
  }
  else if(!volume_pressure_graph){  //an 'else' for safety
    return;
  }
  else{    //an 'else' for safety
    temp = pressure_graph[0] - pressure_graph[volume_pressure_graph-1];
  }
  display.setCursor(0, 57);  //all this text will be replaced by bitmaps and small messages for weather
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  if (temp)  //so if the temp is >0 it means that the pressure was decreasing and it means that the weather is going to be stormy
    display.drawBitmap(0, 20, epd_bitmap_cloudy, 40, 64, SSD1306_WHITE);
  else    //but if it is <0 it means that pressure increased and weather will be better
    display.drawBitmap(0, 20, epd_bitmap_sunny, 40, 64, SSD1306_WHITE);
  display.drawLine(44, 20, 44, 64, SSD1306_WHITE);
  display.drawLine(45, 20, 45, 64, SSD1306_WHITE);
  display.drawLine(127, 20, 127, 64, SSD1306_WHITE);
  display.drawLine(126, 20, 126, 64, SSD1306_WHITE);
  display.drawLine(44, 20, 128, 20, SSD1306_WHITE);
  display.drawLine(44, 21, 128, 21, SSD1306_WHITE);
  display.drawLine(44, 63, 128, 63, SSD1306_WHITE);
  display.drawLine(44, 62, 128, 62, SSD1306_WHITE);
  display.setCursor(48, 28);
  display.print("THE FORECAST");
  display.setCursor(48, 38);
  display.print("MAY NOT BE");
  display.setCursor(48, 48);
  display.print("ACCURATE");

}


void draw_table(){ //function to draw a table for graphic
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
}


void show_estimated_time(int interval_for_time) { //function to show an estimated time
    //after starting a microcontroller we have to wait some time (30 mins for pressure and 15 minutes for temperature)
    //cause there will be no bods on graphic and no logs in arrays
    display.drawLine(0, 20, 128, 20, SSD1306_WHITE);
    display.drawLine(0, 21, 128, 21, SSD1306_WHITE);
    display.drawLine(0, 63, 128, 63, SSD1306_WHITE);
    display.drawLine(0, 62, 128, 62, SSD1306_WHITE);
    display.drawLine(0, 20, 0, 64, SSD1306_WHITE);
    display.drawLine(1, 20, 1, 64, SSD1306_WHITE);
    display.drawLine(127, 20, 127, 64, SSD1306_WHITE);
    display.drawLine(126, 20, 126, 64, SSD1306_WHITE);
    display.setCursor(5, 26);
    display.print("Please wait for data");
    display.setCursor(22, 40);
    display.print("ESTIMATED TIME");
    unsigned long current_time = millis();
    display.setCursor(34, 52);
    display.print(((((interval_for_time) * (interval+10)) - current_time) / 1000) / 60);
    display.print(" minutes");
    display.display();
}


void show_graph_temperature() {  //function to show graphic of temperatures
  if (temperature_graph[0] == -100) { //check if we have logs
    show_estimated_time(interval_for_temperature);
    return;
  }
  int previous = 0; //previous dot on graphic
  int v = volume_temperature_graph-1; 
  int iter; //iterator that is used to go through array
  if(flag_overflow_temp){ 
    //on the graphics is only new information so by handling overflow 
    //second part of array will be rewritten to the first part
    iter = 25;
  }
  else{
    iter = 0;
  }
  while ((v - iter) > 0) {
    int y = 48 - (((int)temperature_graph[iter]) / 2); 
    //a formula to display temperature dot on graphic table
    if (previous != 0) {
      //the line beetween dots have width of 2 pixels
      display.drawLine(horizon_offset_for_graphics - 5, previous, horizon_offset_for_graphics, y, SSD1306_WHITE);
      display.drawLine(horizon_offset_for_graphics - 5, previous + 1, horizon_offset_for_graphics, y + 1, SSD1306_WHITE);
    }
    //the dot is a square 2x2 pixels
    display.drawPixel(horizon_offset_for_graphics, y, SSD1306_WHITE);
    display.drawPixel(horizon_offset_for_graphics + 1, y, SSD1306_WHITE);
    display.drawPixel(horizon_offset_for_graphics, y + 1, SSD1306_WHITE);
    display.drawPixel(horizon_offset_for_graphics + 1, y + 1, SSD1306_WHITE);
    horizon_offset_for_graphics += 4;
    iter+=2;
    previous = y;
  }
  draw_table(); 
  display.setCursor(0, 57);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("-20");  //a lowest border of temperature on graphic
  display.setCursor(4, 23);
  display.print("40");   //a highest
  horizon_offset_for_graphics = 27; 
  previous = 0;
}


void show_graph_pressure() { //function to show graphic of pressures
  if (pressure_graph[0] == 0) { //check if we have logs
    show_estimated_time(interval_for_pressure);
    return;
  }
  int previous = 0; //previous dot on graphic
  int v = volume_pressure_graph-1; 
  int iter;  //iterator that is used to go through array
  if(flag_overflow_pres){
    //on the graphics is only new information so by handling overflow 
    //second part of array will be rewritten to the first part
    iter = 25;
  }
  else{
    iter = 0;
  }
  while ((v - iter) > 0) {
    int y = 64 - (((int)pressure_graph[iter] - 940) / 2);
    //a formula to display pressure dot on graphic table
    if (previous != 0) {
      //the line beetween dots have width of 2 pixels
      display.drawLine(horizon_offset_for_graphics - 5, previous, horizon_offset_for_graphics, y, SSD1306_WHITE);
      display.drawLine(horizon_offset_for_graphics - 5, previous + 1, horizon_offset_for_graphics, y + 1, SSD1306_WHITE);
    }
    //the dot is a square 2x2 pixels
    display.drawPixel(horizon_offset_for_graphics, y, SSD1306_WHITE);
    display.drawPixel(horizon_offset_for_graphics + 1, y, SSD1306_WHITE);
    display.drawPixel(horizon_offset_for_graphics, y + 1, SSD1306_WHITE);
    display.drawPixel(horizon_offset_for_graphics + 1, y + 1, SSD1306_WHITE);
    horizon_offset_for_graphics += 4;
    iter+=2;
    previous = y;
  }
  draw_table();
  display.setCursor(0, 57);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("950");  //a lowest border of pressure on graphic
  display.setCursor(0, 23);
  display.print("1015"); //a highest
  horizon_offset_for_graphics = 27;
  previous = 0;
}


void check_overflow_array(float array[50]) {
  //in this function we are overriding the logs from end of array to the first part
  //so we can place new logs
   for (int j = 25; j < 49; j++) { 
      array[j - 25] = array[j];
      array[j] = 0;
   }
   if(array[0] < 900) //checking if that was an array of pressures or temperatures
   flag_overflow_temp = 1;
   else
   flag_overflow_pres = 1;
}


void log_pressure(float pressure) {
  pressure_graph[volume_pressure_graph] = pressure;
  volume_pressure_graph++;
  if (volume_pressure_graph == 50) {
    check_overflow_array(pressure_graph);
    volume_pressure_graph = 25;
  }
}


void log_temperature(float temperature) {
  temperature_graph[volume_temperature_graph] = temperature;
  volume_temperature_graph++;
  if (volume_temperature_graph == 50) {
    check_overflow_array(temperature_graph);
    volume_temperature_graph = 25;
  }
}


void loop() {
  unsigned long current_time = millis(); //i mot using a delays in this code cause they are slowing the work of microcontroller
  //instead i used millis
  float temperature = bmp.readTemperature();
  float pressure = (bmp.readPressure() / 100.0F);
  if (i != previous_iterator) { 
    //cause the loop function is looping every millisecond 
    //there is an 'if' that will work only when 'i' was changed
    if (i >= 300) {
       i = 0;
    }
  }
  if(step_loop != step_loop_prev){
    Serial.println(step_loop);
    //handling overflow
    if (step_loop >= ULONG_MAX-3) {
       step_loop = 0;
    }
  }
  step_loop_prev = step_loop;
  previous_iterator = i;
  if (current_time - previous_time >= interval && !showSecondScreen && !showThirdScreen) {
    //first screen with pressure graphic
    previous_time = current_time;
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
    step_loop++;
    show_graph_pressure();
    display.display();
    if (step_loop % interval_for_pressure == 0) {  //logging pressure in array
      log_pressure(pressure);
      if (volume_pressure_graph % 12 == 0 && volume_pressure_graph) {
        flag_for_forecast = 1;
      }
    }
    if (step_loop % interval_for_temperature == 0) { //logging tempearture in array
      log_temperature(temperature);
    }
    if (i == 100) {  //moving to second screen after 5 seconds
      showSecondScreen = 1;
    }
  }

  if (current_time - previous_time >= interval && showSecondScreen) {
    //second screen with temperature graphic
    previous_time = current_time;
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
    show_graph_temperature();
    display.display();
    i++;
    step_loop++;
    if (step_loop % interval_for_pressure == 0) {  
      log_pressure(pressure);
      if (volume_pressure_graph % 12 == 0 && volume_pressure_graph) {
        flag_for_forecast = 1;
      }
    }
    if (step_loop % interval_for_temperature == 0) {
      log_temperature(temperature);
    }
    if (i == 200) { //moving to third screen after 5 seconds
      showSecondScreen = 0;  
      if (flag_for_forecast) {
        showThirdScreen = 1; 
      }
      else{
        i = 0;
      }
    }
  }
  if (current_time - previous_time >= interval && showThirdScreen && flag_for_forecast) {
    //third screen with forecast
    previous_time = current_time;
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
    show_forecast();
    display.display();
    i++;
    step_loop++;
    if (i == 300) {
      showThirdScreen = 0;
      showSecondScreen = 0;
    }
    if (step_loop % interval_for_pressure == 0) {  
      log_pressure(pressure);
      if (volume_pressure_graph % 12 == 0 && volume_pressure_graph) {
        flag_for_forecast = 1;
      }
    }
    if (step_loop % interval_for_temperature == 0) {
      log_temperature(temperature);
    }
  }
}
