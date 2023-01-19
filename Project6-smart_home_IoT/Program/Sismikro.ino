#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SimpleTimer.h>

int relay0 = 2;
int relay1 = 4;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "0w_Po-tF6sorbKJd5GFDn8bUkqw27R3r";
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "ABojar";
char pass[] = "1234567890";

SimpleTimer timer;
int pin, tombol0, tombol1;
String kondisi;
String data_str;
String angka, perintah;
byte status_ambil,pengurang;
byte status_timer;

BLYNK_CONNECTED() 
{
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}
BLYNK_WRITE(V0)
{
  data_str = param.asStr();
  for(int i=0; i<data_str.length(); i++)
    { 
      if(String(data_str[i])=="+") {status_ambil=1;pengurang=i+1;}
      else if(status_ambil==0) {angka+=data_str[i];}
      else if(status_ambil==1) {perintah+=data_str[i];}
    }
    status_ambil=0;
    pin=angka.toInt();
    kondisi=perintah;
    angka = "";
    perintah = "";
    status_timer=1;
}
BLYNK_WRITE(V2) 
{
  tombol0 = param.asInt();
  if(tombol0==1){pin=0; kondisi="on";}
  else{pin=0; kondisi="off";}
  status_timer=1;
}
BLYNK_WRITE(V3) 
{
  tombol1 = param.asInt();
  if(tombol1==1){pin=1; kondisi="on";}
  else{pin=1; kondisi="off";}
  status_timer=1;
}

void setup() {
  pinMode(relay0, OUTPUT);
  pinMode(relay1, OUTPUT);
  digitalWrite(relay0, HIGH);
  digitalWrite(relay1, HIGH);
  Serial.begin(115200);
  delay(10);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
//  Blynk.begin(auth, ssid, pass);
  Blynk.begin(auth, ssid, pass, "blynk-cloud.com",8080);
  timer.setInterval(100L,timer_data); 
}

void loop(){
    Blynk.run();
    timer.run();
}

void timer_data()
{
  Serial.print(pin);
  Serial.print(kondisi);
  if(status_timer==1)
  {
    if(pin==0 && kondisi=="on")
    {
      digitalWrite(relay0, LOW);Blynk.virtualWrite(V2, 1);
    }
    else if(pin==0 && kondisi=="off")
    {
      digitalWrite(relay0, HIGH);Blynk.virtualWrite(V2, 0);
    }
    else if(pin==1 && kondisi=="on")
    {
      digitalWrite(relay1, LOW);Blynk.virtualWrite(V3, 1);
    }
    else if(pin==1 && kondisi=="off")
    {
      digitalWrite(relay1, HIGH);Blynk.virtualWrite(V3, 0);
    }
  }
  status_timer=0;
}
