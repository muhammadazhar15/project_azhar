#include <SimpleTimer.h>

SimpleTimer timer;
int relay0 = 2;
int relay1 = 3;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
int pin, tombol0, tombol1;
String kondisi;
String data_str;
String angka, perintah;
byte status_ambil;
byte status_timer;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  pinMode(relay0, OUTPUT);
  pinMode(relay1, OUTPUT);
  digitalWrite(relay0, HIGH);
  digitalWrite(relay1, HIGH);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  timer.setInterval(100L,timer_data); 
}

void loop() {
  timer.run();
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    data_str = inputString;
    for(int i=0; i<data_str.length(); i++)
    { 
      if(String(data_str[i])=="+") {status_ambil=1;}
      else if(status_ambil==0) {angka+=data_str[i];}
      else if(status_ambil==1) {perintah+=data_str[i];}
    }
    status_ambil=0;
    pin=angka.toInt();
    kondisi=perintah;
    angka = "";
    perintah = "";
    status_timer=1;
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == 0x0D) {
      stringComplete = true;
    }
    else{
      // add it to the inputString:
      inputString += inChar;
    }
  }
  while (Serial1.available()){
    tombol0 = Serial1.read() - 48;
    Serial1.println(tombol0);
    if(tombol0==1){pin=0; kondisi="on";}
    else{pin=0; kondisi="off";}
    status_timer=1;
  }
  while (Serial2.available()){
    tombol1 = Serial2.read() - 48;
    Serial2.println(tombol1);
    if(tombol1==1){pin=1; kondisi="on";}
    else{pin=1; kondisi="off";}
    status_timer=1;
  }
}

void timer_data()
{
  if(status_timer==1)
  { //Serial.print("ok");
    //Serial.print(pin);
    //Serial.print(kondisi);
    if(pin==0 && kondisi=="on")
    {
      digitalWrite(relay0, LOW);
    }
    else if(pin==0 && kondisi=="off")
    {
      digitalWrite(relay0, HIGH);
    }
    else if(pin==1 && kondisi=="on")
    {
      digitalWrite(relay1, LOW);
    }
    else if(pin==1 && kondisi=="off")
    {
      digitalWrite(relay1, HIGH);
    }
  }
  status_timer=0;
}
