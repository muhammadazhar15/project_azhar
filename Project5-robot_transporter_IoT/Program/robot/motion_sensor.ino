void take_box() {
  ledcWrite(4, 8);
  ledcWrite(5, 5);
  delay(1000);
  ledcWrite(4, 5);
  ledcWrite(5, 14);
  delay(1000);
  for(byte i=5;i<=17;i++) {
    ledcWrite(4, i);
    delay(100);
  }
  ledcWrite(4, 17);
  ledcWrite(5, 5);
}

void drop_box() {
  ledcWrite(4, 17);
  ledcWrite(5, 14);
  delay(1000);
  ledcWrite(4, 5);
  ledcWrite(5, 14);
  delay(1000);
  ledcWrite(4, 17);
  ledcWrite(5, 5);
  delay(1000);
}

void read_barcode() {
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

void manual_maju(byte v0, byte v1) {
  ledcWrite(0, v0);
  ledcWrite(1, 0);
  ledcWrite(2, v1);
  ledcWrite(3, 0);
}

void manual_mundur(byte v0, byte v1) {
  ledcWrite(0, 0);
  ledcWrite(1, v0);
  ledcWrite(2, 0);
  ledcWrite(3, v1);
}

void manual_kanan(byte v0, byte v1) {
  ledcWrite(0, 0);
  ledcWrite(1, v0);
  ledcWrite(2, v1);
  ledcWrite(3, 0);
}

void manual_kiri(byte v0, byte v1) {
  ledcWrite(0, v0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, v1);
}

void manual_diam() {
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}
