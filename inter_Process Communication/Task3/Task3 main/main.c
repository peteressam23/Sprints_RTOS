/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "semphr.h"
/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

#define TASK1_STACK_DEPTH  100
#define TASK2_STACK_DEPTH  100
#define TASK3_STACK_DEPTH  100
#define TASK4_STACK_DEPTH  100

TaskHandle_t button0TaskHandler = NULL;
TaskHandle_t button1TaskHandler = NULL;
TaskHandle_t consumerTaskHandler = NULL;
TaskHandle_t writeStringTaskHandler = NULL;
xQueueHandle xQueue;

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/*---------------------Global variables----------------------*/
pinState_t button_state;


void button0_triggerTask( void * pvParameters );
void button1_triggerTask( void * pvParameters );
void consumerTask( void * pvParameters );
void writeStringTask( void * pvParameters );

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
  
	
	
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);
	xQueue = xQueueCreate(15,15*sizeof(uint8_t));


	
	 /* Create the task, storing the handle. */
    xTaskCreate(
                    button0_triggerTask,      
                    "buttonTask 0",          
                    TASK1_STACK_DEPTH,     
                    NULL,    
                    1,
                    &button0TaskHandler );      

    xTaskCreate(
                    button1_triggerTask,       
                    "buttonTask 2",         
                    TASK2_STACK_DEPTH,      
                    NULL,    
                    1,
                    &button1TaskHandler );    
							
    xTaskCreate(
                    consumerTask,       
                    "consumerTask",         
                    TASK3_STACK_DEPTH,      
                    NULL,    
                    1,
                    &consumerTaskHandler );     

    xTaskCreate(
                    writeStringTask,      
                    "writeStringTask",         
                    TASK4_STACK_DEPTH,      
                    NULL,    
                    1,
                    &writeStringTaskHandler );     						
										
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


void button0_triggerTask( void * pvParameters )
{	
	pinState_t initialState = PIN_IS_LOW;
	pinState_t newState;
    while(1)
    {
			newState = GPIO_read(PORT_0,PIN0);
			if((newState == PIN_IS_HIGH) && (initialState == PIN_IS_LOW))
			{
				xQueueSend(xQueue,(uint8_t*)"BTN_0_RISING",( TickType_t ) 13);
			}
			else if((newState == PIN_IS_LOW) && (initialState == PIN_IS_HIGH))
			{
				xQueueSend(xQueue,(uint8_t*)"BTN_0_FALLING",( TickType_t ) 14);
			}
			else
			{
			}
			initialState = newState;
			vTaskDelay(100);
    }
}


void button1_triggerTask( void * pvParameters )
{	
	pinState_t initialState = PIN_IS_LOW;
	pinState_t newState;
    while(1)
    {
			newState = GPIO_read(PORT_0,PIN1);
			if((newState==PIN_IS_HIGH) && (initialState == PIN_IS_LOW))
			{
				xQueueSend(xQueue,(uint8_t*)"BTN_1_RISING",( TickType_t ) 13);
			}
			else if((newState == PIN_IS_LOW) && (initialState == PIN_IS_HIGH))
			{
				xQueueSend(xQueue,(uint8_t*)"BTN_1_FALLING",( TickType_t ) 14);
			}else
			{
			}
			initialState = newState;
			vTaskDelay(100);
    }   
}


void consumerTask( void * pvParameters )
{
	uint8_t string[15];
    while(1)
    {
			xQueueReceive(xQueue,string,( TickType_t ) 15);
			vSerialPutString((const signed char*const)string , 15);
			vTaskDelay(100);
			xSerialPutChar('\n');
		}		
}

void writeStringTask( void * pvParameters )
{
    while(1)
    {
			xQueueSend(xQueue,(uint8_t*)"Hello         ",( TickType_t ) 15);
			vTaskDelay(100);
		}		
}

