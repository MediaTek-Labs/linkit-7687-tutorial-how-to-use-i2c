/*
  rgblcd.c
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.
  Author:Loovee
  2013-9-18
  add rgb backlight fucnction @ 2013-10-15
  
  The MIT License (MIT)
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.1  USA
*/

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


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"

#define US2TICK(us) (us/(1000*portTICK_RATE_MS))
#define MS2TICK(ms) (ms/(portTICK_RATE_MS))


#include "lcd.h"

// Device I2C Arress
#define LCD_ADDRESS     (0x7c>>1)
#define RGB_ADDRESS     (0xc4>>1)

#define REG_RED         0x04        // pwm2
#define REG_GREEN       0x03        // pwm1
#define REG_BLUE        0x02        // pwm0

#define REG_MODE1       0x00
#define REG_MODE2       0x01
#define REG_OUTPUT      0x08

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

static uint8_t _displayfunction;
static uint8_t _displaycontrol;
static uint8_t _displaymode;

static uint8_t _initialized;

static uint8_t _numlines,_currline;

static void (*_write_func)(uint8_t addr, uint8_t* data, uint8_t dataLen);

static void _i2c_send_byte(unsigned char data)
{
    uint8_t d[1];
    d[0] = data;
    _write_func(LCD_ADDRESS, d, 1);
}

static void _i2c_send_byte2(unsigned char data1, unsigned char data2)
{
    uint8_t d[2];
    d[0] = data1;
    d[1] = data2;
    _write_func(LCD_ADDRESS, d, 2);
}

static void _setReg(unsigned char addr, unsigned char data)
{
    uint8_t d[2];
    d[0] = addr;
    d[1] = data;
    _write_func(RGB_ADDRESS, d, 2);
}

void rgblcd_registerFunc(void (*i2cWrite)(uint8_t addr, uint8_t* data, uint8_t dataLen))
{
    _write_func = i2cWrite;
}

void rgblcd_begin(uint8_t cols, uint8_t lines) 
{
    uint8_t dotsize = LCD_5x8DOTS;

    if (lines > 1) {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = lines;
    _currline = 0;

    // for some 1 line displays you can select a 10 pixel high font
    if ((dotsize != 0) && (lines == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    vTaskDelay(US2TICK(50000));


    // this is according to the hitachi HD44780 datasheet
    // page 45 figure 23

    // Send function set command sequence
    rgblcd_command(LCD_FUNCTIONSET | _displayfunction);
    vTaskDelay(US2TICK(4500));  // wait more than 4.1ms

    // second try
    rgblcd_command(LCD_FUNCTIONSET | _displayfunction);
    vTaskDelay(US2TICK(150));

    // third go
    rgblcd_command(LCD_FUNCTIONSET | _displayfunction);


    // finally, set # lines, font size, etc.
    rgblcd_command(LCD_FUNCTIONSET | _displayfunction);

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    rgblcd_display();

    // clear it off
    rgblcd_clear();

    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    rgblcd_command(LCD_ENTRYMODESET | _displaymode);

    // backlight init
    _setReg(REG_MODE1, 0);
    // set LEDs controllable by both PWM and GRPPWM registers
    _setReg(REG_OUTPUT, 0xFF);
    // set MODE2 values
    // 0010 0000 -> 0x20  (DMBLNK to 1, ie blinky mode)
    _setReg(REG_MODE2, 0x20);
    
    rgblcd_setRGB(0, 0, 0);

}

/********** high level commands, for the user! */
void rgblcd_clear()
{
    rgblcd_command(LCD_CLEARDISPLAY);        // clear display, set cursor position to zero
    vTaskDelay(US2TICK(2000));          // this command takes a long time!
}

void rgblcd_home()
{
    rgblcd_command(LCD_RETURNHOME);        // set cursor position to zero
    vTaskDelay(US2TICK(2000));        // this command takes a long time!
}

void rgblcd_setCursor(uint8_t col, uint8_t row)
{
    col = (row == 0 ? col|0x80 : col|0xc0);
    _i2c_send_byte2(0x80, col);
}

// Turn the display on/off (quickly)
void rgblcd_noDisplay()
{
    _displaycontrol &= ~LCD_DISPLAYON;
    rgblcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void rgblcd_display() {
    _displaycontrol |= LCD_DISPLAYON;
    rgblcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void rgblcd_noCursor()
{
    _displaycontrol &= ~LCD_CURSORON;
    rgblcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void rgblcd_cursor() {
    _displaycontrol |= LCD_CURSORON;
    rgblcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void rgblcd_noBlink()
{
    _displaycontrol &= ~LCD_BLINKON;
    rgblcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void rgblcd_blink()
{
    _displaycontrol |= LCD_BLINKON;
    rgblcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void rgblcd_scrollDisplayLeft(void)
{
    rgblcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void rgblcd_scrollDisplayRight(void)
{
    rgblcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void rgblcd_leftToRight(void)
{
    _displaymode |= LCD_ENTRYLEFT;
    rgblcd_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rgblcd_rightToLeft(void)
{
    _displaymode &= ~LCD_ENTRYLEFT;
    rgblcd_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void rgblcd_autoscroll(void)
{
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    rgblcd_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void rgblcd_noAutoscroll(void)
{
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    rgblcd_command(LCD_ENTRYMODESET | _displaymode);
}

// Control the backlight LED blinking
void rgblcd_blinkLED(void)
{
    // blink period in seconds = (<reg 7> + 1) / 24
    // on/off ratio = <reg 6> / 256
    _setReg(0x07, 0x17);  // blink every second
    _setReg(0x06, 0x7f);  // half on, half off
}

void rgblcd_noBlinkLED(void)
{
    _setReg(0x07, 0x00);
    _setReg(0x06, 0xff);
}

/*********** mid level commands, for sending data/cmds */

// send command
void rgblcd_command(uint8_t value)
{
    _i2c_send_byte2(0x80, value);
}

// send data
void rgblcd_write(uint8_t value)
{
    _i2c_send_byte2(0x40, value);
}

void rgblcd_write_str(const char *value)
{
    while(*value)
    {
        _i2c_send_byte2(0x40, *value);
        value++;
    }
}

void rgblcd_setRGB(unsigned char r, unsigned char g, unsigned char b)
{
    _setReg(REG_RED, r);
    _setReg(REG_GREEN, g);
    _setReg(REG_BLUE, b);
}

