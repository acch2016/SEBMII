/*
 * user.c
 *
 *  Created on: 30/11/2016
 *      Author: jlpe
 */

#include "user.h"
#include "board.h"
#include "fsl_debug_console.h"



 void blue_task(void *pvParameters) {

  int8_t * pTaskName;
  pTaskName = (int8_t* ) pvParameters;


  for (;;) {
 	  PRINTF(pTaskName);

 	  LED_BLUE_TOGGLE();
 	 vTaskDelay(1000);
   }
 }



 void green_task(void *pvParameters) {

  int8_t * pTaskName;
  pTaskName = (int8_t* ) pvParameters;

  for (;;) {
	PRINTF(pTaskName);

    LED_GREEN_TOGGLE();
    vTaskDelay(1000);
   }
 }





