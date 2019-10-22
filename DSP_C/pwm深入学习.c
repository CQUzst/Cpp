
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

Mark_Para Mark; //电机状态等变量
Uint16 Current_MOSFET_State;  //PWM两两相通时，记录MOS开通状态

//-------------------------------------------------------------------------
//驱动板的 使能引脚 初始化
//##########################################################################
void Gpio_IPM_Init(void)
{
	EALLOW;
	oENIPMMUX = 0;
	oENIPMDIR = 1;
	oENSET();	//使能IPM引脚，逻辑低电平使能，驱动板上的IPM是通过NPN三极管控制的
	EDIS;
}

//#################################################
//-----------------------------------------------
//读取BLDC霍尔IO
//-----------------------------------------------
Uint16 ReadHall(void)
{
	Uint16 temp[4]={0};
	Uint16 tempBuf[]={0,4,2,6,1,5,3,7};

	temp[0] = rHADAT();
	temp[1] = rHBDAT();
	temp[2] = rHCDAT();
	temp[3] = (temp[0] + temp[1]*2 + temp[2]*4);

	temp[1] = (~temp[3])&0x07; //驱动板上霍尔进过非门再输出到DSP板卡，故这里取反，将电机原本的霍尔信号恢复
	temp[0] = tempBuf[temp[1]];

	return (Uint16)(temp[0]&0x07);

}

//旋转方向的定义，调用PWM两两开通函数
pfnvoid pfnCW[]= {HallNull,HALL_0x45,HALL_0x34,HALL_0x23,HALL_0x12,HALL_0x61,HALL_0x56,HallNull};//cw pd
pfnvoid pfnFW[]= {HallNull,HALL_0x12,HALL_0x23,HALL_0x34,HALL_0x45,HALL_0x56,HALL_0x61,HallNull};//cw pdf

// UVW   SQ: 12-23-34-45-56-61
// HALL  ACQ: 132645
Uint16 FWHall1[]=       {0,1,3,2,5,6,4,0};//---A  ---------CW
Uint16 FWHall2[]=       {0,2,4,3,6,1,5,0};//---B
Uint16 FWHall3[]=       {0,3,5,4,1,2,6,0};//---C
Uint16 FWHall4[]=       {0,4,6,5,2,3,1,0};//---D-----------CCW
Uint16 FWHall5[]=       {0,5,1,6,3,4,2,0};//---E
Uint16 FWHall6[]=       {0,6,2,1,4,5,3,0};//---F

//------------------------------------------------------------------
//根据霍尔状态换相
//默认 Regulate_Speed.HallType 为1 ，S100 相序选择
//##########################################################################
void MOS_HX(Uint16 *HallState)
{
	if(Mark.Status_Bits.MotorState != M_STOP )
	{
		switch(HallEncode.HallType)
		{
		case M_CW:		pfnFW[FWHall1[*HallState]](); break;
		case M_CCW:		pfnFW[FWHall4[*HallState]](); break;
		default:break;
		}
	}
}


//###########################################################################
//---------------------------------------------------------------------------
//  PWM占空比设定
//---------------------------------------------------------------------------
void SetPWMVal(int PWMval)
{
	Uint16 REG_PWM_Val;
	if(PWMval <=PWM_FRE)
	{
		REG_PWM_Val =(Uint16)(PWM_FRE - PWMval);

		EALLOW;
		EPwm1Regs.CMPA.half.CMPA = REG_PWM_Val;
		//		EPwm1Regs.CMPB =REG_PWM_Val;
		EPwm2Regs.CMPA.half.CMPA = REG_PWM_Val;
		//		EPwm2Regs.CMPB =REG_PWM_Val;
		EPwm3Regs.CMPA.half.CMPA = REG_PWM_Val;
		//		EPwm3Regs.CMPB =REG_PWM_Val;
		EDIS;
	}
}


//#################################################
//霍尔观测测试
//-----------------------------------------------
void printfHall(Uint16 hallR)
{
	static Uint16  hallLast=0;
	if(hallLast!=hallR)
	{
		hallR = HallEncode.HallState;//60° 6个数 顺时针264513 一圈36个数
		//printf("\r\nH->%x",HallEncode.HallState);
		//		Uint16 hallNumber=hallR;
		//		send_number_to_USART(hallNumber);
		hallLast = hallR;
	}
}

//#define DB_TIMER 150  //150MHz 150参数即为1us 死区时间（仅上下对称PWM方式使用）
////Configuration Options for the Dead-Band Submodule
//PWMDef->DBCTL.bit.OUT_MODE = DBA_ALL;           //S5 = 0; S4 = 0
//PWMDef->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	//S1 = 1; S0 = 1
//PWMDef->DBCTL.bit.POLSEL = DB_ACTV_HIC; 		//S3 = 1; S2 = 0
#define CPU_CLK   150e6
#define PWM_CLK   10e3                // If diff freq. desired, change freq here.
#define SP        CPU_CLK/(2*PWM_CLK)
#define TBCTLVAL  0x200E              // Up-down cnt, timebase = SYSCLKOUT
//#################################################
//PWM 初始化函数
//pwm 20K 频率，中央对其PWM模式（即计数器增减模式）
//-----------------------------------------------
void Init_PWM(void)
{
	//	//先关闭时基时钟
	//	EALLOW;
	//	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC =0;
	//	EPwm1Regs.TBSTS.all=0;//时间基准状态寄存器：未达到最大值，无外部同步时间发生
	//	EPwm1Regs.TBPHS.half.TBPHS=0;//时间相位寄存器：忽略同步事件
	//	EPwm1Regs.TBCTR=0;//时间基准计数器
	//
	//	//	EPwm1Regs.CMPCTL.bit.SHDWAMODE=0x1;//A立即装载
	//	//	EPwm1Regs.CMPCTL.bit.SHDWBMODE=0x1;//B立即装载
	//	//	EPwm1Regs.CMPCTL.bit.LOADAMODE=0x0;//CTR=0时TBCTR=0X0000,立即装载模式下无效
	//	//	EPwm1Regs.CMPCTL.bit.LOADBMODE=0x0;
	//
	//	EPwm1Regs.CMPCTL.bit.SHDWAMODE=0x0;//影子寄存器
	//	EPwm1Regs.CMPCTL.bit.SHDWBMODE=0x0;
	//	EPwm1Regs.CMPCTL.bit.LOADAMODE=0x0;//CTR=0时TBCTR=0X0000,立即装载模式下无效
	//	EPwm1Regs.CMPCTL.bit.LOADBMODE=0x0;
	//
	//	EPwm1Regs.TBCTL.bit.CTRMODE = 0x2;  // 上升下降计数模式
	//	EPwm1Regs.TBCTL.bit.PHSEN = 0x1;          //  使能相位同步功能
	//	EPwm1Regs.TBCTL.bit.SYNCOSEL = 0x1; // 接受同步输出信号
	//	EPwm1Regs.TBCTL.bit.PRDLD=0x1;//立即装载TBPRD
	//	EPwm1Regs.TBCTL.bit.PHSDIR=0x1;//同步后增计数
	//
	//	EPwm1Regs.TBPRD = PWM_FRE;	    //频率  20K
	//	EPwm1Regs.CMPA.half.CMPA = PWM_FRE/3;  //占空比先默认为0，避免电机启动突转
	//	EPwm1Regs.CMPB=0;//计数比较寄存器B
	//
	//	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x0;
	//	EPwm1Regs.TBCTL.bit.CLKDIV = 0x0;
	//
	//
	//
	//	EPwm1Regs.AQCTLA.bit.CAU=0x2;		//CTR=CMPA且计数器递增时，EPWMxA=1，强制输出高
	//	EPwm1Regs.AQCTLA.bit.CAD=0x1;		//CTR=CMPA且计数器递减时，EPWMxA=0，强制输出低
	//	EPwm1Regs.AQCTLB.all=0;             //B不操作
	//	EPwm1Regs.AQSFRC.all=0;				//动作限定软件强制寄存器 无软件强制操作
	//	EPwm1Regs.AQCSFRC.all=0;		//	此位为ADSFRC的阴影寄存器，无动作
	//
	//	EPwm1Regs.TZSEL.all=0;				//TZ no use
	//	EPwm1Regs.TZCTL.all=0;
	//	EPwm1Regs.TZEINT.all=0;
	//	EPwm1Regs.TZFLG.all=0;
	//	EPwm1Regs.TZCLR.all=0;
	//	EPwm1Regs.TZFRC.all=0;
	//
	//	EPwm1Regs.ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
	//	EPwm1Regs.ETFLG.all=0;
	//	EPwm1Regs.ETCLR.all=0;
	//	EPwm1Regs.ETFRC.all=0;
	//
	//
	//	EPwm1Regs.DBCTL.bit.OUT_MODE=0x3;// EPWMxB 互补、无死区延时
	//	EPwm1Regs.DBCTL.bit.POLSEL=0x2;//互补
	//	EPwm1Regs.DBRED=0;//上升沿延时
	//	EPwm1Regs.DBFED=0;//下降沿延时
	//
	//	EPwm1Regs.PCCTL.all = 0;//不设置斩波
	//
	//
	//	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC =1;
	//	EDIS;
	//

	char i;
	volatile struct EPWM_REGS *PWMDef[] = {&EPwm1Regs,&EPwm2Regs,&EPwm3Regs};

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC =0;
	EDIS;
	for(i=0;i<3;i++)
	{
		//		EALLOW;
		//		PWMDef[i]->TBSTS.all=0;//时间基准状态寄存器：未达到最大值，无外部同步时间发生
		//		PWMDef[i]->TBPHS.half.TBPHS=0;//时间相位寄存器：忽略同步事件
		//		PWMDef[i]->TBCTR=0;//时间基准计数器
		//
		//		PWMDef[i]->CMPCTL.bit.SHDWAMODE=0x1;//立即装载
		//		PWMDef[i]->CMPCTL.bit.SHDWBMODE=0x1;
		//		PWMDef[i]->CMPCTL.bit.LOADAMODE=0x0;//CTR=0时TBCTR=0X0000,立即装载模式下无效
		//		PWMDef[i]->CMPCTL.bit.LOADBMODE=0x0;
		//
		//		// Setup TBCLK
		//		PWMDef[i]->TBCTL.bit.PRDLD=0x1;//立即装载TBPRD
		//		PWMDef[i]->TBCTL.bit.PHSDIR=0x1;//同步后增计数
		//		PWMDef[i]->TBCTL.bit.CTRMODE = 0x2;  // 上升下降计数模式
		//		PWMDef[i]->TBCTL.bit.PHSEN = 0x1;          //  使能相位同步功能
		//		PWMDef[i]->TBCTL.bit.SYNCOSEL = 0x1; // 接受同步输出信号
		//		PWMDef[i]->TBCTL.bit.HSPCLKDIV = TB_DIV1;        // 时钟不分频
		//		PWMDef[i]->TBCTL.bit.CLKDIV =    TB_DIV1;        // 时钟不分频
		//
		//		PWMDef[i]->TBPRD = PWM_FRE;	    //频率  20K
		//		PWMDef[i]->CMPA.half.CMPA = 500;
		//		PWMDef[i]->CMPB=0;
		//
		//		PWMDef[i]->AQCTLA.bit.CAU=0x2;		//CTR=CMPA且计数器递增时，EPWMxA=1，强制输出高
		//		PWMDef[i]->AQCTLA.bit.CAD=0x1;		//CTR=CMPA且计数器递减时，EPWMxA=0，强制输出低
		//		PWMDef[i]->AQCTLB.all=0;             //B不操作
		//		PWMDef[i]->AQSFRC.all=0;				//动作限定软件强制寄存器 无软件强制操作
		//		PWMDef[i]->AQCSFRC.all=0;		//	此位为ADSFRC的阴影寄存器，无动作
		//
		//		PWMDef[i]->TZSEL.all=0;				//TZ no use
		//		PWMDef[i]->TZCTL.all=0;
		//		PWMDef[i]->TZEINT.all=0;
		//		PWMDef[i]->TZFLG.all=0;
		//		PWMDef[i]->TZCLR.all=0;
		//		PWMDef[i]->TZFRC.all=0;
		//
		//		PWMDef[i]->ETSEL.all=0;            // Interrupt when TBCTR = 0x0000
		//		PWMDef[i]->ETFLG.all=0;
		//		PWMDef[i]->ETCLR.all=0;
		//		PWMDef[i]->ETFRC.all=0;
		//
		//		PWMDef[i]->DBCTL.bit.OUT_MODE=0x3;// EPWMxB 互补、无死区延时
		//		PWMDef[i]->DBCTL.bit.POLSEL=0x2;//互补
		//		PWMDef[i]->DBRED=0;//上升沿延时
		//		PWMDef[i]->DBFED=0;//下降沿延时
		//
		//		PWMDef[i]->PCCTL.all = 0;//不设置斩波

		EALLOW;
		PWMDef[i]->TBPRD = PWM_FRE;	    //频率  20K,周期3750
		PWMDef[i]->CMPA.half.CMPA = 0;  //占空比
		PWMDef[i]->TBPHS.half.TBPHS = 0x0000;
		PWMDef[i]->TBCTR = 0x0000;

		// Setup TBCLK
		PWMDef[i]->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //上升下降计数模式
		PWMDef[i]->TBCTL.bit.PHSEN = TB_ENABLE;         //使能加载相位
		PWMDef[i]->TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;//不接受同步输出信号
		PWMDef[i]->TBCTL.bit.HSPCLKDIV = TB_DIV1;       //时钟不分频
		PWMDef[i]->TBCTL.bit.CLKDIV =    TB_DIV1;

		PWMDef[i]->CMPCTL.bit.SHDWAMODE = CC_SHADOW;	//加载影子寄存器
		PWMDef[i]->CMPCTL.bit.SHDWBMODE = CC_SHADOW;
		PWMDef[i]->CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;	//时钟位0时加载
		PWMDef[i]->CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

		PWMDef[i]->DBCTL.all = 0;// 初始化死区控制，默认为0，不进行操作
		PWMDef[i]->PCCTL.all = 0;// 初始化斩波寄存器，默认位0，不进行操作
		EDIS;
	}
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC =1;
	EDIS;
}



void HallNull()
{
	;
}

//###########################################################################
//以下为两两导通MOS函数
//---------------------------------------------------------------------------
void HALL_0x12(void)
{
	EPwm1Regs.AQCSFRC.bit.CSFA=0;//AQ强制输出失效
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm1Regs.CMPA.half.CMPA = 2000;//MotorADJ.PWM_Val
	EPwm1Regs.AQCSFRC.bit.CSFB=1;//AQ强制输出低电平

	EPwm2Regs.AQCSFRC.bit.CSFA=1;
	EPwm2Regs.AQCSFRC.bit.CSFB=1;

	EPwm3Regs.AQCSFRC.bit.CSFA=1;
	EPwm3Regs.AQCSFRC.bit.CSFB=2;//AQ强制输出高电平

	Current_MOSFET_State = 0;
}


void HALL_0x23(void)
{
	EPwm1Regs.AQCSFRC.bit.CSFA=1;
	EPwm1Regs.AQCSFRC.bit.CSFB=1;

	EPwm2Regs.AQCSFRC.bit.CSFA=0;//AQ强制输出失效
	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm2Regs.CMPA.half.CMPA = MotorADJ.PWM_Val;
	EPwm2Regs.AQCSFRC.bit.CSFB=1;//AQ强制输出低电平

	EPwm3Regs.AQCSFRC.bit.CSFA=1;
	EPwm3Regs.AQCSFRC.bit.CSFB=2;//AQ强制输出高电平

	Current_MOSFET_State = 1;
}

void HALL_0x34(void)
{
	EPwm1Regs.AQCSFRC.bit.CSFA=1;
	EPwm1Regs.AQCSFRC.bit.CSFB=2;//AQ强制输出高电平

	EPwm2Regs.AQCSFRC.bit.CSFA=0;//AQ强制输出失效
	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm2Regs.CMPA.half.CMPA = MotorADJ.PWM_Val;
	EPwm2Regs.AQCSFRC.bit.CSFB=1;//AQ强制输出低电平

	EPwm3Regs.AQCSFRC.bit.CSFA=1;
	EPwm3Regs.AQCSFRC.bit.CSFB=1;

	Current_MOSFET_State  = 2;
}

void HALL_0x45(void)
{
	EPwm1Regs.AQCSFRC.bit.CSFA=1;
	EPwm1Regs.AQCSFRC.bit.CSFB=2;//AQ强制输出高电平

	EPwm2Regs.AQCSFRC.bit.CSFA=1;
	EPwm2Regs.AQCSFRC.bit.CSFB=1;

	EPwm3Regs.AQCSFRC.bit.CSFA=0;//AQ强制输出失效
	EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm3Regs.CMPA.half.CMPA = MotorADJ.PWM_Val;
	EPwm3Regs.AQCSFRC.bit.CSFB=1;//AQ强制输出低电平

	Current_MOSFET_State  = 3;
}

void HALL_0x56(void)
{
	EPwm1Regs.AQCSFRC.bit.CSFA=1;
	EPwm1Regs.AQCSFRC.bit.CSFB=1;

	EPwm2Regs.AQCSFRC.bit.CSFA=1;
	EPwm2Regs.AQCSFRC.bit.CSFB=2;//AQ强制输出高电平

	EPwm3Regs.AQCSFRC.bit.CSFA=0;//AQ强制输出失效
	EPwm3Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm3Regs.AQCTLA.bit.CAD = AQ_CLEAR;
	EPwm3Regs.CMPA.half.CMPA = MotorADJ.PWM_Val;
	EPwm3Regs.AQCSFRC.bit.CSFB=1;//AQ强制输出低电平

	Current_MOSFET_State = 4;
}

void HALL_0x61(void) //UH PWM  VL HI
{
	EPwm1Regs.AQCSFRC.bit.CSFA=0;//AQ强制输出失效
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;//
	EPwm1Regs.CMPA.half.CMPA = MotorADJ.PWM_Val;
	EPwm1Regs.AQCSFRC.bit.CSFB=1;//AQ强制输出低电平

	EPwm2Regs.AQCSFRC.bit.CSFA=1;
	EPwm2Regs.AQCSFRC.bit.CSFB=2;//AQ强制输出高低电平

	EPwm3Regs.AQCSFRC.bit.CSFA=1;
	EPwm3Regs.AQCSFRC.bit.CSFB=1;

	Current_MOSFET_State  = 5;
}

//电机停止时，三个上管关闭，三个下管开启
void openLowBrige(void)
{
	EALLOW;
	EPwm1Regs.AQCSFRC.bit.CSFA=1;
	EPwm1Regs.AQCSFRC.bit.CSFB=2;
	EPwm2Regs.AQCSFRC.bit.CSFA=1;
	EPwm2Regs.AQCSFRC.bit.CSFB=2;
	EPwm3Regs.AQCSFRC.bit.CSFA=1;
	EPwm3Regs.AQCSFRC.bit.CSFB=2;
	EDIS;
}

