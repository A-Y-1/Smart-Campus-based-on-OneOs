/**
 ***********************************************************************************************************************
 * Copyright (c) 2020, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        cmcc_onenet_process.c
 *
 * @brief       Demonstrate a sample which publish messages into message queue periodic.
 *
 * @details
 *
 * @revision
 * Date         Author          Notes
 * 2020-06-08   OneOs Team      First Version
 ***********************************************************************************************************************
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <os_types.h>
#include <os_mq.h>
#include <os_errno.h>
#include <os_task.h>
#include <os_assert.h>
#include <oneos_config.h>
#include <device.h>
#include <sensor.h>
#include "onenet_mqtts.h"
#include "onenet_process.h"

#define DBG_TAG "onenet_process"
#include <dlog.h>
#define ONENET_UPLOAD_DATA_TASK_STACK_SIZE 4096

os_task_t          *onenet_upload_data_task = NULL;
extern struct os_mq mqtts_mq;
int                 onenet_upoad_flag = 0;
const char *base_dp_upload_str = "{"
                                 "\"id\": %d,"
                                 "\"dp\": {"
                                 "\"Temperature\": [{"
                                 "\"v\": %.2f,"
                                 "}],"
                                 "\"light\": [{"
                                 "\"v\": %d,"
                                 "}],"	
                                 "\"Humidity\": [{"
                                 "\"v\": %.2f"
                                 "}]"	
                                 "}"
                                 "}";

static void onenet_upload_data_task_func(void *arg)
{
    os_err_t           rc;
		double *argv = (double*)arg;
    char     pub_buf[PUB_DATA_BUFF_LEN] = {0};
    char    *pub_msg      = NULL;
    int      pub_msg_len  = 0;
    mq_msg_t mq_msg;
    int      id                = 0;
    struct os_sensor_data light_data, humi_data, temp_data;
		float humi_value,temp_value;
		int light_value;
		/* 开启设备 */
    os_device_t *sensor_temp = os_device_open_s("temp_aht10");
		os_device_t *sensor_humi = os_device_open_s("humi_aht10");
		os_device_t *sensor_light = os_device_open_s("li_ap3216c");
    OS_ASSERT(sensor_temp != NULL);
		OS_ASSERT(sensor_humi != NULL);
		OS_ASSERT(sensor_light != NULL);

    while (1)
    {
        if (onenet_upoad_flag == 1)
        {
            onenet_upoad_flag = 0;
            LOG_I(DBG_TAG, "stop mqtts_device_messagequeue_send function");
            os_task_destroy(onenet_upload_data_task);
        }
        else
        {
            if (id != 2147483647)
            {
                id++;
            }
            else
            {
                id = 1;
            }
            os_device_read_nonblock(sensor_light, 0, &light_data, sizeof(struct os_sensor_data));
						os_device_read_nonblock(sensor_temp, 0, &temp_data, sizeof(struct os_sensor_data));
						os_device_read_nonblock(sensor_humi, 0, &humi_data, sizeof(struct os_sensor_data));
						light_value = light_data.data.light/1000.0;
						temp_value = temp_data.data.temp/1000.0-(double)(id/1147483647);
						humi_value = humi_data.data.humi/1000.0;
						//空调控制
						if(temp_value > 27.0)
							infrared_send_ac();
						//数据上传
						snprintf(pub_buf, sizeof(pub_buf), base_dp_upload_str, id, temp_value , light_value, humi_value);

            pub_msg     = pub_buf;
            pub_msg_len = strlen(pub_msg);

            memset(&mq_msg, 0x00, sizeof(mq_msg));
            mq_msg.topic_type = DATA_POINT_TOPIC;
            memcpy(mq_msg.data_buf, pub_msg, pub_msg_len);
            mq_msg.data_len = pub_msg_len;

            rc = os_mq_send(&mqtts_mq, (void *)&mq_msg, sizeof(mq_msg_t), 0);
            if (rc != OS_EOK)
            {
                LOG_E(DBG_TAG, "mqtts_device_messagequeue_send ERR");
            }

            os_task_msleep(5 * 1000);
        }
    }
}

void onenet_upload_cycle_start(void)
{
    onenet_upload_data_task = os_task_create("generate_pubdata",
                                             onenet_upload_data_task_func,
                                             OS_NULL,
                                             ONENET_UPLOAD_DATA_TASK_STACK_SIZE,
                                             OS_TASK_PRIORITY_MAX / 2);

    if (NULL == onenet_upload_data_task)
    {
        LOG_E(DBG_TAG, "onenet mqtts client create task failed");
        OS_ASSERT(OS_NULL != onenet_upload_data_task);
    }

    os_task_startup(onenet_upload_data_task);
}

static void onenet_upload_cycle_stop(void)
{
    onenet_upoad_flag = 1;
}

#ifdef OS_USING_SHELL
#include <shell.h>
SH_CMD_EXPORT(onenet_upload_cycle_start, onenet_upload_cycle_start, "publish message cycle to onenet specified topic");

SH_CMD_EXPORT(onenet_upload_cycle_stop,
              onenet_upload_cycle_stop,
              "stop publishing message cycle to onenet specified topic");
#endif
