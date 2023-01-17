#include "MPU9250.h"
#include "EEPROM.h"

/* accelerometer and magnetometer data */
float a, ax, ay, az, h, hx, hy, hz;
/* magnetometer calibration data */
float hxb, hxs, hyb, hys, hzb, hzs;
/* euler angles */
float pitch_rad, roll_rad, yaw_rad, heading_rad;
/* filtered heading */
float filtered_heading_rad;
float window_size = 20;
/* conversion radians to degrees */
const float R2D = 180.0f / PI;
/* MPU 9250 object */
MPU9250 imu(Wire, 0x68);
/* MPU 9250 data ready pin */
const uint8_t kMpu9250DrdyPin = 2;
/* Flag set to indicate MPU 9250 data is ready */
volatile bool imu_data_ready = false;
/////////////tambahan////////////
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
int c = 0;
float elapsedTime, currentTime, previousTime;
float yaw;
byte status_gyro;
char dataYawKirim[8];
float head_compas, head_gyro;

/* ISR to set data ready flag */
void data_ready(){
  imu_data_ready = true;
}

void setup()
{
  for(int i = 6; i<=9; i++){
     pinMode(i, OUTPUT);digitalWrite(i, HIGH);delay(250);digitalWrite(i, LOW);
  }
  for(int i = 9; i>=6; i--){
     pinMode(i, OUTPUT);digitalWrite(i, LOW);delay(250);digitalWrite(i, HIGH);
  }
  /* Serial for displaying results */
  Serial.begin(115200);
  while(!Serial) {}
  /* 
  * Start the sensor, set the bandwidth the 10 Hz, the output data rate to
  * 50 Hz, and enable the data ready interrupt. 
  */
  imu.begin();
  imu.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_10HZ);
  imu.setSrd(19);
  imu.enableDataReadyInterrupt();
  /*
  * Load the magnetometer calibration
  */
  uint8_t eeprom_buffer[24];
  for (unsigned int i = 0; i < sizeof(eeprom_buffer); i++ ) {
    eeprom_buffer[i] = EEPROM.read(i);
  }
  memcpy(&hxb, eeprom_buffer, sizeof(hxb));
  memcpy(&hyb, eeprom_buffer + 4, sizeof(hyb));
  memcpy(&hzb, eeprom_buffer + 8, sizeof(hzb));
  memcpy(&hxs, eeprom_buffer + 12, sizeof(hxs));
  memcpy(&hys, eeprom_buffer + 16, sizeof(hys));
  memcpy(&hzs, eeprom_buffer + 20, sizeof(hzs));
  imu.setMagCalX(hxb, hxs);
  imu.setMagCalY(hyb, hys);
  imu.setMagCalZ(hzb, hzs);
  /* Attach the data ready interrupt to the data ready ISR */
  pinMode(kMpu9250DrdyPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(kMpu9250DrdyPin), data_ready, RISING);
  /////tambahan////////
  calculate_IMU_error();
  delay(20);
}

void loop()
{
  if (imu_data_ready) {
    imu_data_ready = false;
    /* Read the MPU 9250 data */
    imu.readSensor();
    ax = imu.getAccelX_mss();
    ay = imu.getAccelY_mss();
    az = imu.getAccelZ_mss();
    hx = imu.getMagX_uT();
    hy = imu.getMagY_uT();
    hz = imu.getMagZ_uT();

    previousTime = currentTime;        // Previous time is stored before the actual time read
    currentTime = millis();            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
    GyroX = imu.getGyroX_rads();
    GyroY = imu.getGyroY_rads();
    GyroZ = imu.getGyroZ_rads();
    // Correct the outputs with the calculated error values
    GyroX = GyroX - GyroErrorX; // GyroErrorX ~(-0.56)
    GyroY = GyroY - GyroErrorY; // GyroErrorY ~(2)
    GyroZ = GyroZ - GyroErrorZ; // GyroErrorZ ~ (-0.8)
    if(abs(GyroZ) < 0.0005){
      GyroZ = 0.0;
    }
    
    if(status_gyro == 1){
      yaw =  yaw + GyroZ * elapsedTime;
      if(yaw >  2*PI) yaw = yaw - 2*PI;
      else if(yaw < 0) yaw = 2*PI - yaw;
    }
    else{
      status_gyro = 1;
    }
  
    /* Normalize accelerometer and magnetometer data */
    a = sqrtf(ax * ax + ay * ay + az * az);
    ax /= a;
    ay /= a;
    az /= a;
    h = sqrtf(hx * hx + hy * hy + hz * hz);
    hx /= h;
    hy /= h;
    hz /= h;
    /* Compute euler angles */
    pitch_rad = asinf(ax);
    roll_rad = asinf(-ay / cosf(pitch_rad));
    yaw_rad = atan2f(hz * sinf(roll_rad) - hy * cosf(roll_rad), hx * cosf(pitch_rad) + hy * sinf(pitch_rad) * sinf(roll_rad) + hz * sinf(pitch_rad) * cosf(roll_rad));
    heading_rad = constrainAngle360(yaw_rad);
    /* Filtering heading */
    filtered_heading_rad = (filtered_heading_rad * (window_size - 1.0f) + heading_rad) / window_size;
    /* Display the results */
//    Serial.print(pitch_rad * R2D);
//    Serial.print("\t");
//    Serial.print(roll_rad * R2D);
//    Serial.print("\t");
//    Serial.print(yaw_rad * R2D);
//    Serial.print("\t");
//    Serial.print(heading_rad * R2D);
//    Serial.print("\t");
//    Serial.println(filtered_heading_rad * R2D);

    head_gyro = yaw * R2D;
    if(head_gyro>180) head_gyro = head_gyro - 360;
    head_compas = heading_rad * R2D;
    if(head_compas>180) head_compas = head_compas - 360;
//    Serial.print(head_gyro);
//    Serial.print("\t");
//    Serial.println(head_compas);
    dataYawKirim[0] = (*(long int*)&head_gyro)&0xFF;
    dataYawKirim[1] = ((*(long int*)&head_gyro)>>8)&0xFF;
    dataYawKirim[2] = ((*(long int*)&head_gyro)>>16)&0xFF;
    dataYawKirim[3] = ((*(long int*)&head_gyro)>>24)&0xFF;
    dataYawKirim[4] = ((*(long int*)&head_compas)>>0)&0xFF;
    dataYawKirim[5] = ((*(long int*)&head_compas)>>8)&0xFF;
    dataYawKirim[6] = ((*(long int*)&head_compas)>>16)&0xFF;
    dataYawKirim[7] = ((*(long int*)&head_compas)>>24)&0xFF;
    Serial.write('i');
    Serial.write('t');
    Serial.write('s');
    Serial.write(dataYawKirim[0]);
    Serial.write(dataYawKirim[1]);
    Serial.write(dataYawKirim[2]);
    Serial.write(dataYawKirim[3]);
    Serial.write(dataYawKirim[4]);
    Serial.write(dataYawKirim[5]);
    Serial.write(dataYawKirim[6]);
    Serial.write(dataYawKirim[7]);
  }
}

void calculate_IMU_error() {
  while (c < 150) {
    imu.readSensor();
    AccX = imu.getAccelX_mss();
    AccY = imu.getAccelY_mss();
    AccZ = imu.getAccelZ_mss();
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 150;
  AccErrorY = AccErrorY / 150;
  c = 0;
  // Read gyro values 200 times
  while (c < 150) {
    imu.readSensor();
    GyroX = imu.getGyroX_rads();
    GyroY = imu.getGyroY_rads();
    GyroZ = imu.getGyroZ_rads();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX);
    GyroErrorY = GyroErrorY + (GyroY);
    GyroErrorZ = GyroErrorZ + (GyroZ);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 150;
  GyroErrorY = GyroErrorY / 150;
  GyroErrorZ = GyroErrorZ / 150;
}
/* Bound angle between 0 and 360 */
float constrainAngle360(float dta) {
  dta = fmod(dta, 2.0 * PI);
  if (dta < 0.0)
    dta += 2.0 * PI;
  return dta;
}






/////////////////////////program Kalibrasi magneto//////////////////////////
//#include "MPU9250.h"
//#include "EEPROM.h"
//
///* MPU 9250 object */
//MPU9250 imu(Wire, 0x68);
//
///* EEPROM buffer to mag bias and scale factors */
//uint8_t eeprom_buffer[24];
//float value;
//
//void setup() {
//  /* Serial for displaying instructions */
//  Serial.begin(115200);
//  while(!Serial) {}
//  /* Start communication with IMU */
//  imu.begin();
//  /* Calibrate magnetometer */
//  Serial.print("Calibrating magnetometer, please slowly move in a figure 8 until complete...");
//  imu.calibrateMag();
//  Serial.println("Done!");
//  Serial.print("Saving results to EEPROM...");
//  /* Save to EEPROM */
//  value = imu.getMagBiasX_uT();
//  memcpy(eeprom_buffer, &value, sizeof(value));
//  value = imu.getMagBiasY_uT();
//  memcpy(eeprom_buffer + 4, &value, sizeof(value));
//  value = imu.getMagBiasZ_uT();
//  memcpy(eeprom_buffer + 8, &value, sizeof(value));
//  value = imu.getMagScaleFactorX();
//  memcpy(eeprom_buffer + 12, &value, sizeof(value));
//  value = imu.getMagScaleFactorY();
//  memcpy(eeprom_buffer + 16, &value, sizeof(value));
//  value = imu.getMagScaleFactorZ();
//  memcpy(eeprom_buffer + 20, &value, sizeof(value));
//  for (unsigned int i = 0; i < sizeof(eeprom_buffer); i++) {
//    EEPROM.write(i, eeprom_buffer[i]);
//  }
//  Serial.println("Done! You may power off your board.");
//}
//
//void loop() {}
