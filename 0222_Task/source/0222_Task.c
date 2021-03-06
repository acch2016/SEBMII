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
 * @file    random_print.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
uint8_t semaphore = 0;


void print1_task(void *arg)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(rand()%10);
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		taskDISABLE_INTERRUPTS();//binary semaphore
		if(0 == semaphore)//no funciona la bandera sola porque la instruccion no es atomica entonces escribimos las lineas de habilitar y deshabilitar interrupciones (//binary semaphore)
		{
			semaphore = 1;
			taskENABLE_INTERRUPTS();

			PRINTF("\rThis is a useless message!\n");

			taskDISABLE_INTERRUPTS();
			semaphore = 0;
		}
		taskENABLE_INTERRUPTS();

//		taskDISABLE_INTERRUPTS();//funciona pero es bloqueante y si existe una tarea que tenga un deadline antes que termine de ejecutar la tarea esto no funcionara
//		PRINTF("\rThis is a useless message!\n");
//		taskENABLE_INTERRUPTS();

		vTaskDelay(rand()%10); //crea modulo random entre 0 y 10
	}
}

void print2_task(void *arg)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(rand()%10);
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		taskDISABLE_INTERRUPTS();
		PRINTF("\rAnother useless message here!\n");
		taskENABLE_INTERRUPTS();

		vTaskDelay(rand()%10);
	}
}

int main(void)
{
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();
    srand(0x15458523);//semilla

    xTaskCreate(print1_task, "print1", 110, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(print2_task, "print2", 110, NULL, configMAX_PRIORITIES-1, NULL);
    vTaskStartScheduler();
    for(;;)
    {

    }
    return 0 ;
}
