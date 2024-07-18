/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "task_led.h"
#include "task_ui.h"

/********************** macros and definitions *******************************/
#define WAITING_PROCESS_CONNECTION_MS_           (4000)
#define TASK_PERIOD_LED_MS_                      (1000)
#define MAX_CONNECTION_                          (1)

#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(led_color_t))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
static int task_cnt_;
static int id = 0;
/********************** external data definition *****************************/
extern ao_led_handle_t ao_led;
/********************** internal functions definition ************************/


/********************** external functions definition ************************/

static void delete_task_(void)
{
  LOGGER_INFO("Elimino tarea");
  task_cnt_--;
  LOGGER_INFO("Cantidad de procesos: %d", task_cnt_);
  vTaskDelete(NULL);
}

static void task_led_(void *argument)
{
	ao_led_handle_t* hao = (ao_led_handle_t*)argument;
	while (true)
	{
		led_color_t msg;
		while (pdPASS == xQueueReceive(hao->hqueue_led, (void*)&msg, 0))
		{
			LOGGER_INFO("Procesando conexión: %d", id);
			vTaskDelay((TickType_t)(WAITING_PROCESS_CONNECTION_MS_ / portTICK_PERIOD_MS));
			switch (msg)
			{
			case LED_COLOR_RED:
				LOGGER_INFO("led red");
				HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_SET);
				vTaskDelay((TickType_t)(TASK_PERIOD_LED_MS_ / portTICK_PERIOD_MS));
				HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
				LOGGER_INFO("Fin de la conexión: %d", id);
				id++;
				break;
			case LED_COLOR_GREEN:
				LOGGER_INFO("led green");
				HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
				vTaskDelay((TickType_t)(TASK_PERIOD_LED_MS_ / portTICK_PERIOD_MS));
				HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
				LOGGER_INFO("Fin de la conexión: %d", id);
				id++;
				break;
			case LED_COLOR_BLUE:
				LOGGER_INFO("led blue");
				HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
				vTaskDelay((TickType_t)(TASK_PERIOD_LED_MS_ / portTICK_PERIOD_MS));
				HAL_GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
				LOGGER_INFO("Fin de la conexión: %d", id);
				id++;
				break;
			default:
				break;
			}
		}
		delete_task_();
	}
}

static bool create_new_task_ao_led_(ao_led_handle_t* hao)
{
  LOGGER_INFO("Crear nueva tarea");
  if(task_cnt_ < MAX_CONNECTION_)
  {
    BaseType_t status;
    status = xTaskCreate(task_led_, "task_led", 128, (void* const)hao, tskIDLE_PRIORITY + 1, NULL);
    if(pdPASS != status)
    {
      LOGGER_INFO("No es posible crear mas tareas");
      return false;
    }
    LOGGER_INFO("Nueva tarea creada");
    task_cnt_++;
    LOGGER_INFO("Cantidad de procesos: %d", task_cnt_);
    return true;
  }
  else
  {
    LOGGER_INFO("Maxima cantidad de tareas creadas");
    return false;
  }
}

/********************** external functions definition ************************/
bool oa_led_send_msg(ao_led_handle_t* hao , led_color_t msg, int id_)
{
	LOGGER_INFO("Ingresa nueva conexión %d", id_);
	if( uxQueueMessagesWaiting( hao->hqueue_led ) != 10 )
	{
		if(pdPASS == xQueueSend(hao->hqueue_led, (void*)&msg, 0))
		{
			if(0 == task_cnt_)
			{
				create_new_task_ao_led_(&ao_led);
			}
			LOGGER_INFO("Nueva tarea para la conexión %d", id_);
			return true;
		}
	}
	LOGGER_INFO("Conexión %d, Error: No hay más lugar en la cola", id_);
	return false;
}

void ao_led_init(ao_led_handle_t* hao)
{
	hao->hqueue_led = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while(NULL == hao->hqueue_led)
	{
		// error
	}

	vQueueAddToRegistry(hao->hqueue_led, "Queue Handle led");

}

/********************** end of file ******************************************/
