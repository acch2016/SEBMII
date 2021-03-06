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
 * @file    0301_Mutex_exemple_avantPapillon.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* TODO: insert other definitions and declarations here. */
const char msg1[] = "\rI'm in task 1\n";
const char msg2[] = "\rHi! this is task 2\n";

SemaphoreHandle_t mutex;

TaskHandle_t task1_handle;
TaskHandle_t task2_handle;

/*
 * @brief   Application entry point.
 */

void task1(void * arg)
{
	char * msg = (char *) arg;

	for (;;)
	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		PRINTF(msg);
		vTaskDelay(pdMS_TO_TICKS(100));
		xSemaphoreGive(mutex);
		vTaskDelay(10);
	}
}

void task2(void * arg)
{
	char * msg = (char *) arg;
	for (;;)
	{
		xSemaphoreTake(mutex, portMAX_DELAY);
		PRINTF(msg);
		xSemaphoreGive(mutex);

	}
}

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    mutex = xSemaphoreCreateMutex();

    xTaskCreate(task1, "tasK1", 110, (void*)msg1, configMAX_PRIORITIES-1, &task1_handle);
    xTaskCreate(task2, "tasK2", 110, (void*)msg1, configMAX_PRIORITIES-2, &task2_handle);
    vTaskStartScheduler();


    return 0 ;
}
