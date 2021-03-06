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
 * @file    0307_Tarea7_binary-countingSemaphore.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
/* TODO: insert other include files here. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* TODO: insert other definitions and declarations here. */
SemaphoreHandle_t b_led_semaphore;
SemaphoreHandle_t g_led_semaphore;

void PORTA_IRQHandler()
 {
 	BaseType_t xHigherPriorityTaskWoken;
 	PORT_ClearPinsInterruptFlags(PORTA, 1<<4);
 	xHigherPriorityTaskWoken = pdFALSE;
 	xSemaphoreGiveFromISR( b_led_semaphore, &xHigherPriorityTaskWoken );
 	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 }

void PORTC_IRQHandler()
 {
 	BaseType_t xHigherPriorityTaskWoken;
 	PORT_ClearPinsInterruptFlags(PORTC, 1<<6);
 	xHigherPriorityTaskWoken = pdFALSE;
 	xSemaphoreGiveFromISR( g_led_semaphore, &xHigherPriorityTaskWoken );
 	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 }

void blue_task(void *arg)
{
	for (;;)
	{
		xSemaphoreTake(b_led_semaphore, portMAX_DELAY);
		GPIO_TogglePinsOutput(GPIOB, 1 << 21);
	}
}

void green_task(void *arg)
{
	for (;;)
	{
		if ( 10 == uxSemaphoreGetCount(g_led_semaphore))
		{
			xSemaphoreTake(g_led_semaphore, portMAX_DELAY);
			g_led_semaphore = xSemaphoreCreateCounting( 10, 0 );
			GPIO_TogglePinsOutput(GPIOE, 1 << 26);
		}
	}
}
int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /////////////////////////////    LED    /////////////////////

 	CLOCK_EnableClock(kCLOCK_PortB);//B
 	CLOCK_EnableClock(kCLOCK_PortE);//G

 	port_pin_config_t config_led =
 	 	{
 	 			kPORT_PullDisable, kPORT_SlowSlewRate, kPORT_PassiveFilterDisable,
 	 			kPORT_OpenDrainDisable, kPORT_LowDriveStrength, kPORT_MuxAsGpio,
 	 			kPORT_UnlockRegister,
 	 	};

 	 	PORT_SetPinConfig(PORTB, 21, &config_led);//B
 	 	PORT_SetPinConfig(PORTE, 26, &config_led);//G



 	 	gpio_pin_config_t led_config_gpio =
 	 	{
 	 			kGPIO_DigitalOutput, 1
 	 	};

 	 	GPIO_PinInit(GPIOB, 21, &led_config_gpio);//B
 		GPIO_PinInit(GPIOE, 26, &led_config_gpio);//G

 	 	///////////////////////////////////	SW   //////////////////////////////

 		CLOCK_EnableClock(kCLOCK_PortA); // PTA4 // SW3
 		CLOCK_EnableClock(kCLOCK_PortC); // PTC6 // SW2

 	 	port_pin_config_t config_switch =
 	 	{
 	 			kPORT_PullDisable, kPORT_SlowSlewRate, kPORT_PassiveFilterDisable,
 	 			kPORT_OpenDrainDisable, kPORT_LowDriveStrength, kPORT_MuxAsGpio,
 	 			kPORT_UnlockRegister
 	 	};

 	 	PORT_SetPinInterruptConfig(PORTA, 4, kPORT_InterruptFallingEdge);// SW3
 	 	PORT_SetPinInterruptConfig(PORTC, 6, kPORT_InterruptFallingEdge);// SW2

 	 	PORT_SetPinConfig(PORTA, 4, &config_switch); // SW3
 		PORT_SetPinConfig(PORTC, 6, &config_switch); // SW2

 	 	gpio_pin_config_t switch_config_gpio =
 	 	{
 	 			kGPIO_DigitalInput, 1
 	 	};

 	 	GPIO_PinInit(GPIOA, 4, &switch_config_gpio);// SW3
 		GPIO_PinInit(GPIOC, 6, &switch_config_gpio);// SW2

 	 	NVIC_EnableIRQ(PORTA_IRQn);// SW3
 		NVIC_EnableIRQ(PORTC_IRQn);// SW2
 	 	NVIC_SetPriority(PORTA_IRQn,5);// SW3
 	 	NVIC_SetPriority(PORTC_IRQn,5);// SW2

 	 	///////////////////////////////  SW config End //////////////////////////

 	 	b_led_semaphore = xSemaphoreCreateBinary();
	    /* Create a counting semaphore that has a maximum count of 10 and an
	    initial count of 1. */
		g_led_semaphore = xSemaphoreCreateCounting( 10, 0 );
	    if( g_led_semaphore != NULL )
	    {
	        PRINTF("The semaphore was created successfully");
	    }

 	 	xTaskCreate(blue_task, "blue_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-1, NULL);
 	 	xTaskCreate(green_task, "green_task", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES-2, NULL);

 	 	vTaskStartScheduler();
 	 	while (1)
 	 	{

 	 	}
 	 	return 0;
 	 }
