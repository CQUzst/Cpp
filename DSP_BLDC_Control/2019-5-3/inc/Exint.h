/*
 * Exint.h
 *
 *  Created on: 2017-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */
#ifndef __MOTOR_EXINT_H
#define __MOTOR_EXINT_H

//Hall Port
#define MOTORIPM

#define HALLANUM 14
#define HALLAMUX GpioCtrlRegs.GPAMUX1.bit.GPIO14
#define HALLADIR GpioCtrlRegs.GPADIR.bit.GPIO14
#define HALLAQSEL GpioCtrlRegs.GPAQSEL1.bit.GPIO14
#define rHADAT()  GpioDataRegs.GPADAT.bit.GPIO14

#define HALLBNUM 15
#define HALLBMUX GpioCtrlRegs.GPAMUX1.bit.GPIO15
#define HALLBDIR GpioCtrlRegs.GPADIR.bit.GPIO15
#define HALLBQSEL GpioCtrlRegs.GPAQSEL1.bit.GPIO15
#define rHBDAT()  GpioDataRegs.GPADAT.bit.GPIO15

#define HALLCNUM 48
#define HALLCMUX GpioCtrlRegs.GPBMUX2.bit.GPIO48
#define HALLCDIR GpioCtrlRegs.GPBDIR.bit.GPIO48
#define HALLCQSEL GpioCtrlRegs.GPBQSEL2.bit.GPIO48
#define rHCDAT()  GpioDataRegs.GPBDAT.bit.GPIO48

//电机状态
typedef enum
{
  M_CW    = 0,//顺时针转动    反转
  M_CCW   = 1//逆时针转动   正转
} MOTOR_DIR;

typedef struct
{
	Uint16 HallState;
	Uint16 HallStatepPevent;
	Uint16 HallType;

//霍尔HC中断计数， 2极对电机，一圈2个脉冲即为4个边缘计数
//为了保证测速的精确度，一圈仅对同一个霍尔元件的信号进行测试判断，避免多级对电机安装时的机械偏差造成测速不精确
//具体代码看测速换算部分
	Uint16 HallCNT;
}DEF_HALL;

extern DEF_HALL HallEncode;

void ExInt_Init(void);

#endif //__MOTOR_EXINT_H

