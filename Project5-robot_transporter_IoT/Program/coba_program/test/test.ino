#include <Arduino.h>
#include "OTA.h"
#include "myWIFI.h"
#include "myMQTT.h"
#include "myOLED.h"

//OLED variabel
byte cnt;
// MQTT variabel
char kirim[40];
float pres = 1;
//Interrupt variabel
hw_timer_t * timer = NULL;
//ADC variabel
int adc[4];

//Program interrupt
void IRAM_ATTR onTime() {
  cnt++;
  adc[0] = analogRead(34);
  adc[1] = analogRead(35);
  adc[2] = analogRead(36);
  adc[3] = analogRead(39);
}

//main program
void setup() {
  //Init serial
  Serial.begin(115200);
  Serial.println("Booting");
  //Init OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // alamat I2C 0x3C untuk 128x32
    Serial.println(F("SSD1306 Gagal"));
    for(;;); // mengulang terus, hingga bisa menghubungkan ke I2C Oled
  }
  else {
    display.display();
    delay(1000);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print(F("...INIT ROBOT MA..."));
    display.display();
  }
  //Init OTA, WIFI, dan MQTT
  setupOTA("ROBOT_MA", mySSID, myPASSWORD);
  setupMQTT();
  //Init Servo
  ledcSetup(4, 50, 8);
  ledcAttachPin(27, 4);
  ledcSetup(5, 50, 8);
  ledcAttachPin(26, 5);
  ledcWrite(4, 5); // semakin kecil => terbuka
  ledcWrite(5, 14); // semakin kecil => terangkat
  
  display.clearDisplay();
  display.drawBitmap(0, 0, logologo, 128, 32, WHITE);
  display.display();
  delay(2000);

  //Init Interrupt
  timer = timerBegin(3, 80, true); //80MHz => 80,000,000 / 80 = 1,000,000 ticks / second               
  timerAttachInterrupt(timer, &onTime, true);    
  //Fire Interrupt ( 1 = 1us )
  timerAlarmWrite(timer, 20000, true);      
  timerAlarmEnable(timer);
}

void loop() {
  //OTA
  ArduinoOTA.handle();
  
  //MQTT
  if (!mqttClient.connected()){
    reconnect();
  }
  sprintf(kirim, "%f", pres);
  mqttClient.publish("/ROBOT_MA/send", kirim);
  pres++;
  mqttClient.publish("/ROBOT_MA/send", "wow");
  mqttClient.loop();

  //OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.print(adc[0]);
  display.setCursor(0,8);
  display.print(adc[1]);
  display.setCursor(0,16);
  display.print(adc[2]);
  display.setCursor(0,24);
  display.print(adc[3]);
  display.display();
  delay(1000);
}
