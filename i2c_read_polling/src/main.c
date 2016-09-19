/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "os.h"
#include "sys_init.h"

#include "syslog.h"
#include "hal.h"
#include <time.h>
#include "gsensor.h"

#define MS2TICK(ms) ((ms)/(portTICK_RATE_MS))

static void _gsensor_check_loop(void)
{
    while(1) {
        int output[3];

        gsnr_getOutput(output);

        LOG_I(common, "(Gx, Gy, Gz) = (%i, %i, %i)\n", output[0], output[1], output[2]);

        // wait 0.333 sec and retry
        vTaskDelay(MS2TICK(333));
    }
}

static int _i2c_write(uint8_t addr, uint8_t* data, uint8_t dataLen)
{
    hal_i2c_status_t ret;

    ret = hal_i2c_master_send_polling(HAL_I2C_MASTER_0, ADXL345_I2C_ADDRESS, data, dataLen);
    if(HAL_I2C_STATUS_OK != ret)
    {
        LOG_E(common, "_i2c_write(%X) FAIL: %d", addr, ret);

        return 1;
    }

    return 0;
}

static int _i2c_read(uint8_t addr, uint8_t* data, uint8_t dataLen)
{
    hal_i2c_status_t ret;

    ret = hal_i2c_master_receive_polling(HAL_I2C_MASTER_0, ADXL345_I2C_ADDRESS, data, dataLen);
    if(HAL_I2C_STATUS_OK != ret)
    {
        LOG_E(common, "_i2c_read(%X) FAIL: %d", addr, ret);

        return 1;
    }

    return 0;
}

static void gsensor_init(void)
{
    hal_i2c_config_t i2c_init;

    // pinmux initialization
    hal_gpio_init(HAL_GPIO_27);
    hal_gpio_init(HAL_GPIO_28);
    hal_pinmux_set_function(HAL_GPIO_27,HAL_GPIO_27_I2C1_CLK);
    hal_pinmux_set_function(HAL_GPIO_28,HAL_GPIO_28_I2C1_DATA);
    hal_gpio_pull_up(HAL_GPIO_27);
    hal_gpio_pull_up(HAL_GPIO_28);

    // setup I2C
    i2c_init.frequency = HAL_I2C_FREQUENCY_100K;
    hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c_init);

    // init ADXL345
    gsnr_registerFunc(_i2c_write, _i2c_read);
    gsnr_initADXL345();

    if (gsnr_setPowerControl(0x00))
    {
        LOG_E(common, "power control init failed.\n");
    }

    if (gsnr_setDataFormatControl(0x0B))
    {
        LOG_E(common, "set format failed.\n");
    }

    if (gsnr_setDataRate(ADXL345_3200HZ))
    {
        LOG_E(common, "set rate failed.\n");
    }

    if (gsnr_setPowerControl(MEASUREMENT_MODE))
    {
        LOG_E(common, "set measurement mode failed.\n");
    }
}

static void main_task(void *args)
{
    LOG_I(common, "Accelerometer init");
    gsensor_init();
    LOG_I(common, "Accelerometer init done");

    vTaskDelay(MS2TICK(1000));
    _gsensor_check_loop();
}

int main(void)
{
    log_config_print_level(hal, PRINT_LEVEL_WARNING);

    system_init();

    xTaskCreate(main_task, "main task", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}


