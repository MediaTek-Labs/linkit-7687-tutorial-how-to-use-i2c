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

/* Modify below configuration to fit your enviornment */
#define AP_SSID "your_SSID"
#define AP_PWD  "your_pwd"

#define NTP_SERVER1 "time.stdtime.gov.tw"
#define NTP_SERVER2 "clock.stdtime.gov.tw"
#define TIMEZONE_OFFSET     8
#define CURR_CENTURY        2000
/* End of modification */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "os.h"
#include "sys_init.h"
#include "wifi_api.h"
#include "network.h"

#include "sntp.h"
#include "syslog.h"
#include "hal.h"
#include <time.h>
#include "lcd.h"

#define US2TICK(us) (us/(1000*portTICK_RATE_MS))
#define MS2TICK(ms) (ms/(portTICK_RATE_MS))

static void _timezone_shift(hal_rtc_time_t* t, int offset_hour)
{
    struct tm gt;
    struct tm *nt;
    time_t secs;
    gt.tm_year = t->rtc_year + (CURR_CENTURY-1900);
    gt.tm_mon = t->rtc_mon-1;
    gt.tm_mday = t->rtc_day;
    gt.tm_wday = t->rtc_week;
    gt.tm_hour = t->rtc_hour;
    gt.tm_min = t->rtc_min;
    gt.tm_sec = t->rtc_sec;
    secs = mktime(&gt);
    secs += offset_hour * 3600;
    nt = gmtime(&secs);
    if (!nt) {
        nt = localtime(&secs);
    }
    t->rtc_year = (nt->tm_year % 100);
    t->rtc_mon = nt->tm_mon + 1;
    t->rtc_day = nt->tm_mday;
    t->rtc_week = nt->tm_wday;
    t->rtc_hour = nt->tm_hour;
    t->rtc_min = nt->tm_min;
    t->rtc_sec = nt->tm_sec;
}

static void _sntp_check_loop(void)
{
    hal_rtc_time_t r_time;
    hal_rtc_status_t ret;

    while(1) {
        ret = hal_rtc_get_time(&r_time);
        if (ret == 0)
        {
            char buf[20];

            _timezone_shift(&r_time, TIMEZONE_OFFSET);
            LOG_I(common, "%04d/%d/%d %02d:%02d:%02d", r_time.rtc_year+CURR_CENTURY, r_time.rtc_mon, r_time.rtc_day, r_time.rtc_hour, r_time.rtc_min, r_time.rtc_sec);

            rgblcd_clear();
            snprintf(buf, 19, "%04d/%d/%d", r_time.rtc_year+CURR_CENTURY, r_time.rtc_mon, r_time.rtc_day);
            rgblcd_setCursor(0, 0);
            rgblcd_write_str(buf);
            snprintf(buf, 19, "%02d:%02d:%02d", r_time.rtc_hour, r_time.rtc_min, r_time.rtc_sec);
            rgblcd_setCursor(0, 1);
            rgblcd_write_str(buf);
        }

        // wait 1 sec and retry
        vTaskDelay(MS2TICK(1000));
    }
}

static void _i2c_write(uint8_t addr, uint8_t* data, uint8_t dataLen)
{
    hal_i2c_status_t ret;

    ret = hal_i2c_master_send_polling(HAL_I2C_MASTER_0, addr, data, dataLen);
    if(HAL_I2C_STATUS_OK != ret)
    {
        LOG_E(common, "_i2c_write(%X) FAIL: %d", addr, ret);
    }
}

static void lcd_init(void)
{
    hal_i2c_config_t i2c_init;

    hal_gpio_init(HAL_GPIO_27);
    hal_gpio_init(HAL_GPIO_28);
    hal_pinmux_set_function(HAL_GPIO_27,HAL_GPIO_27_I2C1_CLK);
    hal_pinmux_set_function(HAL_GPIO_28,HAL_GPIO_28_I2C1_DATA);
    hal_gpio_pull_up(HAL_GPIO_27);
    hal_gpio_pull_up(HAL_GPIO_28);

    i2c_init.frequency = HAL_I2C_FREQUENCY_100K;
    hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c_init);

    rgblcd_registerFunc(_i2c_write);
}

static void main_task(void *args)
{
    LOG_I(common, "LCD init");
    lcd_init();
    rgblcd_begin(16, 2);
    LOG_I(common, "LCD init done");

    rgblcd_setRGB(255, 0, 0);
    rgblcd_setCursor(0, 0);
    rgblcd_write_str("Connecting AP...");

    LOG_I(common, "network init: %s, %s", AP_SSID, AP_PWD);
    network_init(AP_SSID, AP_PWD);
    LOG_I(common, "network init done");

    rgblcd_setRGB(0, 255, 0);
    rgblcd_clear();

    //SNTP start.
    sntp_setservername(0, NTP_SERVER1);
    sntp_setservername(1, NTP_SERVER2);
    sntp_init();
    LOG_I(common, "SNTP inited");

    vTaskDelay(MS2TICK(1000));
    _sntp_check_loop();
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


