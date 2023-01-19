void posisi_sensor()
{
  switch(status_lebar_garis)
  {
    case 0: switch(nilai_sensor)
            {
              case 0b1100000000000001: posisi=12; break;
              case 0b1100000000000011: posisi=11; break;
              case 0b1100000000000010: posisi=10; break;
              case 0b1100000000000110: posisi=9; break;
              case 0b1100000000000100: posisi=8; break;
              case 0b1100000000001100: posisi=7; break;
              case 0b1100000000001000: posisi=6; break;
              case 0b1100000000011000: posisi=5; break;
              case 0b1100000000010000: posisi=4; break;
              case 0b1100000000110000: posisi=3; break;
              case 0b1100000000100000: posisi=2; break;
              case 0b1100000001100000: posisi=1; break;
              case 0b1100000011000000: posisi=0; break;
              case 0b1100000110000000: posisi=-1; break;
              case 0b1100000100000000: posisi=-2; break;
              case 0b1100001100000000: posisi=-3; break;
              case 0b1100001000000000: posisi=-4; break;
              case 0b1100011000000000: posisi=-5; break;
              case 0b1100010000000000: posisi=-6; break;
              case 0b1100110000000000: posisi=-7; break;
              case 0b1100100000000000: posisi=-8; break;
              case 0b1101100000000000: posisi=-9; break;
              case 0b1101000000000000: posisi=-10; break;
              case 0b1111000000000000: posisi=-11; break;
              case 0b1110000000000000: posisi=-12; break;
              default : posisi=posisi_prev;
            }break;//2cm
            
    case 1: switch(nilai_sensor)
            {
              case 0b1100000000000001: posisi=13; break;
              case 0b1100000000000011: posisi=12; break;
              case 0b1100000000000111: posisi=11; break;
              case 0b1100000000000110: posisi=10; break;
              case 0b1100000000001110: posisi=9; break;
              case 0b1100000000001100: posisi=8; break;
              case 0b1100000000011100: posisi=7; break;
              case 0b1100000000011000: posisi=6; break;
              case 0b1100000000111000: posisi=5; break;
              case 0b1100000000110000: posisi=4; break;
              case 0b1100000001110000: posisi=3; break;
              case 0b1100000001100000: posisi=2; break;
              case 0b1100000011100000: posisi=1; break;
              case 0b1100000011000000: posisi=0; break;
              case 0b1100000111000000: posisi=-1; break;
              case 0b1100000110000000: posisi=-2; break;
              case 0b1100001110000000: posisi=-3; break;
              case 0b1100001100000000: posisi=-4; break;
              case 0b1100011100000000: posisi=-5; break;
              case 0b1100011000000000: posisi=-6; break;
              case 0b1100111000000000: posisi=-7; break;
              case 0b1100110000000000: posisi=-8; break;
              case 0b1101110000000000: posisi=-9; break;
              case 0b1101100000000000: posisi=-10; break;
              case 0b1111100000000000: posisi=-11; break;
              case 0b1111000000000000: posisi=-12; break;
              case 0b1110000000000000: posisi=-13; break;
              default : posisi=posisi_prev;
            }break;//3cm
            
    case 2: switch(nilai_sensor)
            {
              case 0b1100000000000001: posisi=10; break;
              case 0b1100000000000011: posisi=9; break;
              case 0b1100000000000111: posisi=8; break;
              case 0b1100000000001111: posisi=7; break;
              case 0b1100000000011111: posisi=6; break;
              case 0b1100000000011110: posisi=5; break;
              case 0b1100000000111110: posisi=4; break;
              case 0b1100000000111100: posisi=3; break;
              case 0b1100000001111000: posisi=2; break;
              case 0b1100000011110000: posisi=1; break;
              case 0b1100000111100000: posisi=0; break;
              case 0b1100001111000000: posisi=-1; break;
              case 0b1100011110000000: posisi=-2; break;
              case 0b1100111100000000: posisi=-3; break;
              case 0b1101111100000000: posisi=-4; break;
              case 0b1101111000000000: posisi=-5; break;
              case 0b1111111000000000: posisi=-6; break;
              case 0b1111110000000000: posisi=-7; break;
              case 0b1111100000000000: posisi=-8; break;
              case 0b1111000000000000: posisi=-9; break;
              case 0b1110000000000000: posisi=-10; break;
              default : posisi=posisi_prev;
            }break;//4cm
  }
  posisi_prev = posisi;
}
