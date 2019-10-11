/*
 *  实现一组互补对称的pwm波形
 *
 *  Created on: 2019年10月11日
 *      Author: 朱世涛
 */
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
void InitePWM1Gpio(void);
void ePWMSetup(void);
void main()
{
	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();   //步骤1。初始化控制系统
	// Step 3. Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	DINT;    //关闭总中断
	// Initialize the PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	// This function is found in the DSP2833x_PieCtrl.c file.
	InitPieCtrl();   //初始化PIE

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
	// This function is found in DSP2833x_PieVect.c.
	InitPieVectTable();    //中断向量表

	ePWMSetup();

	for(;;);                //相当于while(1);

}

void InitePWM1Gpio(void)
{

	EALLOW;
	//gpio上拉，引脚在悬空时，的信号可能会时高时低，上拉后给它一个高电平
	GpioCtrlRegs.GPAPUD.bit.GPIO0=0;
	GpioCtrlRegs.GPAPUD.bit.GPIO1=0;
	//gpio复用选择为pwm模式
	GpioCtrlRegs.GPAMUX1.bit.GPIO0=1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO1=1;

	EDIS;

}

void ePWMSetup(void)
{
	EALLOW;

	InitePWM1Gpio();      //配置为ePWM端口
	EPwm1Regs.TBPRD=1200;        //时基控制寄存器    设定PWM周期为2×600个TBCLK时钟周期
	EPwm1Regs.CMPA.half.CMPA=600;  //比较器A设置为600个TBCLK           TBCLK是时基时钟
	EPwm1Regs.CMPB=600;            //比较器B设置为600个TBCLK，0也行，反正不用这个值
	EPwm1Regs.TBSTS.all=0;              //时基状态寄存器清零
	EPwm1Regs.TBPHS.half.TBPHS = 0;           //时基相位寄存器清零
	EPwm1Regs.TBCTR = 0;                    //时基计数寄存器清零
	EPwm1Regs.CMPCTL.all=0x50;      //计数比较控制寄存器       计数比较A和B寄存器操作模式为立即转载模式，不需要影子寄存器
	EPwm1Regs.TBCTL.all=0x003A;     //时基控制寄存器        设置为:时基时钟分频位为1，高速时基时钟分频位为1，相当于不分频。
	//禁止同步信号输出，禁止使用影子寄存器，禁止计数寄存器装载相位寄存器，计数模式为上下计数。

	EPwm1Regs.AQCTLA.bit.CAU=0x2;   //当向上计数时，时基计数器的值与CMPA寄存器的值相等时使ePWMA输出高；
	EPwm1Regs.AQCTLA.bit.CAD=0x1;   //当向下计数时，时基计数器的值与CMPA寄存器的值相等时使ePWMA输出低；
	EPwm1Regs.AQCTLB.bit.CBU=0x1;   //当向上计数时，时基计数器的值与CMPB寄存器的值相等时使ePWMB输出低；
	EPwm1Regs.AQCTLB.bit.CBD=0x2;   //当向上计数时，时基计数器的值与CMPB寄存器的值相等时使ePWMB输出高；

	EPwm1Regs.DBCTL.all=0x2;        //死区控制寄存器  配置为：ePWMxA是双边沿延时输入源。ePWMA和ePWMB都不翻转。禁止下降沿延时，使能上升沿延时；
	EPwm1Regs.DBRED=100;            //死区上升沿延时寄存器    RED=DBRED×T（TBCLK） 相当于延时100个TBCLK时钟周期
	EPwm1Regs.DBFED=0;              //死区下降沿延时寄存器    FED=DBFED×T（TBCLK） TBCLK就是时基时钟   相当于下降沿不延时

	EDIS;
}
