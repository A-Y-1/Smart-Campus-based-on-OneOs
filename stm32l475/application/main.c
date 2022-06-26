/*
 ***********************************************************************************************************************
 * @file        main.c
 *
 * @brief       User application entry
 ***********************************************************************************************************************
 */

#include <board.h>
#include <onenet_mqtts.h>
#include <onenet_process.h>
int main(void)
{
	onenet_mqtts_device_start();
	onenet_upload_cycle_start();

  return 0;
}
