/*
 * net.h
 *
 *  Created on: Jan 15, 2019
 *      Author: ASUS
 */

#ifndef NET_H_
#define NET_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "lwip.h"
#include "lwip/udp.h"
///////////////////////////////
void udp_client_connect(void);
void TIM1_Callback(void);


#endif /* NET_H_ */
