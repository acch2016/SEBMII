/*
 * Copyright (c) 2017, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @file    0221_FreeRTOS.c
 * @brief   Application entry point.
 */
#include <stdio.h>
//#include "stdint.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"

/* TODO: insert other include files here. */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* TODO: insert other definitions and declarations here. */

/* Task priorities. */
#define dummy_task1_PRIORITY (configMAX_PRIORITIES - 2)
#define dummy_task2_PRIORITY (configMAX_PRIORITIES - 1)
#define dummy_task3_PRIORITY (configMAX_PRIORITIES - 2)

const int8_t * pTextFor_dummy_task1 = "IN TASK 1: %i +++++++++++++++\r\n";
const int8_t * pTextFor_dummy_task2 = "IN TASK 2: %i ***************\r\n";
const int8_t * pTextFor_dummy_task3 = "IN TASK 3: %i ---------------\r\n";

TaskHandle_t taskHandle_dummy_task1;
TaskHandle_t taskHandle_dummy_task2;
TaskHandle_t taskHandle_dummy_task3;

/*
 * @brief   Application entry point.
 */

void dummy_task1(void *pvParameters) {

	int8_t * pTaskName;
	pTaskName = (int8_t*) pvParameters;

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(2000);
	// Initialize the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	uint8_t counter = 0;
	for (;;) {

		PRINTF(pTaskName, counter);
		counter++;
		vTaskDelayUntil( &xLastWakeTime, xPeriod );

	}
}
void dummy_task2(void *pvParameters) {

	int8_t * pTaskName;
	pTaskName = (int8_t*) pvParameters;

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(4000);
	// Initialize the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	uint8_t counter = 0;
	for (;;) {

		PRINTF(pTaskName, counter);
		counter++;
		vTaskDelayUntil( &xLastWakeTime, xPeriod );

	}
}
void dummy_task3(void *pvParameters) {

	int8_t * pTaskName;
	pTaskName = (int8_t*) pvParameters;

	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(1000);
	// Initialize the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	uint8_t counter = 0;
	for (;;) {

		PRINTF(pTaskName, counter);
		counter++;
		vTaskDelayUntil( &xLastWakeTime, xPeriod );

	}
}


int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    xTaskCreate(dummy_task1, "dummy_task1", configMINIMAL_STACK_SIZE, (void*)pTextFor_dummy_task1, dummy_task1_PRIORITY, taskHandle_dummy_task1);
    xTaskCreate(dummy_task2, "dummy_task2", configMINIMAL_STACK_SIZE, (void*)pTextFor_dummy_task2, dummy_task2_PRIORITY, taskHandle_dummy_task2);
    xTaskCreate(dummy_task3, "dummy_task3", configMINIMAL_STACK_SIZE, (void*)pTextFor_dummy_task3, dummy_task3_PRIORITY, taskHandle_dummy_task3);
//    Pudo ser asi:
//    xTaskCreate(dummy_task1, "dummy_task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    vTaskStartScheduler();


    /* Force the counter to be placed into memory. */
//    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
//        i++ ;
        __asm("NOP");
    }
    return 0 ;
}
