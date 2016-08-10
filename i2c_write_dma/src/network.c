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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "os.h"
#include "semphr.h"
#include "wifi_api.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "portmacro.h"

static SemaphoreHandle_t ip_ready;

static void _ip_ready_callback(struct netif *netif)
{
    if (!ip4_addr_isany_val(netif->ip_addr)) 
    {
        if (NULL != inet_ntoa(netif->ip_addr)) 
        {
            char ip_addr[17] = {0};
            strcpy(ip_addr, inet_ntoa(netif->ip_addr));
            LOG_I(common, "************************");
            LOG_I(common, "DHCP got IP: %s", ip_addr);
            LOG_I(common, "************************");
        } else {
            LOG_E(common, "DHCP got IP failed");
        }
    }
    LOG_I(common, "ip ready");
    xSemaphoreGive(ip_ready);
}

static int32_t _wifi_event_handler(wifi_event_t event,
        uint8_t *payload,
        uint32_t length)
{
    struct netif *sta_if;

    LOG_I(common, "wifi event: %d", event);

    switch(event)
    {
    case WIFI_EVENT_IOT_PORT_SECURE:
        sta_if = netif_find_by_type(NETIF_TYPE_STA);
        netif_set_link_up(sta_if);
        LOG_I(common, "wifi connected");
        break;
    case WIFI_EVENT_IOT_DISCONNECTED:
        sta_if = netif_find_by_type(NETIF_TYPE_STA);
        netif_set_link_down(sta_if);
        LOG_I(common, "wifi disconnected");
        break;
    }

    return 1;
}

void network_init(const char* SSID, const char* password)
{
    struct netif *sta_if;

    wifi_config_t wifi_config = {0};
    lwip_tcpip_config_t tcpip_config = {{0}, {0}, {0}, {0}, {0}, {0}};

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_INIT_COMPLETE , _wifi_event_handler);
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_CONNECTED, _wifi_event_handler);
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_PORT_SECURE, _wifi_event_handler);
    wifi_connection_register_event_handler(WIFI_EVENT_IOT_DISCONNECTED, _wifi_event_handler);

    wifi_config.opmode = WIFI_MODE_STA_ONLY;
    strcpy((char *)wifi_config.sta_config.ssid, SSID);
    wifi_config.sta_config.ssid_length = strlen(SSID);
    strcpy((char *)wifi_config.sta_config.password, password);
    wifi_config.sta_config.password_length = strlen(password);

    wifi_init(&wifi_config, NULL);
    lwip_tcpip_init(&tcpip_config, WIFI_MODE_STA_ONLY);

    ip_ready = xSemaphoreCreateBinary();

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_status_callback(sta_if, _ip_ready_callback);
    dhcp_start(sta_if);

    // wait for result
    xSemaphoreTake(ip_ready, portMAX_DELAY);
}

