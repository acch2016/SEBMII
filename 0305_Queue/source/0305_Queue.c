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
 * @file    0305_Queue.c
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

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"
/* TODO: insert other definitions and declarations here. */
#define STACK_PRINT 30
#define EVENT_SECONDS (1<<0)
#define EVENT_MINUTES (1<<1)
#define EVENT_HOURS (1<<2)

SemaphoreHandle_t seconds_semaphore;
SemaphoreHandle_t minutes_semaphore;
SemaphoreHandle_t hours_semaphore;
QueueHandle_t xQueue;
EventGroupHandle_t g_time_events;
SemaphoreHandle_t mutex;


typedef struct
{
	uint8_t second ;
	uint8_t minute ;
	uint8_t hour ;
} alarm_t;
alarm_t g_alarm;

typedef enum{seconds_type, minutes_type, hours_type} time_types_t;
typedef struct
{
	time_types_t time_type;
	uint8_t value;
} time_msg_t;



/*
 * @brief   Application entry point.
 */

void seconds_task(void *arg)
{
	time_msg_t *message;
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(1000);
	xLastWakeTime = xTaskGetTickCount();
	uint8_t seconds = 0;
	for(;;)
	{
		message = pvPortMalloc(sizeof(time_msg_t));
		if ( seconds == g_alarm.second )
		{
			xEventGroupSetBits(g_time_events,EVENT_SECONDS);
		}else{
			xEventGroupClearBits(g_time_events,EVENT_SECONDS);
		}

    	vTaskDelayUntil(&xLastWakeTime, xPeriod);
		seconds++;
		if (60 == seconds) {
			seconds = 0;
			xSemaphoreGive(minutes_semaphore);
		}
		message->time_type = seconds_type;
		message->value = seconds;
		xQueueSend( xQueue,&message,portMAX_DELAY);

	}
}

void minutes_task(void *arg)
{
	time_msg_t *message;
	uint8_t minutes = 0;
	for(;;)
	{
		message = pvPortMalloc(sizeof(time_msg_t));
		if ( minutes == g_alarm.minute )
		{
			xEventGroupSetBits(g_time_events,EVENT_MINUTES);
		}else{
			xEventGroupClearBits(g_time_events,EVENT_MINUTES);
		}

		xSemaphoreTake(minutes_semaphore,portMAX_DELAY);
		minutes ++;
		if (60 == minutes)
		{
			minutes=0;
			xSemaphoreGive(hours_semaphore);
		}

		message->time_type = minutes_type;
		message->value = minutes;
		xQueueSend( xQueue,&message,portMAX_DELAY);
	}

}

void hours_task(void *arg)
{
	time_msg_t *message;
	uint8_t hours = 0;
	for(;;)
	{
		message = pvPortMalloc(sizeof(time_msg_t));
		if ( hours == g_alarm.hour )
		{
			xEventGroupSetBits(g_time_events,EVENT_HOURS);

		}else{
			xEventGroupClearBits(g_time_events,EVENT_HOURS);

		}
		xSemaphoreTake(hours_semaphore,portMAX_DELAY);

		//message = pvPortMalloc(sizeof(time_msg_t));
		hours ++;
		if (24 == hours)
		{
			hours=0;
		}
		message->time_type = hours_type;
		message->value = hours;
		xQueueSend( xQueue,&message,portMAX_DELAY);

	}
}

void alarm_task()
{

	for(;;)
	{
		xEventGroupWaitBits(g_time_events, EVENT_MINUTES | EVENT_HOURS | EVENT_SECONDS, pdTRUE, pdTRUE, portMAX_DELAY);
		xSemaphoreTake(mutex, portMAX_DELAY);
		PRINTF("ALARM\n\r");
		xSemaphoreGive(mutex);
	}

}

void print_task()
{
	time_msg_t *message;
	message = pvPortMalloc(sizeof(time_msg_t));
	static uint8_t print_second= 0;
	static uint8_t print_minute= 0;
	static uint8_t print_hour= 0;

	for(;;)
	{
		xQueueReceive(xQueue,&message,1000);

		if(message->time_type == hours_type ){
			print_hour = message->value;
		}

		if(message->time_type == minutes_type ){
			print_minute = message->value;
		}

		if(message->time_type == seconds_type ){
			print_second = message->value;
		}

		xSemaphoreTake(mutex, portMAX_DELAY);
		PRINTF("%2d:%2d:%2d\n\r",print_hour, print_minute,print_second);
		xSemaphoreGive(mutex);
		vPortFree(message);
	}

}

int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();

	g_alarm.hour = 0;
	g_alarm.minute = 1;
	g_alarm.second = 10;

	seconds_semaphore = xSemaphoreCreateBinary();
	minutes_semaphore = xSemaphoreCreateBinary();
	hours_semaphore = xSemaphoreCreateBinary();

	g_time_events = xEventGroupCreate();
	xQueue = xQueueCreate(3,sizeof( time_msg_t));

	mutex = xSemaphoreCreateMutex();


	xTaskCreate(seconds_task, "seconds_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
	xTaskCreate(minutes_task, "minutes_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
	xTaskCreate(hours_task,   "hours_task",   configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
	xTaskCreate(print_task,   "print_task",   configMINIMAL_STACK_SIZE+STACK_PRINT, NULL, configMAX_PRIORITIES, NULL);
	xTaskCreate(alarm_task,   "alarm_task",   configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);


	vTaskStartScheduler();

	while(1) {

	}

	return 0 ;
}
