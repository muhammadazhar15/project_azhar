void berhenti()
{
  analogWrite(5,0);
  analogWrite(6,0);
  analogWrite(3,0);
  analogWrite(11,0);
}

//0 ("S10");mode_display=0b1100000000000001;break;
//1 ("S01");mode_display=0b1110000000000000;break;
//2 ("D23");mode_display=0b1100000011100011;break;
//3 ("D32");mode_display=0b1111000111000000;break;
//4 ("M24");mode_display=0b1100000111100110;break;
//5 ("M42");mode_display=0b1101100111100000;break;
//6 ("M33");mode_display=0b1100001111110000;break;
//7 ("M11");mode_display=0b1100010000001000;break;
//8 ("M00");mode_display=0b1100000000000000;break;
//9 ("M01");mode_display=0b1100000000001000;break;
//10("M10");mode_display=0b1100010000000000;break;

void deteksi_sensor()
{
  switch(index_sensor[index_jalan])
  {
    case 0: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: 
              case 2:
              case 3: if(bitRead(nilai_sensor,0)==1){status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 1: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: 
              case 2:
              case 3: if(bitRead(nilai_sensor,13)==1){status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 2: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: if((bitRead(nilai_sensor,0)|bitRead(nilai_sensor,1))|(bitRead(nilai_sensor,5)|bitRead(nilai_sensor,6)|bitRead(nilai_sensor,7))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 2: if((bitRead(nilai_sensor,0)|bitRead(nilai_sensor,1))&(bitRead(nilai_sensor,5)|bitRead(nilai_sensor,6)|bitRead(nilai_sensor,7))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 3: if((bitRead(nilai_sensor,0)&bitRead(nilai_sensor,1))&(bitRead(nilai_sensor,5)&bitRead(nilai_sensor,6)&bitRead(nilai_sensor,7))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 3: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: if((bitRead(nilai_sensor,13)|bitRead(nilai_sensor,12))|(bitRead(nilai_sensor,8)|bitRead(nilai_sensor,7)|bitRead(nilai_sensor,6))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 2: if((bitRead(nilai_sensor,13)|bitRead(nilai_sensor,12))&(bitRead(nilai_sensor,8)|bitRead(nilai_sensor,7)|bitRead(nilai_sensor,6))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 3: if((bitRead(nilai_sensor,13)&bitRead(nilai_sensor,12))&(bitRead(nilai_sensor,8)&bitRead(nilai_sensor,7)&bitRead(nilai_sensor,6))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 4: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: if((bitRead(nilai_sensor,1)|bitRead(nilai_sensor,2))|(bitRead(nilai_sensor,5)|bitRead(nilai_sensor,6)|bitRead(nilai_sensor,7)|bitRead(nilai_sensor,8))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 2: if((bitRead(nilai_sensor,1)|bitRead(nilai_sensor,2))&(bitRead(nilai_sensor,5)|bitRead(nilai_sensor,6)|bitRead(nilai_sensor,7)|bitRead(nilai_sensor,8))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 3: if((bitRead(nilai_sensor,1)&bitRead(nilai_sensor,2))&(bitRead(nilai_sensor,5)&bitRead(nilai_sensor,6)&bitRead(nilai_sensor,7)&bitRead(nilai_sensor,8))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 5: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: if((bitRead(nilai_sensor,12)|bitRead(nilai_sensor,11))|(bitRead(nilai_sensor,8)|bitRead(nilai_sensor,7)|bitRead(nilai_sensor,6)|bitRead(nilai_sensor,5))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 2: if((bitRead(nilai_sensor,12)|bitRead(nilai_sensor,11))&(bitRead(nilai_sensor,8)|bitRead(nilai_sensor,7)|bitRead(nilai_sensor,6)|bitRead(nilai_sensor,5))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 3: if((bitRead(nilai_sensor,12)&bitRead(nilai_sensor,11))&(bitRead(nilai_sensor,8)&bitRead(nilai_sensor,7)&bitRead(nilai_sensor,6)&bitRead(nilai_sensor,5))==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 6: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: 
              case 2: if(bitRead(nilai_sensor,4)|bitRead(nilai_sensor,5)|bitRead(nilai_sensor,6)|bitRead(nilai_sensor,7)|bitRead(nilai_sensor,8)|bitRead(nilai_sensor,9)==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 3: if(bitRead(nilai_sensor,4)&bitRead(nilai_sensor,5)&bitRead(nilai_sensor,6)&bitRead(nilai_sensor,7)&bitRead(nilai_sensor,8)&bitRead(nilai_sensor,9)==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 7: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: if(bitRead(nilai_sensor,3)|bitRead(nilai_sensor,10)==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 2:
              case 3: if(bitRead(nilai_sensor,3)&bitRead(nilai_sensor,10)==1)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 8: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: 
              case 2: if((bitRead(nilai_sensor,0)&bitRead(nilai_sensor,1)&bitRead(nilai_sensor,2)&bitRead(nilai_sensor,3)&bitRead(nilai_sensor,4)&bitRead(nilai_sensor,5)&bitRead(nilai_sensor,6)
                                 &bitRead(nilai_sensor,7)&bitRead(nilai_sensor,8)&bitRead(nilai_sensor,9)&bitRead(nilai_sensor,10)&bitRead(nilai_sensor,11)&bitRead(nilai_sensor,12)&bitRead(nilai_sensor,13))==0)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
              case 3: if((bitRead(nilai_sensor,0)|bitRead(nilai_sensor,1)|bitRead(nilai_sensor,2)|bitRead(nilai_sensor,3)|bitRead(nilai_sensor,4)|bitRead(nilai_sensor,5)|bitRead(nilai_sensor,6)
                                 |bitRead(nilai_sensor,7)|bitRead(nilai_sensor,8)|bitRead(nilai_sensor,9)|bitRead(nilai_sensor,10)|bitRead(nilai_sensor,11)|bitRead(nilai_sensor,12)|bitRead(nilai_sensor,13))==0)
                      {status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 9: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: 
              case 2: 
              case 3: if(bitRead(nilai_sensor,3)==1){status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
    case 10: switch(index_sensor_mode[index_jalan])
            {
              case 0: status_pid=0;timer_brake=index_brake[index_jalan];break;
              case 1: 
              case 2: 
              case 3: if(bitRead(nilai_sensor,10)==1){status_pid=0;timer_brake=index_brake[index_jalan];}break;
            }break;
  }
}

void jalan()
{
  switch(index_mode[index_jalan])
  {
    case 0: if(status_pid==1)
            {deteksi_sensor();}
            else if(status_pid==0)
            {
              berhenti();
              if(timer_brake==0)
              {timer_delay=index_delay[index_jalan];status_pid=2;}
            }
            else if(status_pid==2)
            {
              analogWrite(5,index_motor0[index_jalan]);
              analogWrite(6,0);
              analogWrite(3,0);
              analogWrite(11,index_motor1[index_jalan]);
              if(timer_delay==0)
              {
                timer_timer=index_timer[index_jalan];
                pwm_awal=index_pwm[index_jalan];v_max=index_vmax[index_jalan];nilai_pid=index_nilai_pid[index_jalan];
                status_pid=3;
              }
            }
            else if(status_pid==3)
            {
              if(timer_timer==0)
              {pwm_awal=pwm_default;v_max=v_max_default;nilai_pid=pid_default;index_jalan++;status_pid=1;}
            }
            break;
            
    case 1: if(status_pid==1)
            {deteksi_sensor();}
            else if(status_pid==0)
            {
              berhenti();
              if(timer_brake==0)
              {timer_delay=index_delay[index_jalan];status_pid=2;}
            }
            else if(status_pid==2)
            {
              analogWrite(5,0);
              analogWrite(6,index_motor0[index_jalan]);
              analogWrite(3,index_motor1[index_jalan]);
              analogWrite(11,0);
              if(timer_delay==0)
              {
                timer_timer=index_timer[index_jalan];
                pwm_awal=index_pwm[index_jalan];v_max=index_vmax[index_jalan];nilai_pid=index_nilai_pid[index_jalan];
                status_pid=3;
              }
            }
            else if(status_pid==3)
            {
              if(timer_timer==0)
              {pwm_awal=pwm_default;v_max=v_max_default;nilai_pid=pid_default;index_jalan++;status_pid=1;}
            }
            break;
            
    case 2: if(status_pid==1)
            {deteksi_sensor();}
            else if(status_pid==0)
            {
              berhenti();
              if(timer_brake==0)
              {timer_delay=index_delay[index_jalan];status_pid=2;}
            }
            else if(status_pid==2)
            {
              analogWrite(5,0);
              analogWrite(6,index_motor0[index_jalan]);
              analogWrite(3,0);
              analogWrite(11,index_motor1[index_jalan]);
              if(timer_delay==0)
              {
                timer_timer=index_timer[index_jalan];
                pwm_awal=index_pwm[index_jalan];v_max=index_vmax[index_jalan];nilai_pid=index_nilai_pid[index_jalan];
                status_pid=3;
              }
            }
            else if(status_pid==3)
            {
              if(timer_timer==0)
              {pwm_awal=pwm_default;v_max=v_max_default;nilai_pid=pid_default;index_jalan++;status_pid=1;}
            }
            break;
            
    case 3: if(status_pid==1)
            {deteksi_sensor();}
            else if(status_pid==0)
            {
              berhenti();
              if(timer_brake==0)
              {timer_delay=index_delay[index_jalan];status_pid=2;}
            }
            else if(status_pid==2)
            {
              analogWrite(5,index_motor0[index_jalan]);
              analogWrite(6,0);
              analogWrite(3,index_motor1[index_jalan]);
              analogWrite(11,0);
              if(timer_delay==0)
              {
                timer_timer=index_timer[index_jalan];
                pwm_awal=index_pwm[index_jalan];v_max=index_vmax[index_jalan];nilai_pid=index_nilai_pid[index_jalan];
                status_pid=3;
              }
            }
            else if(status_pid==3)
            {
              if(timer_timer==0)
              {pwm_awal=pwm_default;v_max=v_max_default;nilai_pid=pid_default;index_jalan++;status_pid=1;}
            }
            break;
            
    case 4: if(status_pid==1)
            {deteksi_sensor();}
            else if(status_pid==0)
            {
              berhenti();
              if(timer_brake==0)
              {timer_delay=index_delay[index_jalan];status_pid=2;}
            }
            else if(status_pid==2)
            {
              status_garis=0;
              status_lebar_garis=index_motor0[index_jalan];
              setpoint=(char)index_motor1[index_jalan];
              if(timer_delay==0)
              {
                timer_timer=index_timer[index_jalan];
                pwm_awal=index_pwm[index_jalan];v_max=index_vmax[index_jalan];nilai_pid=index_nilai_pid[index_jalan];
                status_pid=3;
              }
            }
            else if(status_pid==3)
            {
              if(timer_timer==0)
              {pwm_awal=pwm_default;v_max=v_max_default;nilai_pid=pid_default;index_jalan++;status_pid=1;}
            }
            break;
            
    case 5: if(status_pid==1)
            {deteksi_sensor();}
            else if(status_pid==0)
            {
              berhenti();
              if(timer_brake==0)
              {timer_delay=index_delay[index_jalan];status_pid=2;}
            }
            else if(status_pid==2)
            {
              status_garis=1;
              status_lebar_garis=index_motor0[index_jalan];
              setpoint=(char)index_motor1[index_jalan];
              if(timer_delay==0)
              {
                timer_timer=index_timer[index_jalan];
                pwm_awal=index_pwm[index_jalan];v_max=index_vmax[index_jalan];nilai_pid=index_nilai_pid[index_jalan];
                status_pid=3;
              }
            }
            else if(status_pid==3)
            {
              if(timer_timer==0)
              {pwm_awal=pwm_default;v_max=v_max_default;nilai_pid=pid_default;index_jalan++;status_pid=1;}
            }
            break;
  }
}
