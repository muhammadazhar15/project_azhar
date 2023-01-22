#include <Wire.h>
#include <Adafruit_SSD1306.h> // Memanggil Library OLED SSD1306

#define SCREEN_WIDTH 128 // Lebar Oled dalam Pixel
#define SCREEN_HEIGHT 32 // Tinggi Oled dalam Pixel

#define OLED_RESET     -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int sensor[2];
int threshold[2] = {750,750};
byte nilai_sensor;
int posisi;
byte v = 3;
int sp =0;
double kp=10,ki=0,kd=25;
double output_motor[2];
//Interrupt variabel
hw_timer_t * timer = NULL;
//Program interrupt
void IRAM_ATTR onTime() {
  sensor[0] = analogRead(35);
  sensor[1] = analogRead(34);

  nilai_sensor=0;
  for(byte i=0;i<2;i++)
  {
   if(sensor[i]>threshold[i]) nilai_sensor |= 1 << i;
   else nilai_sensor |= 0 << i;
  }
  nilai_sensor |= 1 << 7;

  switch(nilai_sensor)
  {
    case 0b10000010: posisi=-1; break;
    case 0b10000011: posisi=0; break;
    case 0b10000001: posisi=1; break;
  }

  static int error,prev_error;
  static double p,i,d,output;
  error = sp - posisi;
  p = kp*error;
  i += ki*error;
  d = kd*(error-prev_error);
  prev_error = error;

  output = p + i + d;
  output_motor[0]=v-output;
  output_motor[1]=v+output;

  if(output_motor[0]>6)output_motor[0]=6;
  else if(output_motor[0]<-6)output_motor[0]=-6;
  if(output_motor[1]>6)output_motor[1]=6;
  else if(output_motor[1]<-6)output_motor[1]=-6;

  if(output_motor[0]>0)
  {
    ledcWrite(0, abs(output_motor[0]));
    ledcWrite(1, 0);
  }
  else
  {
    ledcWrite(0, 0);
    ledcWrite(1, abs(output_motor[0])); 
  }
  
  if(output_motor[1]>0)
  {
    ledcWrite(2, abs(output_motor[1]));
    ledcWrite(3, 0); 
  }
  else
  {
    ledcWrite(2, 0);
    ledcWrite(3, abs(output_motor[1]));   
  }
}

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
  display.println(F("...INIT ROBOT..."));       //Menampilkan Tulisan RobotikIndonesia
  display.display();                      //Mulai Menamp
  delay(1000);
  //Motor dc
  ledcSetup(0, 10000, 8);
  ledcAttachPin(5, 0);
  ledcSetup(1, 10000, 8);
  ledcAttachPin(15, 1);
  ledcSetup(2, 10000, 8);
  ledcAttachPin(4, 2);
  ledcSetup(3, 10000, 8);
  ledcAttachPin(2, 3);
  //Servo
//  ledcSetup(4, 50, 8);
//  ledcAttachPin(27, 4);
//  ledcSetup(5, 50, 8);
//  ledcAttachPin(26, 5);

  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
//  ledcWrite(4, 15); // semakin kecil => terbuka
//  ledcWrite(5, 15); // semakin kecil => terangkat
  //Init Button
  pinMode(23, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);

  Serial.begin(9600);
  analogReadResolution(10);

  //Init Interrupt
  timer = timerBegin(0, 80, true); //80MHz => 80,000,000 / 80 = 1,000,000 ticks / second               
  timerAttachInterrupt(timer, &onTime, true);    
  //Fire Interrupt ( 1 = 1us )
  timerAlarmWrite(timer, 1000, true);      
  timerAlarmEnable(timer);
}

void loop() {
  display.clearDisplay();                 //Membersihkan tampilan
  display.setCursor(0,0);                 // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.print(F("P:"));       //Menampilkan Tulisan RobotikIndonesia
  display.print(kp);
  display.setCursor(0,8);                 // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.print(F("I:"));       //Menampilkan Tulisan RobotikIndonesia
  display.print(ki);
  display.setCursor(0,16);                 // Koordinat awal tulisan (x,y) dimulai dari atas-kiri
  display.print(F("D:"));       //Menampilkan Tulisan RobotikIndonesia
  display.print(kd);
  display.setCursor(0,24);
  display.print("setting");
  display.display();
   
  if(digitalRead(23)==0)kp+=0.1;
  else if(digitalRead(19)==0)kp-=0.1;    
  if(digitalRead(18)==0)kd+=0.1;
  else if(digitalRead(32)==0)kd-=0.1;
  delay(500);
}
