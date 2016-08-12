/**
 * @author Peter Swanson
 * A personal note from me: Jesus Christ has changed my life so much it blows my mind. I say this because
 *                  today, religion is thought of as something that you do or believe and has about as
 *                  little impact on a person as their political stance. But for me, God gives me daily
 *                  strength and has filled my life with the satisfaction that I could never find in any
 *                  of the other things that I once looked for it in. 
 * If your interested, heres verse that changed my life:
 *      Rom 8:1-3: "Therefore, there is now no condemnation for those who are in Christ Jesus,
 *                  because through Christ Jesus, the law of the Spirit who gives life has set
 *                  me free from the law of sin (which brings...) and death. For what the law 
 *                  was powerless to do in that it was weakened by the flesh, God did by sending
 *                  His own Son in the likeness of sinful flesh to be a sin offering. And so He
 *                  condemned sin in the flesh in order that the righteous requirements of the 
 *                  (God's) law might be fully met in us, who live not according to the flesh
 *                  but according to the Spirit."
 *
 *  A special thanks to Ewout van Bekkum for all his patient help in developing this library!
 *
 * @section LICENSE
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * ADXL345, triple axis, I2C interface, accelerometer.
 *
 * Datasheet:
 *
 * http://www.analog.com/static/imported-files/data_sheets/ADXL345.pdf
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


#ifndef ADXL345_I2C_H
#define ADXL345_I2C_H

/**
 * Includes
 */
#include "FreeRTOS.h"

/**
 * Defines
 */
//Registers.
#define ADXL345_DEVID_REG          0x00
#define ADXL345_THRESH_TAP_REG     0x1D
#define ADXL345_OFSX_REG           0x1E
#define ADXL345_OFSY_REG           0x1F
#define ADXL345_OFSZ_REG           0x20
#define ADXL345_DUR_REG            0x21
#define ADXL345_LATENT_REG         0x22
#define ADXL345_WINDOW_REG         0x23
#define ADXL345_THRESH_ACT_REG     0x24
#define ADXL345_THRESH_INACT_REG   0x25
#define ADXL345_TIME_INACT_REG     0x26
#define ADXL345_ACT_INACT_CTL_REG  0x27
#define ADXL345_THRESH_FF_REG      0x28
#define ADXL345_TIME_FF_REG        0x29
#define ADXL345_TAP_AXES_REG       0x2A
#define ADXL345_ACT_TAP_STATUS_REG 0x2B
#define ADXL345_BW_RATE_REG        0x2C
#define ADXL345_POWER_CTL_REG      0x2D
#define ADXL345_INT_ENABLE_REG     0x2E
#define ADXL345_INT_MAP_REG        0x2F
#define ADXL345_INT_SOURCE_REG     0x30
#define ADXL345_DATA_FORMAT_REG    0x31
#define ADXL345_DATAX0_REG         0x32
#define ADXL345_DATAX1_REG         0x33
#define ADXL345_DATAY0_REG         0x34
#define ADXL345_DATAY1_REG         0x35
#define ADXL345_DATAZ0_REG         0x36
#define ADXL345_DATAZ1_REG         0x37
#define ADXL345_FIFO_CTL           0x38
#define ADXL345_FIFO_STATUS        0x39

//Data rate codes.
#define ADXL345_3200HZ      0x0F
#define ADXL345_1600HZ      0x0E
#define ADXL345_800HZ       0x0D
#define ADXL345_400HZ       0x0C
#define ADXL345_200HZ       0x0B
#define ADXL345_100HZ       0x0A
#define ADXL345_50HZ        0x09
#define ADXL345_25HZ        0x08
#define ADXL345_12HZ5       0x07
#define ADXL345_6HZ25       0x06

// read or write bytes
#define ADXL345_I2C_READ    0xA7  
#define ADXL345_I2C_WRITE   0xA6 
#define ADXL345_I2C_ADDRESS 0x53   //the ADXL345 7-bit address is 0x53 when ALT ADDRESS is low as it is on the sparkfun chip: when ALT ADDRESS is high the address is 0x1D

/////////////when ALT ADDRESS pin is high:
//#define ADXL345_I2C_READ    0x3B   
//#define ADXL345_I2C_WRITE   0x3A
//#define ADXL345_I2C_ADDRESS 0x1D 

#define ADXL345_X           0x00
#define ADXL345_Y           0x01
#define ADXL345_Z           0x02



// modes
#define MEASUREMENT_MODE    0x08


void gsnr_initADXL345(void);

void gsnr_registerFunc(
    int (*i2cWrite)(uint8_t addr, uint8_t* data, uint8_t dataLen),
    int  (*i2cRead)(uint8_t addr, uint8_t* data, uint8_t dataLen));

/**
 * Get the output of all three axes.
 *
 * @param Pointer to a buffer to hold the accelerometer value for the
 *        x-axis, y-axis and z-axis [in that order].
 */
void gsnr_getOutput(int* readings);

/**
 * Read the device ID register on the device.
 *
 * @return The device ID code [0xE5]
 */
uint8_t gsnr_getDeviceID(void);



/**
 * Set the power mode.
 *
 * @param mode 0 -> Normal operation.
 *             1 -> Reduced power operation.
 */     
int gsnr_setPowerMode(uint8_t mode);

/**
 * Set the power control settings.
 *
 * See datasheet for details.
 *
 * @param The control byte to write to the POWER_CTL register.
 */
int gsnr_setPowerControl(uint8_t settings);     

/**
 * Get the power control settings.
 *
 * See datasheet for details.
 *
 * @return The contents of the POWER_CTL register.
 */
uint8_t gsnr_getPowerControl(void);

   
/**
 * Get the data format settings.
 *
 * @return The contents of the DATA_FORMAT register.
 */
 
uint8_t gsnr_getDataFormatControl(void);

/**
 * Set the data format settings.
 *
 * @param settings The control byte to write to the DATA_FORMAT register.
 */
int gsnr_setDataFormatControl(uint8_t settings);

/**
 * Set the data rate.
 *
 * @param rate The rate code (see #defines or datasheet).
 */
int gsnr_setDataRate(uint8_t rate);


/**
 * Get the current offset for a particular axis.
 *
 * @param axis 0x00 -> X-axis
 *             0x01 -> Y-axis
 *             0x02 -> Z-axis
 * @return The current offset as an 8-bit 2's complement number with scale
 *         factor 15.6mg/LSB.
 */
uint8_t gsnr_getOffset(uint8_t axis);

/**
 * Set the offset for a particular axis.
 *
 * @param axis 0x00 -> X-axis
 *             0x01 -> Y-axis
 *             0x02 -> Z-axis
 * @param offset The offset as an 8-bit 2's complement number with scale
 *               factor 15.6mg/LSB.
 */
int gsnr_setOffset(uint8_t axis, uint8_t offset);



/**
 * Get the FIFO control settings.
 *
 * @return The contents of the FIFO_CTL register.
 */
uint8_t gsnr_getFifoControl(void);

/**
 * Set the FIFO control settings.
 *
 * @param The control byte to write to the FIFO_CTL register.
 */
int gsnr_setFifoControl(uint8_t settings);

/**
 * Get FIFO status.
 *
 * @return The contents of the FIFO_STATUS register.
 */
uint8_t gsnr_getFifoStatus(void);

/**
 * Read the tap threshold on the device.
 *
 * @return The tap threshold as an 8-bit number with a scale factor of
 *         62.5mg/LSB.
 */
uint8_t gsnr_getTapThreshold(void);

/**
 * Set the tap threshold.
 *
 * @param The tap threshold as an 8-bit number with a scale factor of
 *        62.5mg/LSB.
 */
int gsnr_setTapThreshold(uint8_t threshold);

/**
 * Get the tap duration required to trigger an event.
 *
 * @return The max time that an event must be above the tap threshold to
 *         qualify as a tap event, in microseconds.
 */
float gsnr_getTapDuration(void);

/**
 * Set the tap duration required to trigger an event.
 *
 * @param duration_us The max time that an event must be above the tap
 *                    threshold to qualify as a tap event, in microseconds.
 *                    Time will be normalized by the scale factor which is
 *                    625us/LSB. A value of 0 disables the single/double
 *                    tap functions.
 */
int gsnr_setTapDuration(short int duration_us);

/**
 * Get the tap latency between the detection of a tap and the time window.
 *
 * @return The wait time from the detection of a tap event to the start of
 *         the time window during which a possible second tap event can be
 *         detected in milliseconds.
 */
float gsnr_getTapLatency(void);

/**
 * Set the tap latency between the detection of a tap and the time window.
 *
 * @param latency_ms The wait time from the detection of a tap event to the
 *                   start of the time window during which a possible
 *                   second tap event can be detected in milliseconds.
 *                   A value of 0 disables the double tap function.
 */
int gsnr_setTapLatency(short int latency_ms);

/**
 * Get the time of window between tap latency and a double tap.
 *
 * @return The amount of time after the expiration of the latency time
 *         during which a second valid tap can begin, in milliseconds.
 */
float gsnr_getWindowTime(void);

/**
 * Set the time of the window between tap latency and a double tap.
 *
 * @param window_ms The amount of time after the expiration of the latency
 *                  time during which a second valid tap can begin,
 *                  in milliseconds.
 */
int gsnr_setWindowTime(short int window_ms);

/**
 * Get the threshold value for detecting activity.
 *
 * @return The threshold value for detecting activity as an 8-bit number.
 *         Scale factor is 62.5mg/LSB.
 */
uint8_t gsnr_getActivityThreshold(void);

/**
 * Set the threshold value for detecting activity.
 *
 * @param threshold The threshold value for detecting activity as an 8-bit
 *                  number. Scale factor is 62.5mg/LSB. A value of 0 may
 *                  result in undesirable behavior if the activity
 *                  interrupt is enabled.
 */
int gsnr_setActivityThreshold(uint8_t threshold);

/**
 * Get the threshold value for detecting inactivity.
 *
 * @return The threshold value for detecting inactivity as an 8-bit number.
 *         Scale factor is 62.5mg/LSB.
 */
uint8_t gsnr_getInactivityThreshold(void);

/**
 * Set the threshold value for detecting inactivity.
 *
 * @param threshold The threshold value for detecting inactivity as an
 *                  8-bit number. Scale factor is 62.5mg/LSB.
 */
int gsnr_setInactivityThreshold(uint8_t threshold);

/**
 * Get the time required for inactivity to be declared.
 *
 * @return The amount of time that acceleration must be less than the
 *         inactivity threshold for inactivity to be declared, in
 *         seconds.
 */
uint8_t gsnr_getTimeInactivity(void);

/**
 * Set the time required for inactivity to be declared.
 *
 * @param inactivity The amount of time that acceleration must be less than
 *                   the inactivity threshold for inactivity to be
 *                   declared, in seconds. A value of 0 results in an
 *                   interrupt when the output data is less than the
 *                   threshold inactivity.
 */
int gsnr_setTimeInactivity(uint8_t timeInactivity);

/**
 * Get the activity/inactivity control settings.
 *
 *      D7            D6             D5            D4
 * +-----------+--------------+--------------+--------------+
 * | ACT ac/dc | ACT_X enable | ACT_Y enable | ACT_Z enable |
 * +-----------+--------------+--------------+--------------+
 *
 *        D3             D2               D1              D0
 * +-------------+----------------+----------------+----------------+
 * | INACT ac/dc | INACT_X enable | INACT_Y enable | INACT_Z enable |
 * +-------------+----------------+----------------+----------------+
 *
 * See datasheet for details.
 *
 * @return The contents of the ACT_INACT_CTL register.
 */
uint8_t gsnr_getActivityInactivityControl(void);

/**
 * Set the activity/inactivity control settings.
 *
 *      D7            D6             D5            D4
 * +-----------+--------------+--------------+--------------+
 * | ACT ac/dc | ACT_X enable | ACT_Y enable | ACT_Z enable |
 * +-----------+--------------+--------------+--------------+
 *
 *        D3             D2               D1              D0
 * +-------------+----------------+----------------+----------------+
 * | INACT ac/dc | INACT_X enable | INACT_Y enable | INACT_Z enable |
 * +-------------+----------------+----------------+----------------+
 *
 * See datasheet for details.
 *
 * @param settings The control byte to write to the ACT_INACT_CTL register.
 */
int gsnr_setActivityInactivityControl(uint8_t settings);

/**
 * Get the threshold for free fall detection.
 *
 * @return The threshold value for free-fall detection, as an 8-bit number,
 *         with scale factor 62.5mg/LSB.
 */
uint8_t gsnr_getFreefallThreshold(void);

/**
 * Set the threshold for free fall detection.
 *
 * @return The threshold value for free-fall detection, as an 8-bit number,
 *         with scale factor 62.5mg/LSB. A value of 0 may result in 
 *         undesirable behavior if the free-fall interrupt is enabled.
 *         Values between 300 mg and 600 mg (0x05 to 0x09) are recommended.
 */
int gsnr_setFreefallThreshold(uint8_t threshold);

/**
 * Get the time required to generate a free fall interrupt.
 *
 * @return The minimum time that the value of all axes must be less than
 *         the freefall threshold to generate a free-fall interrupt, in
 *         milliseconds.
 */
uint8_t gsnr_getFreefallTime(void);

/**
 * Set the time required to generate a free fall interrupt.
 *
 * @return The minimum time that the value of all axes must be less than
 *         the freefall threshold to generate a free-fall interrupt, in
 *         milliseconds. A value of 0 may result in undesirable behavior
 *         if the free-fall interrupt is enabled. Values between 100 ms 
 *         and 350 ms (0x14 to 0x46) are recommended.
 */
int gsnr_setFreefallTime(short int freefallTime_ms);

/**
 * Get the axis tap settings.
 *
 *      D3           D2            D1             D0
 * +----------+--------------+--------------+--------------+
 * | Suppress | TAP_X enable | TAP_Y enable | TAP_Z enable |
 * +----------+--------------+--------------+--------------+
 *
 * (D7-D4 are 0s).
 *
 * See datasheet for more details.
 *
 * @return The contents of the TAP_AXES register.
 */ 
uint8_t gsnr_getTapAxisControl(void);

/**
 * Set the axis tap settings.
 *
 *      D3           D2            D1             D0
 * +----------+--------------+--------------+--------------+
 * | Suppress | TAP_X enable | TAP_Y enable | TAP_Z enable |
 * +----------+--------------+--------------+--------------+
 *
 * (D7-D4 are 0s).
 *
 * See datasheet for more details.
 *
 * @param The control byte to write to the TAP_AXES register.
 */
int gsnr_setTapAxisControl(uint8_t settings);

/**
 * Get the source of a tap.
 *
 * @return The contents of the ACT_TAP_STATUS register.
 */
uint8_t gsnr_getTapSource(void);

 /**
 * Get the interrupt enable settings.
 *
 * @return The contents of the INT_ENABLE register.
 */
uint8_t gsnr_getInterruptEnableControl(void);

/**
 * Set the interrupt enable settings.
 *
 * @param settings The control byte to write to the INT_ENABLE register.
 */
int gsnr_setInterruptEnableControl(uint8_t settings);

/**
 * Get the interrupt mapping settings.
 *
 * @return The contents of the INT_MAP register.
 */
uint8_t gsnr_getInterruptMappingControl(void);

/**
 * Set the interrupt mapping settings.
 *
 * @param settings The control byte to write to the INT_MAP register.
 */
int gsnr_setInterruptMappingControl(uint8_t settings);

/**
 * Get the interrupt source.
 *
 * @return The contents of the INT_SOURCE register.
 */
uint8_t gsnr_getInterruptSource(void);


/**
 * Read one byte from a register on the device.
 *
 * @param: - the address to be read from
 *
 * @return: the value of the data read
 */
uint8_t gsnr_SingleByteRead(uint8_t address);

/**
 * Write one byte to a register on the device.
 *
 * @param:
    - address of the register to write to.
    - the value of the data to store
 */
int gsnr_SingleByteWrite(uint8_t address, uint8_t data);

/**
 * Read several consecutive bytes on the device and store them in a given location.
 *
 * @param startAddress: The address of the first register to read from.
 * @param ptr_output: a pointer to the location to store the data being read
 * @param size: The number of bytes to read.
 */
void gsnr_multiByteRead(uint8_t startAddress, uint8_t* ptr_output, int size);

/**
 * Write several consecutive bytes  on the device.
 *
 * @param startAddress: The address of the first register to write to.
 * @param ptr_data: Pointer to a location which contains the data to write.
 * @param size: The number of bytes to write.
 */
int gsnr_multiByteWrite(uint8_t startAddress, uint8_t* ptr_data, int size);


#endif /* ADXL345_I2C_H */
