/*
 * odometry.c
 *
 *  Created on: Feb 25, 2019
 *      Author: ASUS
 */

#include "odometry.h"

short int kecepatan_odometry0;
short int kecepatan_odometry1;

double posisi_x_buffer = 0.0;
double posisi_y_buffer = 0.0;
float posisi_x_offset = 0.0;
float posisi_y_offset = 0.0;
float posisi_x = 0.0;
float posisi_y = 0.0;
float posisi_x_LCD = 0.0;
float posisi_y_LCD = 0.0;

char gyro_status = 0;
float gyro_buffer = 0.0;
float gyro_offset = 90.0;
float gyro_derajat = 90.0;
double gyro_radian = M_PI_2;

// data baru gyro magneto
float gyro_buffer_baru = 0;
float kompas_buffer_baru = 0;

void hitung_odometry(void)
{

	kecepatan_odometry0 = odometry0;
	odometry0 = 0;
	kecepatan_odometry1 = odometry1;
	odometry1 = 0;

	double buffer_x[2];
	double buffer_y[2];

	buffer_x[0] = kecepatan_odometry0 * cosf(gyro_radian + M_PI_4);
	buffer_x[1] = kecepatan_odometry1 * cosf(gyro_radian + M_3PI_4);

	buffer_y[0] = kecepatan_odometry0 * sinf(gyro_radian + M_PI_4);
	buffer_y[1] = kecepatan_odometry1 * sinf(gyro_radian + M_3PI_4);

	posisi_x_buffer += (buffer_x[0] + buffer_x[1]) * odometry_to_cm1;
	posisi_y_buffer += (buffer_y[0] + buffer_y[1]) * odometry_to_cm;

	posisi_x = posisi_x_buffer - Offset_X(gyro_derajat);
	posisi_y = posisi_y_buffer - Offset_Y(gyro_derajat);

//	posisi_x = posisi_x_buffer - (20*(cosf(gyro_derajat)));
//	posisi_y = posisi_y_buffer ;

	posisi_x_LCD = posisi_x - posisi_x_offset;
	posisi_y_LCD = posisi_y - posisi_y_offset;
}

void terima_gyro()
{
	gyro_derajat = (gyro_offset - gyro_buffer);
	gyro_radian = (gyro_offset - gyro_buffer) * (M_PI/180.0);

	if (gyro_derajat > 180.0)
			gyro_derajat -= 360.0;
	if (gyro_derajat < -180.0)
		gyro_derajat += 360.0;
	if (gyro_radian > M_PI)
		gyro_radian -= M_TWOPI;
	if (gyro_radian < (M_PI*-1.0))
		gyro_radian += M_TWOPI;
}

double Offset_X(double x) {
  double terms[] = {
	-5.0394574443225509e-002,
	-2.5239783785525592e-003,
	-2.0857522860124292e-003,
	 1.3649833827668140e-007,
	 4.9039617834308138e-008,
	 -2.1781069071787921e-012,
	 -3.5298112864759803e-013
};

  size_t csz = sizeof terms / sizeof *terms;

  double t = 1;
  double r = 0;
  for (int i = 0; i < csz;i++) {
    r += terms[i] * t;
    t *= x;
  }
  return r;
}

double Offset_Y(double y) {
  double terms[] = {
	   5.0486138917734139e-002,
	   2.5288120634632294e-001,
	  -2.6089510254917583e-005,
	  -1.2157766567126481e-005,
	   2.6966035613736916e-010,
	   1.3468156499751170e-010,
	   1.0281848890722639e-014
};

  size_t csz = sizeof terms / sizeof *terms;

  double t = 1;
  double r = 0;
  for (int i = 0; i < csz;i++) {
    r += terms[i] * t;
    t *= y;
  }
  return r;
}
