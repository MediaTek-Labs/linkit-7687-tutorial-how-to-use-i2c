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
/**
 * Includes
 */
#include "gsensor.h"

static int (*_write_func)(uint8_t addr, uint8_t* data, uint8_t dataLen);
static int  (*_read_func)(uint8_t addr, uint8_t* data, uint8_t dataLen);


void gsnr_registerFunc(
    int (*i2cWrite)(uint8_t addr, uint8_t* data, uint8_t dataLen),
    int  (*i2cRead)(uint8_t addr, uint8_t* data, uint8_t dataLen))
{
    _write_func = i2cWrite;
    _read_func  = i2cRead;
}

void gsnr_initADXL345(void)
{
    // initialize the BW data rate
    uint8_t tx[2];
    tx[0] = ADXL345_BW_RATE_REG;
    tx[1] = ADXL345_1600HZ; //value greater than or equal to 0x0A is written into the rate bits (Bit D3 through Bit D0) in the BW_RATE register 
    _write_func( ADXL345_I2C_WRITE , tx, 2);  

    //Data format (for +-16g) - This is done by setting Bit D3 of the DATA_FORMAT register (Address 0x31) and writing a value of 0x03 to the range bits (Bit D1 and Bit D0) of the DATA_FORMAT register (Address 0x31).
    uint8_t rx[2];
    rx[0] = ADXL345_DATA_FORMAT_REG;
    rx[1] = 0x0B; 
     // full res and +_16g
    _write_func( ADXL345_I2C_WRITE , rx, 2); 
 
    // Set Offset  - programmed into the OFSX, OFSY, and OFXZ registers, respectively, as 0xFD, 0x03 and 0xFE.
    uint8_t x[2];
    x[0] = ADXL345_OFSX_REG ;
    x[1] = 0xFD; 
    _write_func( ADXL345_I2C_WRITE , x, 2);
  
    uint8_t y[2];
    y[0] = ADXL345_OFSY_REG ;
    y[1] = 0x03; 
    _write_func( ADXL345_I2C_WRITE , y, 2);
    
    uint8_t z[2];
    z[0] = ADXL345_OFSZ_REG ;
    z[1] = 0xFE; 
    _write_func( ADXL345_I2C_WRITE , z, 2);
}


uint8_t gsnr_SingleByteRead(uint8_t address){   
   uint8_t tx = address;
   uint8_t output; 
    _write_func( ADXL345_I2C_WRITE , &tx, 1);  //tell it what you want to read
    _read_func( ADXL345_I2C_READ , &output, 1);    //tell it where to store the data
    return output;
  
}


/*
***info on the _write_func***
address     8-bit I2C slave address [ addr | 0 ]
data        Pointer to the byte-array data to send
length        Number of bytes to send
repeated    Repeated start, true - do not send stop at end
returns     0 on success (ack), or non-0 on failure (nack)
*/

int gsnr_SingleByteWrite(uint8_t address, uint8_t data){ 
   int ack = 0;
   uint8_t tx[2];
   tx[0] = address;
   tx[1] = data;
   return   ack | _write_func( ADXL345_I2C_WRITE , tx, 2);   
}



void gsnr_multiByteRead(uint8_t address, uint8_t* output, int size) {
    _write_func( ADXL345_I2C_WRITE, &address, 1);  //tell it where to read from
    _read_func( ADXL345_I2C_READ , output, size);      //tell it where to store the data read
}


int gsnr_multiByteWrite(uint8_t address, uint8_t* ptr_data, int size) {
        int ack;
   
               ack = _write_func( ADXL345_I2C_WRITE, &address, 1);  //tell it where to write to
        return ack | _write_func( ADXL345_I2C_READ, ptr_data, size);  //tell it what data to write
                                    
}


void gsnr_getOutput(int* readings){
    uint8_t buffer[6];    
    gsnr_multiByteRead(ADXL345_DATAX0_REG, buffer, 6);
    
    readings[0] = (int)buffer[1] << 8 | (int)buffer[0];
    readings[1] = (int)buffer[3] << 8 | (int)buffer[2];
    readings[2] = (int)buffer[5] << 8 | (int)buffer[4];

}



uint8_t gsnr_getDeviceID() {  
    return gsnr_SingleByteRead(ADXL345_DEVID_REG);
    }
//
int gsnr_setPowerMode(uint8_t mode) { 

    //Get the current register contents, so we don't clobber the rate value.
    uint8_t registerContents = (mode << 4) | gsnr_SingleByteRead(ADXL345_BW_RATE_REG);

   return gsnr_SingleByteWrite(ADXL345_BW_RATE_REG, registerContents);

}

uint8_t gsnr_getPowerControl() {    
    return gsnr_SingleByteRead(ADXL345_POWER_CTL_REG);
}

int gsnr_setPowerControl(uint8_t settings) {    
    return gsnr_SingleByteWrite(ADXL345_POWER_CTL_REG, settings);

}



uint8_t gsnr_getDataFormatControl(void){

    return gsnr_SingleByteRead(ADXL345_DATA_FORMAT_REG);
}

int gsnr_setDataFormatControl(uint8_t settings){

   return gsnr_SingleByteWrite(ADXL345_DATA_FORMAT_REG, settings);
    
}

int gsnr_setDataRate(uint8_t rate) {

    //Get the current register contents, so we don't clobber the power bit.
    uint8_t registerContents = gsnr_SingleByteRead(ADXL345_BW_RATE_REG);

    registerContents &= 0x10;
    registerContents |= rate;

    return gsnr_SingleByteWrite(ADXL345_BW_RATE_REG, registerContents);

}


uint8_t gsnr_getOffset(uint8_t axis) {     

    uint8_t address = 0;

    if (axis == ADXL345_X) {
        address = ADXL345_OFSX_REG;
    } else if (axis == ADXL345_Y) {
        address = ADXL345_OFSY_REG;
    } else if (axis == ADXL345_Z) {
        address = ADXL345_OFSZ_REG;
    }

   return gsnr_SingleByteRead(address);
}

int gsnr_setOffset(uint8_t axis, uint8_t offset) {        

    uint8_t address = 0;

    if (axis == ADXL345_X) {
        address = ADXL345_OFSX_REG;
    } else if (axis == ADXL345_Y) {
        address = ADXL345_OFSY_REG;
    } else if (axis == ADXL345_Z) {
        address = ADXL345_OFSZ_REG;
    }

   return gsnr_SingleByteWrite(address, offset);

}


uint8_t gsnr_getFifoControl(void){

    return gsnr_SingleByteRead(ADXL345_FIFO_CTL);

}

int gsnr_setFifoControl(uint8_t settings){
   return gsnr_SingleByteWrite(ADXL345_FIFO_STATUS, settings);

}

uint8_t gsnr_getFifoStatus(void){

    return gsnr_SingleByteRead(ADXL345_FIFO_STATUS);

}



uint8_t gsnr_getTapThreshold(void) {

    return gsnr_SingleByteRead(ADXL345_THRESH_TAP_REG);
}

int gsnr_setTapThreshold(uint8_t threshold) {   

   return gsnr_SingleByteWrite(ADXL345_THRESH_TAP_REG, threshold);

}


float gsnr_getTapDuration(void) {     

    return (float)gsnr_SingleByteRead(ADXL345_DUR_REG)*625;
}

int gsnr_setTapDuration(short int duration_us) {

    short int tapDuration = duration_us / 625;
    uint8_t tapChar[2];
     tapChar[0] = (tapDuration & 0x00FF);
     tapChar[1] = (tapDuration >> 8) & 0x00FF;
    return gsnr_multiByteWrite(ADXL345_DUR_REG, tapChar, 2);

}

float gsnr_getTapLatency(void) {

    return (float)gsnr_SingleByteRead(ADXL345_LATENT_REG)*1.25;
}

int gsnr_setTapLatency(short int latency_ms) {

    latency_ms = latency_ms / 1.25;
    uint8_t latChar[2];
     latChar[0] = (latency_ms & 0x00FF);
     latChar[1] = (latency_ms << 8) & 0xFF00;
    return gsnr_multiByteWrite(ADXL345_LATENT_REG, latChar, 2);

}

float gsnr_getWindowTime(void) {

    return (float)gsnr_SingleByteRead(ADXL345_WINDOW_REG)*1.25;
}

int gsnr_setWindowTime(short int window_ms) {

    window_ms = window_ms / 1.25;
    uint8_t windowChar[2];
    windowChar[0] = (window_ms & 0x00FF);
    windowChar[1] = ((window_ms << 8) & 0xFF00);
   return gsnr_multiByteWrite(ADXL345_WINDOW_REG, windowChar, 2);

}

uint8_t gsnr_getActivityThreshold(void) {

    return gsnr_SingleByteRead(ADXL345_THRESH_ACT_REG);
}

int gsnr_setActivityThreshold(uint8_t threshold) {
    return gsnr_SingleByteWrite(ADXL345_THRESH_ACT_REG, threshold);

}

uint8_t gsnr_getInactivityThreshold(void) {
    return gsnr_SingleByteRead(ADXL345_THRESH_INACT_REG);
       
}

//int FUNCTION(short int * ptr_Output)
//short int FUNCTION ()

int gsnr_setInactivityThreshold(uint8_t threshold) {
    return gsnr_SingleByteWrite(ADXL345_THRESH_INACT_REG, threshold);

}

uint8_t gsnr_getTimeInactivity(void) {

    return gsnr_SingleByteRead(ADXL345_TIME_INACT_REG);

}

int gsnr_setTimeInactivity(uint8_t timeInactivity) {
    return gsnr_SingleByteWrite(ADXL345_TIME_INACT_REG, timeInactivity);

}

uint8_t gsnr_getActivityInactivityControl(void) {

    return gsnr_SingleByteRead(ADXL345_ACT_INACT_CTL_REG);

}

int gsnr_setActivityInactivityControl(uint8_t settings) {
    return gsnr_SingleByteWrite(ADXL345_ACT_INACT_CTL_REG, settings);
    
}

uint8_t gsnr_getFreefallThreshold(void) {

    return gsnr_SingleByteRead(ADXL345_THRESH_FF_REG);

}

int gsnr_setFreefallThreshold(uint8_t threshold) {
   return gsnr_SingleByteWrite(ADXL345_THRESH_FF_REG, threshold);

}

uint8_t gsnr_getFreefallTime(void) {

    return gsnr_SingleByteRead(ADXL345_TIME_FF_REG)*5;

}

int gsnr_setFreefallTime(short int freefallTime_ms) {
     freefallTime_ms = freefallTime_ms / 5;
     uint8_t fallChar[2];
     fallChar[0] = (freefallTime_ms & 0x00FF);
     fallChar[1] = (freefallTime_ms << 8) & 0xFF00;
    
    return gsnr_multiByteWrite(ADXL345_TIME_FF_REG, fallChar, 2);

}

uint8_t gsnr_getTapAxisControl(void) {

    return gsnr_SingleByteRead(ADXL345_TAP_AXES_REG);

}

int gsnr_setTapAxisControl(uint8_t settings) {
   return gsnr_SingleByteWrite(ADXL345_TAP_AXES_REG, settings);

}

uint8_t gsnr_getTapSource(void) {

    return gsnr_SingleByteRead(ADXL345_ACT_TAP_STATUS_REG);

}



uint8_t gsnr_getInterruptEnableControl(void) {

    return gsnr_SingleByteRead(ADXL345_INT_ENABLE_REG);

}

int gsnr_setInterruptEnableControl(uint8_t settings) {
   return gsnr_SingleByteWrite(ADXL345_INT_ENABLE_REG, settings);

}

uint8_t gsnr_getInterruptMappingControl(void) {

    return gsnr_SingleByteRead(ADXL345_INT_MAP_REG);

}

int gsnr_setInterruptMappingControl(uint8_t settings) {
    return gsnr_SingleByteWrite(ADXL345_INT_MAP_REG, settings);

}

uint8_t gsnr_getInterruptSource(void){

    return gsnr_SingleByteRead(ADXL345_INT_SOURCE_REG);

}




