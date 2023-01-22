String kode;
byte tanda;
int balas;

void setup() {
  Serial.begin(9600);
  
  Serial2.begin(115200);
  Serial2.setTimeout(100);

  Serial2.write(0x7E);
  Serial2.write(0x00);
  Serial2.write(0x08);
  Serial2.write(0x01);
  Serial2.write(0x00);
  Serial2.write(0x02);
  Serial2.write(0x01);
  Serial2.write(0xAB);
  Serial2.write(0xCD);
}

void loop() {
  while (Serial2.available() > 0) {
    if(tanda<7){
      balas = Serial2.read();
      Serial.println(balas,HEX);
      tanda++;
    }
    else
    {
      kode = Serial2.readString();
      Serial.println(kode);
    }
  }
}
