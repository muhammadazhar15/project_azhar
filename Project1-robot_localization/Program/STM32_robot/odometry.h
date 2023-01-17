/*
 * odometry.h
 *
 *  Created on: Feb 25, 2019
 *      Author: ASUS
 */

#ifndef ODOMETRY_H_
#define ODOMETRY_H_

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

#define odometry0 TIM4->CNT
#define odometry1 TIM3->CNT

#define odometry_to_cm 	0.019488
#define odometry_to_cm1 0.018804

double posisi_x_buffer;
double posisi_y_buffer;
float posisi_x_offset;
float posisi_y_offset;
float posisi_x;
float posisi_y;
float posisi_x_LCD;
float posisi_y_LCD;

char gyro_status;

float gyro_buffer;
float gyro_offset;
float gyro_derajat;
double gyro_radian;

float gyro_buffer_baru;
float kompas_buffer_baru;

void hitung_odometry(void);
void terima_gyro(void);
double Offset_X(double x);
double Offset_Y(double y);

#endif /* ODOMETRY_H_ */
