#include "DSP2833x_Device.h"//DSP281x_Device.h提供了对应的CPU寄存器的结构体信息
#include "DSP2833x_Examples.h"//DSP281x_Examples.h主要提供一些常用函数
//为了使用函数，还需要将一些汇编代码文件添加到工程source中
int main(void) {
	//1 初始化系统控制，PLL，看门狗，预分频
	InitSysCtrl();
	//2 初始化GPIO
	InitGpio();
	//3 关闭终端，初始化PIE向量表
	DINT;
	InitPieCtrl();
	//PIE中断禁用，标志清空
	IER=0x0000;//禁用CPU
	IFR=0x0000;//清空CPU中断标志
	InitPieVectTable();//初始化PIE向量表
	//4 初始化所有外设
	//InitPeripherals();//需要用哪些硬件，在此函数中设置
	EINT;//Enable Global interrupt 使能全局中断
	ERTM;//Enable Global realtime interrupt启用全局实时中断
	// 配置GPIO0/GPIO1作为输出引脚
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
	EDIS;
	Uint32 main_counter=0;
	for(;;)
	{
		main_counter++;
		if(main_counter > 1000000){
			main_counter = 0;
			GpioDataRegs.GPATOGGLE.bit.GPIO6 = 1;
		}
	}
}
