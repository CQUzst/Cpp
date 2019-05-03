/*
 * HK_include_all.h
 *
 *  Created on: 2017-09-07
 *      Author:M新动力电子  https://shop111940119.taobao.com
 */
#ifndef HK_INCLUDE_ALL_H_
#define HK_INCLUDE_ALL_H_

#define SYSCLK 150000000L

#define uint8_t unsigned char
#define uint16_t Uint16
#define uint32_t Uint32

#define u8 unsigned char
#define u16 Uint16
#define u32 Uint32

#define CLAMP(x,min,max)       {if ((x) <= (min)) (x) = (min); else if ((x) >= (max)) (x) = (max);}

#include "DSP28x_Project.h"

#include "IQmathLib.h"
#include "pi.h"                 // Include header for the PIDREG3 object
#include "speed_pr.h"           // Include header for the SPEED_MEAS_REV object
#include "HVBLDC_Sensored-Settings.h"

#include "timer0_base.h"
#include "sci_base.h"
#include "TM1650_IIC.h"
#include "ad.h"
#include "ePWM.h"
#include "Exint.h"
#include "Motor_PI.h"
#include "M_Cap.h"


//D400 低电平亮;（也是PWM7B的引脚）
#define D400GPIO GPIO8
#define D400MUX GpioCtrlRegs.GPAMUX1.bit.D400GPIO
#define D400DIR GpioCtrlRegs.GPADIR.bit.D400GPIO
#define D400TOGGLE()  GpioDataRegs.GPATOGGLE.bit.D400GPIO = 1
#define D400ON()  GpioDataRegs.GPACLEAR.bit.D400GPIO =1
#define D400LOW() GpioDataRegs.GPASET.bit.D400GPIO =1

//D401 低电平亮;
#define D401GPIO GPIO10
#define D401MUX GpioCtrlRegs.GPAMUX1.bit.D401GPIO
#define D401DIR GpioCtrlRegs.GPADIR.bit.D401GPIO
#define D401TOGGLE()  GpioDataRegs.GPATOGGLE.bit.D401GPIO = 1
#define D401ON()  GpioDataRegs.GPACLEAR.bit.D401GPIO =1
#define D401LOW() GpioDataRegs.GPASET.bit.D401GPIO =1


//独立按键S1
#define S100GPIO GPIO34
#define S100MUX GpioCtrlRegs.GPBMUX1.bit.S100GPIO
#define S100DIR GpioCtrlRegs.GPBDIR.bit.S100GPIO
#define S100PUD GpioCtrlRegs.GPBPUD.bit.S100GPIO
#define rS100DAT()  GpioDataRegs.GPBDAT.bit.S100GPIO

#endif /* HK_INCLUDE_ALL_H_ */
