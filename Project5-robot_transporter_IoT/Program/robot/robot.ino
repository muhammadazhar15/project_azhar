#include <EEPROM.h>
#include "myOLED.h"

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "AAsus-X550Z";
const char* password = "1234567890";
//const char* ssid = "yahyakrs_";
//const char* password = "12345678";
// MQTT client
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient); 
char *mqttServer = "broker.hivemq.com";
int mqttPort = 1883;
char kirim[40];
String ambil = "Take";
String zona = "Drop Zone";
String lepas = "Droping";
String misi = "Misi Start";

void connectToWiFi() {
  Serial.print("Connectiog to ");
 
  WiFi.begin(ssid, password);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println(" ");
}

void setupMQTT(){
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
      Serial.println("Reconnecting to MQTT Broker..");
      String clientId = "ESP32Client-"; 
      clientId += String(random(0xffff), HEX);
      if (mqttClient.connect(clientId.c_str())) {
        Serial.println("Connected.");
        // subscribe to topic
        mqttClient.subscribe("/robot_ma/receive");
      }  
  }
}

//Display variabel
byte status_menu,tombol_menu;
byte status_sub_menu, tombol_sub_menu;
byte timer_tombol;
//Motion variabel
unsigned int timer_delay;
unsigned int status_jalan;
//ADC variabel
byte sensor_aktif = 0;
byte status_kalibrasi;
int sensor[4];
int min_sensor[4]={1023,1023,1023,1023};
int max_sensor[4]={0,0,0,0};
int threshold[4];
byte threshold_8bit[4];
byte nilai_sensor;
int posisi;
//PID variabel
byte v=3,vmax=6;
int sp =0;
double kp=5.7,ki=0.0,kd=2;
double output_motor[2];
byte status_pid = 0;
//TCS3200 variabel
#define S2 12
#define S3 14
#define OUT 13
int color[3];
//GM66 variabel
String kode;
String drop;
byte tanda;
int balas;
//Interrupt variabel
hw_timer_t * timer = NULL;
void IRAM_ATTR onTime(void);

void setup() {
  //Init OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // alamat I2C 0x3C untuk 128x32
    Serial.println(F("SSD1306 Gagal"));
    for(;;); // mengulang terus, hingga bisa menghubungkan ke I2C Oled
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print(F("...INIT ROBOT MA..."));
  display.display();
  delay(1000);
  //Init serial monitor, WiFi, MQTT, ADC, dan EEPROM
  Serial.begin(9600);
  connectToWiFi();
  setupMQTT();
  if (!mqttClient.connected()){
    reconnect();
  }
  analogReadResolution(10);
  EEPROM.begin(512);
  for(byte k=0;k<4;k++) {
    threshold_8bit[k]=EEPROM.read(k);
    threshold[k]=map(threshold_8bit[k],0,255,0,1023);
  }
  //Init Button
  pinMode(23, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  //Motor dc
  ledcSetup(0, 10000, 8);
  ledcAttachPin(5, 0);
  ledcSetup(1, 10000, 8);
  ledcAttachPin(15, 1);
  ledcSetup(2, 10000, 8);
  ledcAttachPin(4, 2);
  ledcSetup(3, 10000, 8);
  ledcAttachPin(2, 3);
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  //Init Servo
  ledcSetup(4, 50, 8);
  ledcAttachPin(27, 4);
  ledcSetup(5, 50, 8);
  ledcAttachPin(26, 5);
  ledcWrite(4, 17); // semakin kecil => terbuka
  ledcWrite(5, 5); // semakin kecil => terangkat
  //Init TCS3200
  pinMode(S2, OUTPUT); //S2
  pinMode(S3, OUTPUT); //S3
  pinMode(OUT, INPUT);  //OUT
  //Init GM66
  Serial2.begin(115200);
  Serial2.setTimeout(100);
  //END Init
  display.clearDisplay();
  display.drawBitmap(0, 0, logologo, 128, 32, WHITE);
  display.display();
  delay(1000);

  //Init Interrupt
  timer = timerBegin(3, 80, true); //80MHz => 80,000,000 / 80 = 1,000,000 ticks / second               
  timerAttachInterrupt(timer, &onTime, true);    
  //Fire Interrupt ( 1 = 1us )
  timerAlarmWrite(timer, 1000, true);      
  timerAlarmEnable(timer);
}

void loop() {
  //MQTT Client
  mqttClient.loop();
  //Display OLED
  if(status_menu==0) {
    if(status_sub_menu==0) {
      display.clearDisplay();
      display.setCursor(0,0); 
      display.print(F("P:"));
      display.print(kp,1);
      display.setCursor(0,8);
      display.print(F("I:"));
      display.print(ki,1);
      display.setCursor(0,16);
      display.print(F("D:"));
      display.print(kd,1);
      display.setCursor(0,24);
      display.print(F("..READY TO START.."));
      display.display();
      delay(1000);
    }
    else if(status_sub_menu==1) {
      display.clearDisplay();
      display.setCursor(0,0); 
      display.print(F(".....PLAYING....."));
      display.display();
      status_sub_menu=2;
    }
    else if(status_sub_menu==2) {
      /////////MOTION ROBOT/////////////////////
      switch(status_jalan) {
        case 0:
        read_barcode();
        status_jalan++; timer_delay=5000; 
        break;
        case 1:
        if(timer_delay!=0) {
          while (Serial2.available() > 0) {
            if(tanda<7){
              balas = Serial2.read();
              tanda++;
            }
            else {
              Serial2.readString();
              misi.toCharArray(kirim,40);
              mqttClient.publish("/robot_ma/send", kirim);
              tanda=0; 
              manual_kanan(0,5); status_jalan++;
            }
          }
        }
        else {
          tanda=0; status_jalan--;
        }
        break;
        
        case 2:
        if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
           status_pid=1; status_jalan++;
        }
        break;
        case 3: 
        if(color[0]-color[1]<-10) {
          status_jalan++; timer_delay=150; 
        }
        break;
        case 4:
        if(timer_delay==0) {
          v=0;
          delay(1000);
          status_pid=0; 
          manual_diam(); status_jalan++;
        }
        break;
        case 5:
        read_barcode();
        status_jalan++; timer_delay=6000; 
        break;
        case 6:
        if(timer_delay!=0) {
          while (Serial2.available() > 0) {
            if(tanda<7){
              balas = Serial2.read();
              tanda++;
            }
            else {
              kode = Serial2.readString();
              ambil = ambil+" "+kode;
              ambil.toCharArray(kirim,40);
              mqttClient.publish("/robot_ma/send", kirim);
              ambil="Take";
              tanda=0; status_jalan++;
            }
          }
        }
        else {
          tanda=0;
          manual_kiri(0,15);
          delay(50);
          manual_diam(); status_jalan--;
        }
        break;
        case 7:
        take_box(); status_jalan++;
        timer_delay=400;
        break;
        case 8:
        manual_kiri(15,0);
        if(timer_delay==0) {
          manual_kiri(0,4); status_jalan++;
        }
        break;
        case 9:
        if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
          manual_diam();
          status_pid=1; 
          delay(1000); status_jalan++;
          timer_delay=2000;
        }
        break;
        case 10:
        v=3;
        if(timer_delay==0){
          v=0;
          delay(1000); 
          status_pid=0;
          manual_diam(); status_jalan++;
          timer_delay=300;
        }
        break;
        case 11:
        manual_maju(5,30);
        if(timer_delay==0){
          if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
            manual_diam(); status_jalan++;
            timer_delay=100;
          }
        }
        break;
        case 12:
        manual_kiri(0,5);
        if(timer_delay==0) {
          if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
            manual_diam();
            status_pid=1; 
            delay(1000); 
            v=3; status_jalan++;
          }
        }
        break;
        case 13: 
        if(color[0]-color[1]<-10) {
          status_jalan++; timer_delay=60; 
        }
        break;
        case 14:
        if(timer_delay==0) {
          v=0;
          delay(1000);
          status_pid=0; 
          manual_diam(); status_jalan++;
        }
        break;
        case 15:
        read_barcode();
        status_jalan++; timer_delay=6000; 
        break;
        case 16:
        if(timer_delay!=0) {
          while (Serial2.available() > 0) {
            if(tanda<7){
              balas = Serial2.read();
              tanda++;
            }
            else {
              drop = Serial2.readString();
              zona = zona+" "+drop;
              zona.toCharArray(kirim,40);
              mqttClient.publish("/robot_ma/send", kirim);
              zona="Drop zone";
              tanda=0; status_jalan++;
            }
          }
        }
        else {
          tanda=0;
          manual_kiri(0,15);
          delay(50);
          manual_diam(); status_jalan--;
        }
        break;
        case 17:
        if(kode==drop) {
          lepas = lepas+" "+drop;
          lepas.toCharArray(kirim,40);
          mqttClient.publish("/robot_ma/send", kirim);
          lepas="Droping";
          drop_box(); 
          manual_mundur(5,5);
          delay(100); status_jalan++;
          timer_delay=500;
        }
        break;
        case 18:
        manual_kiri(15,0);
        if(timer_delay==0) {
          manual_kiri(0,4); status_jalan++;
        }
        break;
        case 19:
        if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
          status_pid=1;
          v=3; 
          delay(1000); status_jalan++;
        }
        break;
        case 20:
        if(color[0]-color[1]<-10) {
          status_jalan++; 
          timer_delay=150; 
        }
        break;
        case 21:
        if(timer_delay==0) {
          v=0;
          delay(1000);
          status_pid=0; 
          manual_diam(); status_jalan++;
        }
        break;
        case 22:
        read_barcode();
        status_jalan++; timer_delay=6000; 
        break;
        case 23:
        if(timer_delay!=0) {
          while (Serial2.available() > 0) {
            if(tanda<7){
              balas = Serial2.read();
              tanda++;
            }
            else {
              kode = Serial2.readString();
              ambil = ambil+" "+kode;
              ambil.toCharArray(kirim,40);
              mqttClient.publish("/robot_ma/send", kirim);
              ambil="Take";
              tanda=0; status_jalan++;
            }
          }
        }
        else {
          tanda=0;
          manual_kiri(0,15);
          delay(50);
          manual_diam(); status_jalan--;
        }
        break;
        case 24:
        v=0; //coba
        take_box(); status_jalan++;
        timer_delay=500;
        break;
        case 25:
        manual_kiri(10,5);
        if(timer_delay==0) {
          sensor_aktif=1; status_jalan++;
        }
        break;
        case 26:
        manual_kiri(0,5);
        if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
          manual_diam();
          status_pid=1;
          delay(1000); 
          v=3; status_jalan++;
          timer_delay=500;
        }
        break;
        case 27:
        if(timer_delay==0) {
          v=0;
          delay(1000);
          status_pid=0;
          manual_maju(3,70);
          sensor_aktif=0;
          delay(200);status_jalan++;
        }
        break;
        case 28:
        if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
          status_pid=1;
          delay(1000);
          v=3; status_jalan++;
        }
        break;
        case 29:
        if(color[0]-color[1]<-10) {
          status_jalan++; 
          timer_delay=150; 
        }
        break;
        case 30:
        if(timer_delay==0) {
          v=0;
          delay(1000);
          status_pid=0; 
          manual_diam(); status_jalan++;
        }
        break;
        case 31:
        read_barcode();
        status_jalan++; timer_delay=6000; 
        break;
        case 32:
        if(timer_delay!=0) {
          while (Serial2.available() > 0) {
            if(tanda<7){
              balas = Serial2.read();
              tanda++;
            }
            else {
              drop = Serial2.readString();
              zona = zona+" "+drop;
              zona.toCharArray(kirim,40);
              mqttClient.publish("/robot_ma/send", kirim);
              zona="Drop zone";
              tanda=0; status_jalan++;
            }
          }
        }
        else {
          tanda=0;
          manual_kiri(0,15);
          delay(50);
          manual_diam(); status_jalan--;
        }
        break;
        case 33:
        if(kode==drop) {
          lepas = lepas+" "+drop;
          lepas.toCharArray(kirim,40);
          mqttClient.publish("/robot_ma/send", kirim);
          lepas="Droping";
          drop_box(); 
          manual_mundur(5,5);
          delay(100); status_jalan++;
          timer_delay=500;
        }
        break;
        case 34:
        manual_kiri(15,0);
        if(timer_delay==0) {
          manual_kiri(0,4); status_jalan++;
        }
        break;
        case 35:
        if(bitRead(nilai_sensor,1)|bitRead(nilai_sensor,0)==1) {
          status_pid=1;
          delay(1000);
          v=3; status_jalan++;
        }
        break;
        case 36:
        if(color[2]-color[0]<-10) {
          status_pid=0;v=0;
          manual_maju(5,30);
          delay(200);
          status_jalan=100; 
        }
        break;

        case 100:
        status_pid=0;v=0;manual_diam();
        break;
            
//        default: status_pid=0; break;
      }
    }
    else status_sub_menu=0;
  }
  else if(status_menu==1) {
    if(status_sub_menu==0) {
      display.clearDisplay();
      display.setCursor(0,0); 
      display.print(F("<<:"));
      display.print(sensor[1]);
      display.setCursor(20,8);
      display.print(F("<:"));
      display.print(sensor[0]);
      display.setCursor(50,16);
      display.print(F(">:"));
      display.print(sensor[3]);
      display.setCursor(70,24);
      display.print(F(">>:"));
      display.print(sensor[2]);
      display.display();
      delay(1000);
    }
    else if(status_sub_menu==1) {
      status_kalibrasi=1;
      display.clearDisplay();
      display.setCursor(0,0); 
      display.print(F("KALIBRASI"));
      display.setCursor(0,8); 
      display.print(analogRead(39));
      display.setCursor(20,16);
      display.print(analogRead(36));
      display.setCursor(50,8);
      display.print(analogRead(34));
      display.setCursor(70,16);
      display.print(analogRead(35));
      display.display();
      delay(1000);
    }
    else if(status_sub_menu==2) {
      status_kalibrasi=0;
      display.clearDisplay();
      display.setCursor(0,0); 
      display.print(F("SAVING......"));
      display.display();
      timerAlarmDisable(timer);
      timerDetachInterrupt(timer);
      timerEnd(timer);
      for(byte k=0;k<4;k++) {
        threshold[k]=min_sensor[k]+((max_sensor[k]-min_sensor[k])*4/5);
        threshold_8bit[k]=map(threshold[k],0,1023,0,255);
        EEPROM.write(k,threshold_8bit[k]);
        EEPROM.commit();
        min_sensor[k]=1023;max_sensor[k]=0;
      }
      delay(2000);
      timer = timerBegin(3, 80, true);         
      timerAttachInterrupt(timer, &onTime, true);    
      timerAlarmWrite(timer, 1000, true);      
      timerAlarmEnable(timer);
      status_sub_menu++;
    }
    else status_sub_menu=0;
  }
  else if(status_menu==2) {
    display.clearDisplay();
    display.setCursor(0,0); 
    display.print(F("R:"));
    display.print(color[0]);
    display.setCursor(0,8);
    display.print(F("G:"));
    display.print(color[1]);
    display.setCursor(0,16);
    display.print(F("B:"));
    display.print(color[2]);
    display.display();
    delay(1000);
  }
}

//Program interrupt
void IRAM_ATTR onTime() {
  ////////////////////TIMER-TIMER////////////////////////////////
  if(timer_delay!=0)timer_delay--;
  ///////////////////TOMBOL/////////////////////////////////////////
  if(timer_tombol==0) {
    if(tombol_menu==1&&digitalRead(18)!=tombol_menu) {
      status_menu++; status_sub_menu=0;
      if(status_menu>2)status_menu=0;
    }
    tombol_menu = digitalRead(18);
    if(tombol_sub_menu==1&&digitalRead(32)!=tombol_sub_menu) {
      status_sub_menu++;
    }
    tombol_sub_menu=digitalRead(32);
  }
  if(timer_tombol!=0)timer_tombol--;
  else timer_tombol = 50;
  ////////////////////READ color///////////////////////////////
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  color[0] = pulseIn(OUT, LOW); //red
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  color[1] = pulseIn(OUT, LOW); //green
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  color[2] = pulseIn(OUT, LOW); //blue
  ///////////////////ADC SENSOR GARIS/////////////////////////////////////
  sensor[0] = analogRead(36);
  sensor[1] = analogRead(39);
  sensor[2] = analogRead(35);
  sensor[3] = analogRead(34);
  if(status_kalibrasi==0) {
    nilai_sensor=0;
    for(byte i=0;i<2;i++) {
     if(sensor[i+(2*sensor_aktif)]>threshold[i+(2*sensor_aktif)]) nilai_sensor |= 1 << i;
     else nilai_sensor |= 0 << i;
    }
    nilai_sensor |= 1 << 7;
    switch(nilai_sensor){
      case 0b10000010: posisi=-1; break;
      case 0b10000011: posisi=0; break;
      case 0b10000001: posisi=1; break;
    }
  }
  else if(status_kalibrasi==1)
  {
    for(byte i=0;i<4;i++)
    {
     if(min_sensor[i]>sensor[i])min_sensor[i]=sensor[i];
     if(max_sensor[i]<sensor[i])max_sensor[i]=sensor[i];
    }
  }
  
  ////////////////PID Process////////////////////////
  static int error,prev_error;
  static double p,i,d,output;

  if(status_pid==1) {
    error = sp - posisi;
    p = kp*error;
    i += ki*error;
    d = kd*(error-prev_error);
    prev_error = error;
  
    output = p + i + d;
    output_motor[0]=v-output;
    output_motor[1]=v+output;
  
    if(output_motor[0]>vmax)output_motor[0]=vmax;
    else if(output_motor[0]<-vmax)output_motor[0]=-vmax;
    if(output_motor[1]>vmax)output_motor[1]=vmax;
    else if(output_motor[1]<-vmax)output_motor[1]=-vmax;
  }
  else if(status_pid==0) {
    p = 0;
    i = 0;
    d = 0;
    error = 0;
    prev_error = 0;
    output = 0;
    output_motor[0] = output_motor[1] = 0;
  }

  if(status_pid==1) {
    if(output_motor[0]>0) {
      ledcWrite(0, abs(output_motor[0]));
      ledcWrite(1, 0);
    }
    else {
      ledcWrite(0, 0);
      ledcWrite(1, abs(output_motor[0])); 
    }
    
    if(output_motor[1]>0) {
      ledcWrite(2, abs(output_motor[1]));
      ledcWrite(3, 0); 
    }
    else {
      ledcWrite(2, 0);
      ledcWrite(3, abs(output_motor[1]));   
    }
  } 
}
