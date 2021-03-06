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
 * @file    0131_PIT_LED.c
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
#include "fsl_pit.h"


/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */
uint8_t counter = 0;
uint8_t counterSW2 = 1;
uint8_t counterSW3 = 1;

/*
 * @brief   Application entry point.
 */

int main(void) {

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /////////////////////////////    LED    /////////////////////

	CLOCK_EnableClock(kCLOCK_PortB);
	CLOCK_EnableClock(kCLOCK_PortE);

	port_pin_config_t config_led =
	{ kPORT_PullDisable, kPORT_SlowSlewRate, kPORT_PassiveFilterDisable,
			kPORT_OpenDrainDisable, kPORT_LowDriveStrength, kPORT_MuxAsGpio,
			kPORT_UnlockRegister, };

	PORT_SetPinConfig(PORTB, 21, &config_led);
	PORT_SetPinConfig(PORTB, 22, &config_led);
	PORT_SetPinConfig(PORTE, 26, &config_led);

	gpio_pin_config_t led_config_gpio =
	{ kGPIO_DigitalOutput, 1 };

	GPIO_PinInit(GPIOB, 21, &led_config_gpio);
	GPIO_PinInit(GPIOB, 22, &led_config_gpio);
	GPIO_PinInit(GPIOE, 26, &led_config_gpio);

	///////////////////////////////////	SW   //////////////////////////////

	CLOCK_EnableClock(kCLOCK_PortA); // PTA4 // SW3
	CLOCK_EnableClock(kCLOCK_PortC); // PTC6 // SW2

	port_pin_config_t config_switch =
	{ kPORT_PullDisable, kPORT_SlowSlewRate,
			kPORT_PassiveFilterDisable, kPORT_OpenDrainDisable,
			kPORT_LowDriveStrength, kPORT_MuxAsGpio, kPORT_UnlockRegister };

	PORT_SetPinInterruptConfig(PORTA, 4, kPORT_InterruptFallingEdge);
	PORT_SetPinInterruptConfig(PORTC, 6, kPORT_InterruptFallingEdge);

	PORT_SetPinConfig(PORTA, 4, &config_switch); // SW3
	PORT_SetPinConfig(PORTC, 6, &config_switch); // SW2

	gpio_pin_config_t switch_config_gpio =
	{ kGPIO_DigitalInput, 1 };

	GPIO_PinInit(GPIOA, 4, &switch_config_gpio);
	GPIO_PinInit(GPIOC, 6, &switch_config_gpio);

	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);

////////////////////////////////////////////	PIT     ////////////////////

    pit_config_t pit_config;
    PIT_GetDefaultConfig(&pit_config);
    //CLOCK_EnableClock(kCLOCK_Pit0);
    //MCR
    PIT_Init(PIT, &pit_config);
    PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, CLOCK_GetBusClkFreq()*(1.5));
    PIT_GetStatusFlags(PIT, kPIT_Chnl_0);
    PIT_StartTimer(PIT, kPIT_Chnl_0);
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
    NVIC_EnableIRQ(PIT0_IRQn);


    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
    }
    return 0 ;
}

void PIT0_IRQHandler()
{
	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);

		if (2 == counterSW3)
		{
			counter --;
			if (0 == counter)
			{
				counter = 3;
			}
		}
		else
		{
			counter ++;
			if (4 == counter)
			{
				counter = 1;
			}
		}

	switch (counter)
	{
	case 1:
		GPIO_WritePinOutput(GPIOB, 21, 1);    //B
		GPIO_WritePinOutput(GPIOE, 26, 1);    //G
		GPIO_WritePinOutput(GPIOB, 22, 0);    //R ON
		break;
	case 2:
		GPIO_WritePinOutput(GPIOB, 21, 1);    //B
		GPIO_WritePinOutput(GPIOB, 22, 1);    //R
		GPIO_WritePinOutput(GPIOE, 26, 0);    //G ON
		break;
	default:
		GPIO_WritePinOutput(GPIOB, 22, 1);    //R
		GPIO_WritePinOutput(GPIOE, 26, 1);    //G
		GPIO_WritePinOutput(GPIOB, 21, 0);    //B ON
	}


}

void PORTA_IRQHandler() // SW3 cambio de sentido
{
	PORT_ClearPinsInterruptFlags(PORTA, 1<<4);

	counterSW3 ++;

	if (3 == counterSW3)
	{
		counterSW3 = 1;
	}

}

void PORTC_IRQHandler() // SW2 la secuencia pare y el led se quede estatico
{
	PORT_ClearPinsInterruptFlags(PORTC, 1<<6);

	counterSW2 ++;

	if (3 == counterSW2)
	{
		counterSW2 = 1;
	}
	if (2 == counterSW2)
	{
		PIT_StopTimer(PIT, kPIT_Chnl_0);
	}
	else
	{
		PIT_StartTimer(PIT, kPIT_Chnl_0);
	}
}
