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

#include "task_ui.h"
#include "task_led.h"

/********************** macros and definitions *******************************/
#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(button_type_t))
/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern ao_btn_handle_t ao_btn;
extern ao_led_handle_t ao_led;
/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void task_ui(void *argument)
{
	static int id_ = 0;
	ao_btn_handle_t* hao = (ao_btn_handle_t*)argument;
	while (true)
	{
		button_type_t msg;
		led_color_t led_color;

		while(pdPASS == xQueueReceive(hao->hqueue_btn, &msg, 0))
		{
			switch (msg) {
			case BUTTON_TYPE_PULSE:
				LOGGER_INFO("ui led activate");
				led_color = LED_COLOR_RED;
				if(oa_led_send_msg(&ao_led, led_color, id_))
				{
					LOGGER_INFO("Conexion %d recibida", id_);
				}
				else
				{
					LOGGER_INFO("Conexion %d rechazada", id_);
				}
				id_++;
				break;
			case BUTTON_TYPE_SHORT:
				LOGGER_INFO("ui led activate");
				led_color = LED_COLOR_GREEN;
				if(oa_led_send_msg(&ao_led, led_color, id_))
				{
					LOGGER_INFO("Conexion %d recibida", id_);
				}
				else
				{
					LOGGER_INFO("Conexion %d rechazada", id_);
				}
				id_++;
				break;
			case BUTTON_TYPE_LONG:
				LOGGER_INFO("ui led activate");
				led_color = LED_COLOR_BLUE;
				if(oa_led_send_msg(&ao_led, led_color, id_))
				{
					LOGGER_INFO("Conexion %d recibida", id_);
				}
				else
				{
					LOGGER_INFO("Conexion %d rechazada", id_);
				}
				id_++;
				break;
			default:
				LOGGER_INFO("task_ui error");
				break;
			}

		}
	}
}

bool oa_ui_send_msg(ao_btn_handle_t* hao, button_type_t msg)
{
	return (pdPASS == xQueueSend(hao->hqueue_btn, (void*)&msg, 0));
}

void ao_btn_init(ao_btn_handle_t* hao)
{
	hao->hqueue_btn = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while(NULL == hao->hqueue_btn)
	{
		// error
	}

	vQueueAddToRegistry(hao->hqueue_btn, "Queue Handle btn");

	BaseType_t status;
	status = xTaskCreate(task_ui, "task_ui", 128, (void* const)hao, tskIDLE_PRIORITY, NULL);
	while (pdPASS != status)
	{
		// error
	}
}

/********************** end of file ******************************************/
