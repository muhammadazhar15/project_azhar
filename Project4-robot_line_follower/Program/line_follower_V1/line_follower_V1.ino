#include <TimerOne.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

LiquidCrystal lcd(13, 12, 10, 9, 8, 7);//RS,E,d4,d5,d6,d7

char lcd_buffer[16];
byte status_menu,tombol_menu=1;
byte status_sub_menu,prev_status_sub_menu,tombol_sub_menu=1;
byte status_tambah,status_kurang,tombol_tambah=1,tombol_kurang=1;
unsigned int timer_tambah,timer_kurang;
byte status_layar;
///////SENSOR////////////////
int sensor[14];
int status_kalibrasi,timer_status_kalibrasi;
byte status_garis,status_lebar_garis;
unsigned int nilai_sensor;
byte threshold_8bit[14];
int threshold[14];
int min_sensor[14]={1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,
                    1023,1023};
int max_sensor[14]={0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte pewaktu_sensor=0;
//////////////Motion/////////////////////
int posisi,posisi_prev;
float output_motor[2];
byte pwm_default=50,v_max_default=255;
byte pwm_awal,v_max;
byte status_pid = 0;
byte kp[6],ki[6],kd[6];
int setpoint_default=0,setpoint;
float a=0.5;
byte status_percepatan[2];
byte status_menu_pid,index_pid,pid_default,nilai_pid;
int timer_status_menu_pid;
/////////////////indexing////////////////
byte status_menu_index;
byte index,total_index,index_tujuan;
byte index_mode[100],index_sensor[100],index_sensor_mode[100];
byte index_motor0[100],index_motor1[100];
byte index_delay[100],index_timer[100],index_brake[100];
byte index_pwm[100],index_vmax[100],index_nilai_pid[100];
int timer_status_index;
byte timer_brake,timer_delay,timer_timer;
byte pewaktu=0;
byte status_mulai;
////////////////////CP/////////////////////////////
byte status_menu_start,status_lintasan;
byte index_jalan,delay_start,delay_default;

void setup() {
//  Serial.begin(9600);
  for(byte k=0;k<14;k++)
  {
    threshold_8bit[k]=EEPROM.read(k);
    threshold[k]=map(threshold_8bit[k],0,255,0,1023);
  }
//  kp=EEPROM.read(14);ki=EEPROM.read(15);kd=EEPROM.read(16);
  nilai_pid = EEPROM.read(14);
  index_pid = pid_default = nilai_pid;
  setpoint = EEPROM.read(17);
  setpoint_default = setpoint;
  v_max = EEPROM.read(18);
  v_max_default = v_max;
  pwm_awal = EEPROM.read(19);
  pwm_default = pwm_awal;
  total_index = EEPROM.read(20);
  status_lintasan = EEPROM.read(21);
  status_garis = EEPROM.read(22);
  status_lebar_garis = EEPROM.read(23);
  for(byte j=0;j<=total_index;j++)
  {
    index_mode[j] = EEPROM.read(24+j);
    index_sensor[j] = EEPROM.read(124+j);
    index_sensor_mode[j] = index_sensor[j]&0b00000111;
    index_sensor[j] = index_sensor[j]>>3;
    index_brake[j] = EEPROM.read(224+j);
    index_delay[j] = EEPROM.read(324+j);
    index_motor0[j] = EEPROM.read(424+j);
    index_motor1[j] = EEPROM.read(524+j);
    index_timer[j] = EEPROM.read(624+j);
    index_pwm[j] = EEPROM.read(724+j);
    index_vmax[j] = EEPROM.read(824+j);
    if(j%2==0)
    {
      if(j==total_index&&total_index%2==0)
      {
        index_nilai_pid[j] = EEPROM.read(924+(j/2));
        index_nilai_pid[j] = index_nilai_pid[j]>>4;
      }
      else
      {
        index_nilai_pid[j] = EEPROM.read(924+(j/2));
        index_nilai_pid[j+1] = index_nilai_pid[j]&0b00001111;
        index_nilai_pid[j] = index_nilai_pid[j]>>4;
      }
    }
  }
  for(byte j=0;j<=5;j++)
  {
    kp[j]=EEPROM.read(1000+j);
    ki[j]=EEPROM.read(1006+j);
    kd[j]=EEPROM.read(1011+j);
  }
  
  /////////LCD////////
  lcd.begin(16, 2);
  character_init();
  ///////I/O/////////
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  pinMode(0,INPUT_PULLUP);
  pinMode(1,INPUT_PULLUP);
  pinMode(2,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
    ///////Interrupt Timer/////////
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timer_int);
}

void loop() {
  if(status_menu==0)
  {if(status_layar==0){lcd.clear(); status_layar=1;}
   if(status_menu_start==0)
   {
    sprintf(lcd_buffer,"i%03d",index_jalan);
    lcd.setCursor(0,0);
    lcd.print(lcd_buffer);
    lcd.setCursor(5,0);
    lcd.print(F("MAIN"));
    lcd.setCursor(10,0);
    switch(status_lintasan)
    {case 0:lcd.print(F("R:SPT"));break;
     case 1:lcd.print(F("R:CRT"));break;}
    sprintf(lcd_buffer,"d%03d",delay_start);
    lcd.setCursor(0,1);
    lcd.print(lcd_buffer);
    lcd.setCursor(5,1);
    switch(status_garis)
    {case 0:lcd.print(F("L:BL"));break;
     case 1:lcd.print(F("L:WH"));break;}
    lcd.setCursor(10,1);
    switch(status_lebar_garis)
    {case 0:lcd.print(F("W:2cm"));break;
     case 1:lcd.print(F("W:3cm"));break;
     case 2:lcd.print(F("W:4cm"));break;}
    if(status_sub_menu==0)
    { lcd.setCursor(4,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
      lcd.setCursor(4,1);lcd.print(" ");lcd.setCursor(9,1);lcd.print(" ");
      lcd.setCursor(15,1);lcd.print(" ");
      if(status_tambah==1)
      {status_menu_start=1;status_tambah=0;lcd.clear();}
      else if(status_kurang==1)
      {status_menu_start=1;status_kurang=0;lcd.clear();}
    }
    else if(status_sub_menu==1)
    { lcd.setCursor(4,0);lcd.write(127);lcd.setCursor(15,0);lcd.print(" ");
      lcd.setCursor(4,1);lcd.print(" ");lcd.setCursor(9,1);lcd.print(" ");
      lcd.setCursor(15,1);lcd.print(" ");
      if(status_tambah==1)
      {index_jalan++;if(index_jalan>total_index)index_jalan=0;status_tambah=0;}
      else if(status_kurang==1)
      {index_jalan--;if(index_jalan>total_index)index_jalan=total_index;status_kurang=0;}
    }
    else if(status_sub_menu==2)
    { lcd.setCursor(4,0);lcd.print(" ");lcd.setCursor(15,0);lcd.write(127);
      lcd.setCursor(4,1);lcd.print(" ");lcd.setCursor(9,1);lcd.print(" ");
      lcd.setCursor(15,1);lcd.print(" ");
      if(status_tambah==1)
      {status_lintasan++;if(status_lintasan>1)status_lintasan=0;;EEPROM.write(21,status_lintasan);status_tambah=0;}
      else if(status_kurang==1)
      {status_lintasan--;if(status_lintasan>1)status_lintasan=1;EEPROM.write(21,status_lintasan);status_kurang=0;}
    }
    else if(status_sub_menu==3)
    { lcd.setCursor(4,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
      lcd.setCursor(4,1);lcd.write(127);lcd.setCursor(9,1);lcd.print(" ");
      lcd.setCursor(15,1);lcd.print(" ");
      if(status_tambah==1)
      {delay_start++;status_tambah=0;}
      else if(status_kurang==1)
      {delay_start--;status_kurang=0;}
    }
    else if(status_sub_menu==4)
    { lcd.setCursor(4,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
      lcd.setCursor(4,1);lcd.print(" ");lcd.setCursor(9,1);lcd.write(127);
      lcd.setCursor(15,1);lcd.print(" ");
      if(status_tambah==1)
      {status_garis++;if(status_garis>1)status_garis=0;EEPROM.write(22,status_garis);status_tambah=0;}
      else if(status_kurang==1)
      {status_garis--;if(status_garis>1)status_garis=1;EEPROM.write(22,status_garis);status_kurang=0;} 
    }
    else if(status_sub_menu==5)
    { lcd.setCursor(4,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
      lcd.setCursor(4,1);lcd.print(" ");lcd.setCursor(9,1);lcd.print(" ");
      lcd.setCursor(15,1);lcd.write(127);
      if(status_tambah==1)
      {status_lebar_garis++;if(status_lebar_garis>2)status_lebar_garis=0;EEPROM.write(23,status_lebar_garis);status_tambah=0;}
      else if(status_kurang==1)
      {status_lebar_garis--;if(status_lebar_garis>2)status_lebar_garis=2;EEPROM.write(23,status_lebar_garis);status_kurang=0;}      
    }
    else status_sub_menu=0;
   }
   else if(status_menu_start==1)
   {
    if(status_sub_menu==0)
    {lcd.setCursor(0,0);lcd.print(F(".....~READY....."));
     lcd.setCursor(0,1);lcd.print(F("PRESS <D1> START"));
     if(status_tambah==1)
     {status_menu_start=0;status_tambah=0;lcd.clear();}
     else if(status_kurang==1)
     {status_menu_start=0;status_kurang=0;lcd.clear();}
    }
    else if(status_sub_menu==1)
    {lcd.clear();lcd.setCursor(0,0);lcd.print(F("....~PLAYING...."));status_pid=1;
     status_sub_menu++;delay_default=delay_start;status_mulai=1;}
    else if(status_sub_menu==2)
    { 
      ////////////////RUN MAIN PROGRAM////////////////////////
    }
    else if(status_sub_menu==3)
    {
      status_mulai=0;status_pid=0;status_sub_menu++;lcd.clear();
    }
    else status_sub_menu=0;
   }
  }
  else if(status_menu==1)
  { if(status_layar==0){lcd.clear(); status_layar=1;}
    if(status_kalibrasi==0)
    { lcd.setCursor(0, 0);lcd.print(F("D")); 
      lcd.setCursor(15, 1);lcd.print(F("A")); 
      for(byte j=0;j<14;j++)
      {
        lcd.setCursor(j+1, 0);
        if(bitRead(nilai_sensor,j)==1)lcd.write(byte(7));
        else lcd.write(byte(0));
      }
      int sensor_display[14];
      for(byte j=0; j<14 ;j++)
      {
        sensor_display[j]=map(sensor[j],0,1023,0,7);
        lcd.setCursor(j+1, 1);
        lcd.write(byte(sensor_display[j]));
      }
      if(status_sub_menu==0)
      { 
        if(status_tambah==1)
        {status_kalibrasi=1;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_kalibrasi=1;status_kurang=0;lcd.clear();}
      }
      else status_sub_menu=0;
    }
    else if(status_kalibrasi==1)
    {
      lcd.setCursor(0,0);lcd.print(F(">"));lcd.setCursor(15,0);lcd.print(F("<"));
      lcd.setCursor(0,1);lcd.print(F("M"));lcd.setCursor(15,1);lcd.print(F("T"));
      for(byte j=0;j<14;j++)
      {
        lcd.setCursor(j+1, 0);
        if(bitRead(nilai_sensor,j)==1)lcd.write(byte(7));
        else lcd.write(byte(0));
      }
      if(status_sub_menu!=prev_status_sub_menu){for(byte j=1;j<15;j++){lcd.setCursor(j,1);lcd.print(" ");}prev_status_sub_menu=status_sub_menu;}
      if(status_sub_menu==0)
      { 
        if(status_tambah==1)
        {status_kalibrasi=0;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_kalibrasi=0;status_kurang=0;lcd.clear();}
      }
      else if(status_sub_menu==1||status_sub_menu==2||status_sub_menu==3||status_sub_menu==4||status_sub_menu==5||status_sub_menu==6||status_sub_menu==7)
      { lcd.setCursor(status_sub_menu,1);lcd.print("^");
        sprintf(lcd_buffer,"T%04d",threshold[status_sub_menu-1]);
        lcd.setCursor(9,1);lcd.print(lcd_buffer);
        if(status_tambah==1)
        {threshold[status_sub_menu-1]++;if(threshold[status_sub_menu-1]>1023)threshold[status_sub_menu-1]=0;
         threshold_8bit[status_sub_menu-1]=map(threshold[status_sub_menu-1],0,1023,0,255);
         EEPROM.write(status_sub_menu-1,threshold_8bit[status_sub_menu-1]);status_tambah=0;}
        else if(status_kurang==1)
        {threshold[status_sub_menu-1]--;if(threshold[status_sub_menu-1]<0)threshold[status_sub_menu-1]=1023;
         threshold_8bit[status_sub_menu-1]=map(threshold[status_sub_menu-1],0,1023,0,255);
         EEPROM.write(status_sub_menu-1,threshold_8bit[status_sub_menu-1]);status_kurang=0;}
      }
      else if(status_sub_menu==8||status_sub_menu==9||status_sub_menu==10||status_sub_menu==11||status_sub_menu==12||status_sub_menu==13||status_sub_menu==14)
      { lcd.setCursor(status_sub_menu,1);lcd.print("^");
        sprintf(lcd_buffer,"T%04d",threshold[status_sub_menu-1]);
        lcd.setCursor(2,1);lcd.print(lcd_buffer);
        if(status_tambah==1)
        {threshold[status_sub_menu-1]++;if(threshold[status_sub_menu-1]>1023)threshold[status_sub_menu-1]=0;
         threshold_8bit[status_sub_menu-1]=map(threshold[status_sub_menu-1],0,1023,0,255);
         EEPROM.write(status_sub_menu-1,threshold_8bit[status_sub_menu-1]);status_tambah=0;}
        else if(status_kurang==1)
        {threshold[status_sub_menu-1]--;if(threshold[status_sub_menu-1]<0)threshold[status_sub_menu-1]=1023;
         threshold_8bit[status_sub_menu-1]=map(threshold[status_sub_menu-1],0,1023,0,255);
         EEPROM.write(status_sub_menu-1,threshold_8bit[status_sub_menu-1]);status_kurang=0;}
      }
      else status_sub_menu=0;
    }
    else if(status_kalibrasi==2)
    {
      lcd.setCursor(0, 0);
      lcd.print(F("   ~KALIBRASI   "));
      lcd.setCursor(0, 1);lcd.print(" ");
      for(byte j=0; j<14 ;j++)
      {
        sensor[j]=map(sensor[j],0,1023,0,7);
        lcd.setCursor(j+1, 1);
        lcd.write(byte(sensor[j]));
      }
    }
    else if(status_kalibrasi==3)
    {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print(F("SAVING...."));
      for(byte k=0;k<14;k++)
      {
        threshold[k]=min_sensor[k]+((max_sensor[k]-min_sensor[k])*2/5);
        threshold_8bit[k]=map(threshold[k],0,1023,0,255);
        EEPROM.write(k,threshold_8bit[k]);
        min_sensor[k]=1023;max_sensor[k]=0;
      }
      delay(2000);
      lcd.clear();
      status_kalibrasi=0;
    }
  }
  else if(status_menu==2)
  { if(status_layar==0){lcd.clear(); status_layar=1;}
    sprintf(lcd_buffer,"S%+03d",setpoint);
    lcd.setCursor(0,0);
    lcd.print(lcd_buffer);
    sprintf(lcd_buffer,"Vm%03d",v_max);
    lcd.setCursor(5,0);
    lcd.print(lcd_buffer);
    sprintf(lcd_buffer,"V%03d",pwm_awal);
    lcd.setCursor(11,0);
    lcd.print(lcd_buffer);
    sprintf(lcd_buffer,"P%+03d",posisi);
    lcd.setCursor(0, 1);
    lcd.print(lcd_buffer);
    lcd.setCursor(5, 1);
    lcd.print(F("~MOTION SET")); 
    if(status_sub_menu==0)
    { lcd.setCursor(4,0);lcd.write(127);
      lcd.setCursor(10,0);lcd.print(" ");
      lcd.setCursor(15,0);lcd.print(" ");
      if(status_tambah==1)
      {
        setpoint++;if(setpoint>10)setpoint=-10;EEPROM.write(17,setpoint);status_tambah=0;
      }
      else if(status_kurang==1)
      {
        setpoint--;if(setpoint<-10)setpoint=10;EEPROM.write(17,setpoint);status_kurang=0;
      }
    }
    else if(status_sub_menu==1)
    { lcd.setCursor(4,0);lcd.print(" ");
      lcd.setCursor(10,0);lcd.write(127);
      lcd.setCursor(15,0);lcd.print(" ");
      if(status_tambah==1)
      {
        v_max++;v_max_default=v_max;EEPROM.write(18,v_max);status_tambah=0;
      }
      else if(status_kurang==1)
      {
        v_max--;v_max_default=v_max;EEPROM.write(18,v_max);status_kurang=0;
      }
    }
    else if(status_sub_menu==2)
    { lcd.setCursor(4,0);lcd.print(" ");
      lcd.setCursor(10,0);lcd.print(" ");
      lcd.setCursor(15,0);lcd.write(127);
      if(status_tambah==1)
      {
        pwm_awal++;pwm_default=pwm_awal;EEPROM.write(19,pwm_awal);status_tambah=0;
      }
      else if(status_kurang==1)
      {
        pwm_awal--;pwm_default=pwm_awal;EEPROM.write(19,pwm_awal);status_kurang=0;
      }
    }  
    else status_sub_menu=0;
  }
  else if(status_menu==3)
  { 
    if(status_menu_index==0)
    {
      if(status_layar==0){lcd.clear(); status_layar=1;}
      sprintf(lcd_buffer,"i%02d",index);
      lcd.setCursor(0, 0);
      lcd.print(lcd_buffer);
      lcd.setCursor(4, 0);
      switch(index_mode[index])
      {
        case 0:lcd.print(F("RGT"));break;
        case 1:lcd.print(F("LFT"));break;
        case 2:lcd.print(F("FWD"));break;
        case 3:lcd.print(F("RVS"));break;
        case 4:lcd.print(F("BLK"));break;
        case 5:lcd.print(F("WHT"));break;
      }
      lcd.setCursor(8, 0);
      int mode_display;
      switch(index_sensor[index])
      {
        case 0:lcd.print(F("S10"));mode_display=0b1100000000000001;break;
        case 1:lcd.print(F("S01"));mode_display=0b1110000000000000;break;
        case 2:lcd.print(F("D23"));mode_display=0b1100000011100011;break;
        case 3:lcd.print(F("D32"));mode_display=0b1111000111000000;break;
        case 4:lcd.print(F("M24"));mode_display=0b1100000111100110;break;
        case 5:lcd.print(F("M42"));mode_display=0b1101100111100000;break;
        case 6:lcd.print(F("M33"));mode_display=0b1100001111110000;break;
        case 7:lcd.print(F("M11"));mode_display=0b1100010000001000;break;
        case 8:lcd.print(F("M00"));mode_display=0b1100000000000000;break;
        case 9:lcd.print(F("M10"));mode_display=0b1100000000001000;break;
        case 10:lcd.print(F("M01"));mode_display=0b1100010000000000;break;
      }
      for(byte j=0;j<14;j++)
      {lcd.setCursor(j, 1);if(bitRead(mode_display,j)==1)lcd.write(byte(7));else lcd.write(byte(0));}
      lcd.setCursor(12, 0);
      switch(index_sensor_mode[index])
      {
        case 0:lcd.print(F("[=]"));lcd.setCursor(15,1);lcd.print(F("="));break;
        case 1:lcd.print(F(" OR"));lcd.setCursor(15,1);lcd.print(F("R"));break;
        case 2:lcd.print(F("XOR"));lcd.setCursor(15,1);lcd.print(F("X"));break;
        case 3:lcd.print(F("ALL"));lcd.setCursor(15,1);lcd.print(F("A"));break;
      }

      if(status_sub_menu==0)
      { lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(7,0);lcd.print(" ");
        lcd.setCursor(11,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_index=1;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_menu_index=2;status_kurang=0;lcd.clear();}
      }
      else if(status_sub_menu==1)
      { lcd.setCursor(3,0);lcd.write(127);lcd.setCursor(7,0);lcd.print(" ");
        lcd.setCursor(11,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        if(status_tambah==1)
        {index++;if(index>99)index=0;status_tambah=0;}
        else if(status_kurang==1)
        {index--;if(index>99)index=99;status_kurang=0;}
      }
      else if(status_sub_menu==2)
      { lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(7,0);lcd.write(127);
        lcd.setCursor(11,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        if(status_tambah==1)
        {index_mode[index]++;if(index_mode[index]>5)index_mode[index]=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_mode[index]--;if(index_mode[index]>5)index_mode[index]=6;status_kurang=0;}
      }
      else if(status_sub_menu==3)
      { lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(7,0);lcd.print(" ");
        lcd.setCursor(11,0);lcd.write(127);lcd.setCursor(15,0);lcd.print(" ");
        if(status_tambah==1)
        {index_sensor[index]++;if(index_sensor[index]>10)index_sensor[index]=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_sensor[index]--;if(index_sensor[index]>10)index_sensor[index]=10;status_kurang=0;}
      }
      else if(status_sub_menu==4)
      { lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(7,0);lcd.print(" ");
        lcd.setCursor(11,0);lcd.print(" ");lcd.setCursor(15,0);lcd.write(127);
        if(status_tambah==1)
        {index_sensor_mode[index]++;if(index_sensor_mode[index]>3)index_sensor_mode[index]=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_sensor_mode[index]--;if(index_sensor_mode[index]>3)index_sensor_mode[index]=3;status_kurang=0;}
      }
      else status_sub_menu=0;      
    }
    else if(status_menu_index==1)
    {
      sprintf(lcd_buffer,"i%02d",index);
      lcd.setCursor(0, 0);
      lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"BR%03d",index_brake[index]);
      lcd.setCursor(4, 0);
      lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"DL%03d",index_delay[index]);
      lcd.setCursor(10, 0);
      lcd.print(lcd_buffer);
      lcd.setCursor(0, 1);
      switch(index_mode[index])
      {
        case 0:lcd.print(F("RGT"));
               sprintf(lcd_buffer,"R-%03d",index_motor0[index]);
               lcd.setCursor(4,1);lcd.print(lcd_buffer);
               sprintf(lcd_buffer,"L+%03d",index_motor1[index]);
               lcd.setCursor(10,1);lcd.print(lcd_buffer);break;
        case 1:lcd.print(F("LFT"));
               sprintf(lcd_buffer,"R+%03d",index_motor0[index]);
               lcd.setCursor(4,1);lcd.print(lcd_buffer);
               sprintf(lcd_buffer,"L-%03d",index_motor1[index]);
               lcd.setCursor(10,1);lcd.print(lcd_buffer);break;
        case 2:lcd.print(F("FWD"));
               sprintf(lcd_buffer,"R+%03d",index_motor0[index]);
               lcd.setCursor(4,1);lcd.print(lcd_buffer);
               sprintf(lcd_buffer,"L+%03d",index_motor1[index]);
               lcd.setCursor(10,1);lcd.print(lcd_buffer);break;
        case 3:lcd.print(F("RVS"));
               sprintf(lcd_buffer,"R-%03d",index_motor0[index]);
               lcd.setCursor(4,1);lcd.print(lcd_buffer);
               sprintf(lcd_buffer,"L-%03d",index_motor1[index]);
               lcd.setCursor(10,1);lcd.print(lcd_buffer);break;
        case 4:lcd.print(F("BLK"));
               if(index_motor0[index]>2)index_motor0[index]=0;
               if(index_motor1[index]>10&&index_motor1[index]<246)index_motor1[index]=0;
               lcd.setCursor(4,1);
               switch(index_motor0[index])
                {case 0:lcd.print(F("W:2cm"));break;
                 case 1:lcd.print(F("W:3cm"));break;
                 case 2:lcd.print(F("W:4cm"));break;}
               sprintf(lcd_buffer,"S:%+03d",(char)index_motor1[index]);
               lcd.setCursor(10,1);lcd.print(lcd_buffer);break;
        case 5:lcd.print(F("WHT"));
               lcd.setCursor(4,1);
               if(index_motor0[index]>2)index_motor0[index]=0;
               if(index_motor1[index]>10&&index_motor1[index]<246)index_motor1[index]=0;
               switch(index_motor0[index])
                {case 0:lcd.print(F("W:2cm"));break;
                 case 1:lcd.print(F("W:3cm"));break;
                 case 2:lcd.print(F("W:4cm"));break;}
               sprintf(lcd_buffer,"S:%+03d",(char)index_motor1[index]);
               lcd.setCursor(10,1);lcd.print(lcd_buffer);break;
      }
      if(status_sub_menu==0)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_index=2;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_menu_index=0;status_kurang=0;lcd.clear();}
      }
      else if(status_sub_menu==1)
      { lcd.setCursor(9,0);lcd.write(127);lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {index_brake[index]++;status_tambah=0;}
        else if(status_kurang==1)
        {index_brake[index]--;status_kurang=0;}
      }
      else if(status_sub_menu==2)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.write(127);
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {index_delay[index]++;if(index_delay[index]>9999)index_delay[index]=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_delay[index]--;if(index_delay[index]<0)index_delay[index]=9999;status_kurang=0;}
      }
      else if(status_sub_menu==3)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.write(127);lcd.setCursor(15,1);lcd.print(" ");
        switch(index_mode[index])
        {
          case 0:
          case 1:
          case 2:
          case 3: if(status_tambah==1)
                  {index_motor0[index]++;status_tambah=0;}
                  else if(status_kurang==1)
                  {index_motor0[index]--;status_kurang=0;}break;
          case 4: 
          case 5: if(status_tambah==1)
                  {index_motor0[index]++;if(index_motor0[index]>2)index_motor0[index]=0;status_tambah=0;}
                  else if(status_kurang==1)
                  {index_motor0[index]--;if(index_motor0[index]>2)index_motor0[index]=2;status_kurang=0;}break;
        }     
      }
      else if(status_sub_menu==4)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.write(127);
        switch(index_mode[index])
        {
          case 0:
          case 1:
          case 2:
          case 3: if(status_tambah==1)
                  {index_motor1[index]++;status_tambah=0;}
                  else if(status_kurang==1)
                  {index_motor1[index]--;status_kurang=0;}break;
          case 4:
          case 5: if(status_tambah==1)
                  {index_motor1[index]++;if(index_motor1[index]>10&&index_motor1[index]<246)index_motor1[index]=246;status_tambah=0;}
                  else if(status_kurang==1)
                  {index_motor1[index]--;if(index_motor1[index]>10&&index_motor1[index]<246)index_motor1[index]=10;status_kurang=0;}break;
        }
      }
      else status_sub_menu=0;
    }
    else if(status_menu_index==2)
    {
      sprintf(lcd_buffer,"i%02d",index);
      lcd.setCursor(0, 0);
      lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"V%03d",index_pwm[index]);
      lcd.setCursor(5, 0);
      lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"Td%03d",index_timer[index]);
      lcd.setCursor(10, 0);
      lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"M%03d",index_vmax[index]);
      lcd.setCursor(5, 1);
      lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"PID:%d",index_nilai_pid[index]);
      lcd.setCursor(10, 1);
      lcd.print(lcd_buffer);
      if(status_sub_menu==0)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_index=0;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_menu_index=1;status_kurang=0;lcd.clear();}
      }
      else if(status_sub_menu==1)
      { lcd.setCursor(9,0);lcd.write(127);lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {index_pwm[index]++;status_tambah=0;}
        else if(status_kurang==1)
        {index_pwm[index]--;status_kurang=0;}
      }
      else if(status_sub_menu==2)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.write(127);
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {index_timer[index]++;if(index_timer[index]>9999)index_timer[index]=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_timer[index]--;if(index_timer[index]<0)index_timer[index]=9999;status_kurang=0;}
      }
      else if(status_sub_menu==3)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.write(127);lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {index_vmax[index]++;status_tambah=0;}
        else if(status_kurang==1)
        {index_vmax[index]--;status_kurang=0;}
      }
      else if(status_sub_menu==4)
      { lcd.setCursor(9,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        lcd.setCursor(9,1);lcd.print(" ");lcd.setCursor(15,1);lcd.write(127);
        if(status_tambah==1)
        {index_nilai_pid[index]++;if(index_nilai_pid[index]>5)index_nilai_pid[index]=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_nilai_pid[index]--;if(index_nilai_pid[index]>5)index_nilai_pid[index]=5;status_kurang=0;}
      }
      else status_sub_menu=0;
    }
    else if(status_menu_index==3)
    { if(status_layar==0){lcd.clear(); status_layar=1;}
      lcd.setCursor(0,0);lcd.print(F("SVE"));lcd.setCursor(4,0);lcd.print(F("BCK"));
      lcd.setCursor(8,0);lcd.print(F("CPY"));lcd.setCursor(12,0);lcd.print(F("MVE"));
      lcd.setCursor(0,1);lcd.print(F("<D4 for ~SELECT>"));
      if(status_sub_menu==0)
      { lcd.setCursor(3,0);lcd.write(127);lcd.setCursor(7,0);lcd.print(" ");
        lcd.setCursor(11,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_index=4;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_kurang=0;}
      }
      else if(status_sub_menu==1)
      { lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(7,0);lcd.write(127);
        lcd.setCursor(11,0);lcd.print(" ");lcd.setCursor(15,0);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_index=0;status_sub_menu=0;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_kurang=0;}
      }
      else if(status_sub_menu==2)
      { lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(7,0);lcd.print(" ");
        lcd.setCursor(11,0);lcd.write(127);lcd.setCursor(15,0);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_index=5;status_sub_menu=0;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_kurang=0;}
      }
      else if(status_sub_menu==3)
      { lcd.setCursor(3,0);lcd.print(" ");lcd.setCursor(7,0);lcd.print(" ");
        lcd.setCursor(11,0);lcd.print(" ");lcd.setCursor(15,0);lcd.write(127);
        if(status_tambah==1)
        {status_menu_index=6;status_sub_menu=0;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_kurang=0;}
      }
      else status_sub_menu=0;
    }
    else if(status_menu_index==4)
    { 
      lcd.setCursor(0,0);lcd.print(F("SAVING........."));
      total_index=index;EEPROM.write(20,total_index);
      lcd.setCursor(0,1);lcd.print(F("Total index:"));
      sprintf(lcd_buffer,"%02d",total_index);
      lcd.setCursor(12,1);lcd.print(lcd_buffer);
      for(byte j=0;j<=total_index;j++)
      {
        EEPROM.write(24+j,index_mode[j]);
        index_sensor[j]=index_sensor[j]<<3;
        index_sensor[j]|=index_sensor_mode[j];
        EEPROM.write(124+j,index_sensor[j]);
        index_sensor[j]=index_sensor[j]>>3;
        EEPROM.write(224+j,index_brake[j]);
        EEPROM.write(324+j,index_delay[j]);
        EEPROM.write(424+j,index_motor0[j]);
        EEPROM.write(524+j,index_motor1[j]);
        EEPROM.write(624+j,index_timer[j]);
        EEPROM.write(724+j,index_pwm[j]);
        EEPROM.write(824+j,index_vmax[j]);
        if(j%2==0)
        {
          index_nilai_pid[j]=index_nilai_pid[j]<<4;
          index_nilai_pid[j]|=index_nilai_pid[j+1];
          EEPROM.write(924+(j/2),index_nilai_pid[j]);
          index_nilai_pid[j]=index_nilai_pid[j]>>4;
        }
        
        
      }
      delay(3000);
      lcd.clear();
      status_menu_index=0;
    }
    else if(status_menu_index==5)
    { 
      if(status_sub_menu==0)
      {
        lcd.setCursor(1,0);lcd.print(F("COPY to :"));
        sprintf(lcd_buffer,"i%02d",index_tujuan);
        lcd.setCursor(11,0);lcd.print(lcd_buffer);lcd.setCursor(14,0);lcd.write(127);
        lcd.setCursor(2,1);lcd.print(F("<D1 for ~OK>"));
        if(status_tambah==1)
        {index_tujuan++;if(index_tujuan>99)index_tujuan=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_tujuan--;if(index_tujuan>99)index_tujuan=99;status_kurang=0;}
      }
      else if(status_sub_menu==1)
      { lcd.clear();
        lcd.setCursor(1,0);lcd.print(F("COPYING to:"));
        sprintf(lcd_buffer,"i%02d",index_tujuan);
        lcd.setCursor(12, 0);lcd.print(lcd_buffer);
        index_mode[index_tujuan]=index_mode[index];
        index_sensor[index_tujuan]=index_sensor[index];
        index_sensor_mode[index_tujuan]=index_sensor_mode[index];
        index_brake[index_tujuan]=index_brake[index];
        index_delay[index_tujuan]=index_delay[index];
        index_motor0[index_tujuan]=index_motor0[index];
        index_motor1[index_tujuan]=index_motor1[index];
        index_timer[index_tujuan]=index_timer[index];
        index_pwm[index_tujuan]=index_pwm[index];
        index_vmax[index_tujuan]=index_vmax[index];
        index_nilai_pid[index_tujuan]=index_nilai_pid[index];
        delay(3000);
        lcd.clear();
        status_menu_index=0;status_sub_menu=1;
      }
    }
    else if(status_menu_index==6)
    {
      if(status_sub_menu==0)
      {
        lcd.setCursor(1,0);lcd.print(F("MOVE to :"));
        sprintf(lcd_buffer,"i%02d",index_tujuan);
        lcd.setCursor(11,0);lcd.print(lcd_buffer);lcd.setCursor(14,0);lcd.write(127);
        lcd.setCursor(2,1);lcd.print(F("<D1 for ~OK>"));
        if(status_tambah==1)
        {index_tujuan++;if(index_tujuan>99)index_tujuan=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_tujuan--;if(index_tujuan>99)index_tujuan=99;status_kurang=0;}
      }
      else if(status_sub_menu==1)
      { lcd.clear();
        lcd.setCursor(1,0);lcd.print(F("MOVING to:"));
        sprintf(lcd_buffer,"i%02d",index_tujuan);
        lcd.setCursor(12, 0);lcd.print(lcd_buffer);
        if((index_tujuan-index)>0)
        {
          for(byte j=99;j>=index_tujuan;j--)
          {
            index_mode[j]=index_mode[j-(index_tujuan-index)];
            index_sensor[j]=index_sensor[j-(index_tujuan-index)];
            index_sensor_mode[j]=index_sensor_mode[j-(index_tujuan-index)];
            index_brake[j]=index_brake[j-(index_tujuan-index)];
            index_delay[j]=index_delay[j-(index_tujuan-index)];
            index_motor0[j]=index_motor0[j-(index_tujuan-index)];
            index_motor1[j]=index_motor1[j-(index_tujuan-index)];
            index_timer[j]=index_timer[j-(index_tujuan-index)];
            index_pwm[j]=index_pwm[j-(index_tujuan-index)];
            index_vmax[j]=index_vmax[j-(index_tujuan-index)];
            index_nilai_pid[j]=index_nilai_pid[j-(index_tujuan-index)];
          }
          for(byte j=index;j<index_tujuan;j++)
          {
            index_mode[j]=0;
            index_sensor[j]=0;
            index_sensor_mode[j]=0;
            index_brake[j]=0;
            index_delay[j]=0;
            index_motor0[j]=0;
            index_motor1[j]=0;
            index_timer[j]=0;
            index_pwm[j]=0;
            index_vmax[j]=0;
            index_nilai_pid[j]=0;
          }
        }
        else if((index_tujuan-index)<0)
        {
          for(byte j=index;j<=99;j++)
          {
            index_mode[j-(index-index_tujuan)]=index_mode[j];
            index_sensor[j-(index-index_tujuan)]=index_sensor[j];
            index_sensor_mode[j-(index-index_tujuan)]=index_sensor_mode[j];
            index_brake[j-(index-index_tujuan)]=index_brake[j];
            index_delay[j-(index-index_tujuan)]=index_delay[j];
            index_motor0[j-(index-index_tujuan)]=index_motor0[j];
            index_motor1[j-(index-index_tujuan)]=index_motor1[j];
            index_timer[j-(index-index_tujuan)]=index_timer[j];
            index_pwm[j-(index-index_tujuan)]=index_pwm[j];
            index_vmax[j-(index-index_tujuan)]=index_vmax[j];
            index_nilai_pid[j-(index-index_tujuan)]=index_nilai_pid[j];
          }
          for(byte j=99;j>(99-(index-index_tujuan));j--)
          {
            index_mode[j]=0;
            index_sensor[j]=0;
            index_sensor_mode[j]=0;
            index_brake[j]=0;
            index_delay[j]=0;
            index_motor0[j]=0;
            index_motor1[j]=0;
            index_timer[j]=0;
            index_pwm[j]=0;
            index_vmax[j]=0;
            index_nilai_pid[j]=0;
          }
        }
        delay(3000);
        lcd.clear();
        status_menu_index=0;status_sub_menu=1;
      }
    }
  }
  else if(status_menu==4)
  { 
    if(status_menu_pid==0)
    {
      if(status_layar==0){lcd.clear(); status_layar=1;}
      lcd.setCursor(0, 0);lcd.print(F("PID"));
      sprintf(lcd_buffer,"%d",index_pid);
      lcd.setCursor(3, 0);lcd.print(lcd_buffer);
      lcd.setCursor(5, 0);lcd.print(F("| Default:"));
      sprintf(lcd_buffer,"%d",pid_default);
      lcd.setCursor(15, 0);lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"P%03d",kp[index_pid]);
      lcd.setCursor(1, 1);lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"I%03d",ki[index_pid]);
      lcd.setCursor(6, 1);lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"D%03d",kd[index_pid]);
      lcd.setCursor(11, 1);lcd.print(lcd_buffer);
      if(status_sub_menu==0)
      { lcd.setCursor(4,0);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_pid=1;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_menu_pid=1;status_kurang=0;lcd.clear();}
      }
      else if(status_sub_menu==1)
      { lcd.setCursor(4,0);lcd.write(127);
        if(status_tambah==1)
        {index_pid++;if(index_pid>5)index_pid=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_pid--;if(index_pid>5)index_pid=5;status_kurang=0;}
      }
      else status_sub_menu=0;
    }
    else if(status_menu_pid==1)
    {
      lcd.setCursor(2, 0);lcd.print(F("SETTING:PID"));
      sprintf(lcd_buffer,"%d",index_pid);
      lcd.setCursor(13, 0);lcd.print(lcd_buffer); 
      sprintf(lcd_buffer,"P%03d",kp[index_pid]);
      lcd.setCursor(1, 1);lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"I%03d",ki[index_pid]);
      lcd.setCursor(6, 1);lcd.print(lcd_buffer);
      sprintf(lcd_buffer,"D%03d",kd[index_pid]);
      lcd.setCursor(11, 1);lcd.print(lcd_buffer);
      if(status_sub_menu==0)
      { lcd.setCursor(14,0);lcd.print(" ");lcd.setCursor(5,1);lcd.print(" ");
        lcd.setCursor(10,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {status_menu_pid=0;status_tambah=0;lcd.clear();}
        else if(status_kurang==1)
        {status_menu_pid=0;status_kurang=0;lcd.clear();}
      }
      else if(status_sub_menu==1)
      { lcd.setCursor(14,0);lcd.write(127);lcd.setCursor(5,1);lcd.print(" ");
        lcd.setCursor(10,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {index_pid++;if(index_pid>5)index_pid=0;status_tambah=0;}
        else if(status_kurang==1)
        {index_pid--;if(index_pid>5)index_pid=5;status_kurang=0;}
      }
      else if(status_sub_menu==2)
      { lcd.setCursor(14,0);lcd.print(" ");lcd.setCursor(5,1);lcd.write(127);
        lcd.setCursor(10,1);lcd.print(" ");lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {kp[index_pid]++;EEPROM.write(1000+index_pid,kp[index_pid]);status_tambah=0;}
        else if(status_kurang==1)
        {kp[index_pid]--;EEPROM.write(1000+index_pid,kp[index_pid]);status_kurang=0;}
      }
      else if(status_sub_menu==3)
      { lcd.setCursor(14,0);lcd.print(" ");lcd.setCursor(5,1);lcd.print(" ");
        lcd.setCursor(10,1);lcd.write(127);lcd.setCursor(15,1);lcd.print(" ");
        if(status_tambah==1)
        {ki[index_pid]++;EEPROM.write(1006+index_pid,ki[index_pid]);status_tambah=0;}
        else if(status_kurang==1)
        {ki[index_pid]--;EEPROM.write(1006+index_pid,ki[index_pid]);status_kurang=0;}
      }
      else if(status_sub_menu==4)
      { lcd.setCursor(14,0);lcd.print(" ");lcd.setCursor(5,1);lcd.print(" ");
        lcd.setCursor(10,1);lcd.print(" ");lcd.setCursor(15,1);lcd.write(127);
        if(status_tambah==1)
        {kd[index_pid]++;EEPROM.write(1011+index_pid,kd[index_pid]);status_tambah=0;}
        else if(status_kurang==1)
        {kd[index_pid]--;EEPROM.write(1011+index_pid,kd[index_pid]);status_kurang=0;}
      }
      else status_sub_menu=0;
    }
    else if(status_menu_pid==2)
    {
      lcd.clear();
      lcd.setCursor(0, 0);lcd.print(F("SAVING"));
      lcd.setCursor(0, 1);lcd.print(F("PID Default:"));
      lcd.setCursor(12, 1);lcd.print(F("PID"));
      lcd.setCursor(15, 1);lcd.print(index_pid);
      nilai_pid = pid_default = index_pid;
      EEPROM.write(14,nilai_pid);
      delay(100000);
      lcd.clear();
      status_menu_pid=0;
    }
  }
  else if(status_menu==5)
  { if(status_layar==0){lcd.clear(); status_layar=1;}
    if(status_sub_menu==0)
    {
      lcd.setCursor(0,0);
      lcd.print(F("Test ~PID Motion"));
      lcd.setCursor(0,1);
      lcd.print(F("START PRESS <D1>"));
    }
    else if(status_sub_menu==1)
    {
      lcd.clear();lcd.setCursor(0,0);lcd.print(F("STOP PRESS~ <D1>"));
      status_pid=1;status_sub_menu++;
    }
    else if(status_sub_menu==2)
    {
//      sprintf(lcd_buffer,"R%+04d",(int)output_motor[0]);
//      lcd.setCursor(2, 1);
//      lcd.print(lcd_buffer);
//      sprintf(lcd_buffer,"L%+04d",(int)output_motor[1]);
//      lcd.setCursor(9, 1);
//      lcd.print(lcd_buffer);
    }
    else if(status_sub_menu==3)
    {
      status_pid=0;status_sub_menu++;lcd.clear();
    }
    else status_sub_menu=0;
  }
//    delay(20);
}

void timer_int()
{ ///////////////TOMBOL//////////////////////////
  if(digitalRead(0)==0&&status_kalibrasi==0&&status_menu==1){if(++timer_status_kalibrasi==1500)status_kalibrasi=2;}
  else if(digitalRead(0)==0&&status_kalibrasi==2&&status_menu==1){if(++timer_status_kalibrasi==1500)status_kalibrasi=3;}
  else timer_status_kalibrasi=0;

  if(digitalRead(0)==0&&status_menu==3){if(++timer_status_index==1500){status_menu_index=3;status_sub_menu=0;status_layar=0;}}
  else timer_status_index=0;

  if(digitalRead(0)==0&&status_menu==4&&status_menu_pid!=1){if(++timer_status_menu_pid==1500){status_menu_pid=2;status_sub_menu=0;}}
  else timer_status_menu_pid=0;
  
  if(tombol_menu==0&&digitalRead(0)!=tombol_menu&&status_kalibrasi==0&&(status_menu!=5||status_sub_menu!=2)&&(status_menu!=0||status_menu_start!=1||status_sub_menu!=2)&&(status_menu!=3||status_menu_index<3)&&(status_menu!=4||status_menu_pid<1))
  {
    status_menu++;status_sub_menu=0;status_menu_index=0;status_layar=0;
    if(status_menu > 5)status_menu=0;
  }
  if(tombol_sub_menu==1&&digitalRead(1)!=tombol_sub_menu) status_sub_menu++;
  if(tombol_tambah==1&&digitalRead(4)!=tombol_tambah&&status_menu!=5) status_tambah=1;
  else if(tombol_kurang==1&&digitalRead(2)!=tombol_kurang&&status_menu!=5) status_kurang=1;
  if(digitalRead(4)==0){if(++timer_tambah>800)status_tambah=1;}
  else if(digitalRead(2)==0){if(++timer_kurang>800)status_kurang=1;}
  else{timer_tambah=0;timer_kurang=0;}
  tombol_menu=digitalRead(0);
  tombol_sub_menu=digitalRead(1);
  tombol_tambah=digitalRead(4);tombol_kurang=digitalRead(2);
  ////////////////////TIMER-TIMER////////////////////////////////
  if(delay_default!=0)
  { if(pewaktu==25){delay_default--;pewaktu=0;}
    else pewaktu++;}
  else if(timer_brake!=0)
  { if(pewaktu==5){timer_brake--;pewaktu=0;}
    else pewaktu++;}
  else if(timer_delay!=0)
  { if(pewaktu==5){timer_delay--;pewaktu=0;}
    else pewaktu++;}
  else if(timer_timer!=0)
  { if(pewaktu==25){timer_timer--;pewaktu=0;}
    else pewaktu++;}
  ////////////////////////SENSOR//////////////////////////////
  if(status_pid!=0)
  {
    digitalWrite(14, LOW);
    delayMicroseconds(90);
    for(byte i=0; i<7; i++) sensor[i] =  analogRead(7-i);
  
    digitalWrite(14, HIGH);
    delayMicroseconds(90);
    for(byte i=0; i<7; i++) sensor[i+7] =  analogRead(i+1);
  }
  else if(pewaktu_sensor==1)
  {
    digitalWrite(14, LOW);
    delayMicroseconds(90);
    for(byte i=0; i<7; i++) sensor[i] =  analogRead(7-i);
  
    digitalWrite(14, HIGH);
    delayMicroseconds(90);
    for(byte i=0; i<7; i++) sensor[i+7] =  analogRead(i+1);
    pewaktu_sensor=0;
  }
  else pewaktu_sensor=1;

  
  if(status_kalibrasi==0||status_kalibrasi==1)
  {
    nilai_sensor=0;
    for(byte i=0;i<14;i++)
    {
     if(status_garis==0)
     {if(sensor[i]>threshold[i]) nilai_sensor |= 1 << i;
      else nilai_sensor |= 0 << i;}
     else
     {if(sensor[i]>threshold[i]) nilai_sensor |= 0 << i;
      else nilai_sensor |= 1 << i;}
    } 
    nilai_sensor |= 1 << 14;
    nilai_sensor |= 1 << 15;
  }
  else if(status_kalibrasi==2)
  {
    for(byte i=0;i<14;i++)
    {
     if(min_sensor[i]>sensor[i])min_sensor[i]=sensor[i];
     if(max_sensor[i]<sensor[i])max_sensor[i]=sensor[i];
    }
  }
  ////////////////////////////////POSISI////////////////////////////
  posisi_sensor();
  ////////////////PID//////////////////////
  pid(setpoint,pwm_awal,v_max);

  switch(status_mulai)
  {
    case 0: break;
    case 1: if(delay_default==0)
            {
              switch(index_jalan)
              {
                case 0: jalan();break;
                case 1: jalan();break;
                case 2: jalan();break;
                case 3: jalan();break;
                case 4: jalan();break;
                case 5: jalan();break;
                case 6: jalan();break;
                case 7: jalan();break;
                case 8: jalan();break;
                case 9: jalan();break;
                case 10: jalan();break;
                case 11: jalan();break;
                case 12: jalan();break;
                case 13: jalan();break;
                case 14: jalan();break;
                case 15: jalan();break;
                case 16: jalan();break;
                case 17: jalan();break;
                case 18: jalan();break;
                case 19: jalan();break;
                case 20: jalan();break;
                case 21: jalan();break;
                case 22: jalan();break;
                case 23: jalan();break;
                case 24: jalan();break;
                case 25: jalan();break;
                case 26: jalan();break;
                case 27: jalan();break;
                case 28: jalan();break;
                case 29: jalan();break;
                case 30: jalan();break;
                case 31: jalan();break;
                case 32: jalan();break;
                case 33: jalan();break;
                case 34: jalan();break;
                case 35: jalan();break;
                case 36: jalan();break;
                case 37: jalan();break;
                case 38: jalan();break;
                case 39: jalan();break;
                case 40: jalan();break;
                case 41: jalan();break;
                case 42: jalan();break;
                case 43: jalan();break;
                case 44: jalan();break;
                case 45: jalan();break;
                case 46: jalan();break;
                case 47: jalan();break;
                case 48: jalan();break;
                case 49: jalan();break;
                case 50: jalan();break;
                case 51: jalan();break;
                case 52: jalan();break;
                case 53: jalan();break;
                case 54: jalan();break;
                case 55: jalan();break;
                case 56: jalan();break;
                case 57: jalan();break;
                case 58: jalan();break;
                case 59: jalan();break;
                case 60: jalan();break;
                case 61: jalan();break;
                case 62: jalan();break;
                case 63: jalan();break;
                case 64: jalan();break;
                case 65: jalan();break;
                case 66: jalan();break;
                case 67: jalan();break;
                case 68: jalan();break;
                case 69: jalan();break;
                case 70: jalan();break;
                case 71: jalan();break;
                case 72: jalan();break;
                case 73: jalan();break;
                case 74: jalan();break;
                case 75: jalan();break;
                case 76: jalan();break;
                case 77: jalan();break;
                case 78: jalan();break;
                case 79: jalan();break;
                case 80: jalan();break;
                case 81: jalan();break;
                case 82: jalan();break;
                case 83: jalan();break;
                case 84: jalan();break;
                case 85: jalan();break;
                case 86: jalan();break;
                case 87: jalan();break;
                case 88: jalan();break;
                case 89: jalan();break;
                case 90: jalan();break;
                case 91: jalan();break;
                case 92: jalan();break;
                case 93: jalan();break;
                case 94: jalan();break;
                case 95: jalan();break;
                case 96: jalan();break;
                case 97: jalan();break;
                case 98: jalan();break;
                case 99: jalan();break;
                default: status_pid=0;
              }
              switch(status_lintasan)
              {
                case 0:if(index_jalan>total_index){status_pid=0;index_jalan=0;status_sub_menu++;}break;
                case 1:if(index_jalan>total_index)index_jalan=0; break;
              }
            }
            break;
  }
}

void pid(int sp,byte pwm_maju,byte v_max)
{
  static int error;
  static float p,i,d,prev_error,output;
  
  if(status_pid==1||status_pid==3)
  {
    error = sp - posisi;
    p = kp[nilai_pid]*error;
    i += ki[nilai_pid]*error/100;
    d = kd[nilai_pid]*(error-prev_error);
    prev_error = error;
    
    if(i>255)i=255;
    else if(i<-255)i=-255;
    
    output = p + i + d;
    if(output>255)output=255;
    else if(output<-255)output=-255;

//    if(pwm_maju+output<-80)
//    { 
//      if(status_percepatan[0]==0){output_motor[0]=-50;status_percepatan[0]=1;}
//      else output_motor[0]-=a;
//      if(output_motor[0]<pwm_maju+output)output_motor[0]=pwm_maju+output;
//    }
//    else 
//    {
//      output_motor[0]=pwm_maju+output;
//      status_percepatan[0]=0;
//    }
//
//    if(pwm_maju-output<-80)
//    { 
//      if(status_percepatan[1]==0){output_motor[1]=-50;status_percepatan[1]=1;}
//      else output_motor[1]-=a;
//      if(output_motor[1]<pwm_maju-output)output_motor[1]=pwm_maju-output;
//    }
//    else 
//    {
//      output_motor[1]=pwm_maju-output;
//      status_percepatan[1]=0;
//    }
     
    output_motor[0]=pwm_maju+output;
    output_motor[1]=pwm_maju-output; 

    if(output_motor[0]>v_max)output_motor[0]=v_max;
    else if(output_motor[0]<-v_max)output_motor[0]=-v_max;
    if(output_motor[1]>v_max)output_motor[1]=v_max;
    else if(output_motor[1]<-v_max)output_motor[1]=-v_max;
  }
  else if(status_pid == 0||status_pid==2)
  {
    p = 0;
    i = 0;
    d = 0;
    error = 0;
    prev_error = 0;
    output = 0;
    output_motor[0] = output_motor[1] = 0;
    status_percepatan[0] = status_percepatan[1] = 0;
  }

  if(status_pid!=2)
  {
    if(output_motor[0]<0)
    {
      analogWrite(5,abs(output_motor[0]));
      analogWrite(6,0);
    }
    else
    {
      analogWrite(5,0);
      analogWrite(6,abs(output_motor[0]));  
    }
    
    if(output_motor[1]<0)
    {
      analogWrite(3,abs(output_motor[1]));
      analogWrite(11,0);
    }
    else
    {
      analogWrite(3,0);
      analogWrite(11,abs(output_motor[1]));    
    }
  }
  
}
