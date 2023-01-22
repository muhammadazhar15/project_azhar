#include <Wire.h>
#include <Adafruit_SSD1306.h> // Memanggil Library OLED SSD1306

#define SCREEN_WIDTH 128 // Lebar Oled dalam Pixel
#define SCREEN_HEIGHT 32 // Tinggi Oled dalam Pixel

#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define S2 12
#define S3 14
#define OUT 13
int color[3];

int adc[4];              //Mendeklarasi varibel dataInt
byte button[4];

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // alamat I2C 0x3C untuk 128x32
    Serial.println(F("SSD1306 Gagal"));
    for(;;); // mengulang terus, hingga bisa menghubungkan ke I2C Oled
  }
  display.display();
  delay(1000);
  display.clearDisplay();                 //Membersihkan tampilan
  display.setTextSize(1);                 //Ukuran tulisan
  display.setTextColor(SSD1306_WHITE);    //Warna Tulisan
  display.setCursor(0,0);                 // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.println(F("INIT ROBOT"));       //Menampilkan Tulisan RobotikIndonesia
  display.display();                      //Mulai Menampilkan

  //Init ADC
  analogReadResolution(10);
  //Init Button
  pinMode(23, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  //Init TCS3200
  pinMode(S2, OUTPUT); //S2
  pinMode(S3, OUTPUT); //S3
  pinMode(OUT, INPUT);  //OUT
  //Init Servo
  ledcSetup(4, 50, 8);
  ledcAttachPin(27, 4);
  ledcSetup(5, 50, 8);
  ledcAttachPin(26, 5);
  ledcWrite(4, 5); // semakin kecil => terbuka
  ledcWrite(5, 14); // semakin kecil => terangkat
  //Init Motor dc
  ledcSetup(0, 10000, 8); //Motor kanan
  ledcAttachPin(15, 0);
  ledcSetup(1, 10000, 8); //Motor kanan
  ledcAttachPin(5, 1);
  ledcSetup(2, 10000, 8); //Motor kiri
  ledcAttachPin(2, 2);
  ledcSetup(3, 10000, 8); //Motor kiri
  ledcAttachPin(4, 3);
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void loop(){
  adc[0] = analogRead(34);
  adc[1] = analogRead(35);
  adc[2] = analogRead(36);
  adc[3] = analogRead(39);

  button[0] = digitalRead(23);
  button[1] = digitalRead(19);
  button[2] = digitalRead(18);
  button[3] = digitalRead(32);

  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  color[0] = pulseIn(OUT, LOW); //red
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  color[1] = pulseIn(OUT, LOW); //green
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  color[2] = pulseIn(OUT, LOW); //blue
  
  display.clearDisplay();                 
  display.setCursor(0,0);
  display.print("34:"); 
  display.print(adc[0]);
  display.setCursor(0,8);
  display.print("35:"); 
  display.print(adc[1]);
  display.setCursor(0,16);
  display.print("36:"); 
  display.print(adc[2]);
  display.setCursor(0,24);
  display.print("39:"); 
  display.print(adc[3]);

  display.setCursor(48,0);
  display.print("23:"); 
  display.print(button[0]);
  display.setCursor(48,8);
  display.print("19:"); 
  display.print(button[1]);
  display.setCursor(48,16);
  display.print("18:"); 
  display.print(button[2]);
  display.setCursor(48,24);
  display.print("32:"); 
  display.print(button[3]);

  display.setCursor(75,0);
  display.print("R:"); 
  display.print(color[0]);
  display.setCursor(75,8);
  display.print("G:"); 
  display.print(color[1]);
  display.setCursor(75,16);
  display.print("B:"); 
  display.print(color[2]);
  display.display();                      
  delay(100);
}
