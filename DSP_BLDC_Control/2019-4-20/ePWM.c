
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

interrupt void main_isr(void);//PWM中断函数，主中断函数

Mark_Para Mark; //电机状态等变量
Uint16 Current_MOSFET_State;  //PWM两两相通时，记录MOS开通状态
int tempIsum=2300;
int filter[6]={2300};

//-------------------------------------------------------------------------
//驱动板的 使能引脚 初始化
//##########################################################################
void Gpio_IPM_Init(void)
{
	EALLOW;
	oENIPMMUX = 0;
	oENIPMDIR = 1;
	oENSET();
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


//TZ触发中断服务函数
//如果清除中断标志位，则TZ引脚再次检测，当检测到低电平是，再次进入该函数
//在试验场景下，故这里将清除中断标识取消
__interrupt void epwm1_tzint_isr(void)
{
	//	char i;
	//	volatile struct EPWM_REGS *PWMDef[] = {&EPwm1Regs,&EPwm2Regs,&EPwm3Regs};
	//
	//	EALLOW;
	//	for(i=0;i<3;i++)
	//	{
	//		PWMDef[i]->TZCLR.bit.OST = 1;//手动清除中断标识
	//		PWMDef[i]->TZCLR.bit.INT = 1;
	//	}
	//	EDIS;

	// 这里可以添加，  TZ触发后，控制器需要做的事情

	//   printf("\r\n\r\nTZ1 interrupt~~~");
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

//###########################################################################
//GPIO12/TZ1 IO口初始化
//---------------------------------------------------------------------------
void Motor_TzGpioInit(void)
{
	EALLOW;
	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;    // Enable pull-up on GPIO12 (TZ1)
	GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;  // Asynch input GPIO12 (TZ1)
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;   // Configure GPIO12 as TZ1
	EDIS;
}


//###########################################################################
//TZ功能控制
//使能或者关闭
//---------------------------------------------------------------------------
void TZ_OSHT_Ctr(Uint16 enFlag)
{
	char i;
	volatile struct EPWM_REGS *PWMDef[] = {&EPwm1Regs,&EPwm2Regs,&EPwm3Regs};

	if(enFlag)
	{
		EALLOW;
		for(i=0;i<3;i++)
		{
			PWMDef[i]->TZCLR.bit.OST = 1;
			PWMDef[i]->TZCLR.bit.INT = 1;
			PWMDef[i]->TZSEL.bit.OSHT1 = 1;        //OSHT使能
		}
		EDIS;
	}
	else
	{
		EALLOW;
		for(i=0;i<3;i++)
		{
			PWMDef[i]->TZCLR.bit.OST = 1;
			PWMDef[i]->TZCLR.bit.INT = 1;
			PWMDef[i]->TZSEL.bit.OSHT1 = 0;        //OSHT禁能
		}
		EDIS;
	}
}


#define PWMADC        //PWM触发ADC，在PWM计数周期触发
#define DB_TIMER 150  //150MHz 150参数即为1us 死区时间（仅上下对称PWM方式使用）
//Configuration Options for the Dead-Band Submodule
//PWMDef->DBCTL.bit.OUT_MODE = DBA_ALL;           //S5 = 0; S4 = 0
//PWMDef->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;	//S1 = 1; S0 = 1
//PWMDef->DBCTL.bit.POLSEL = DB_ACTV_HIC; 		//S3 = 1; S2 = 0

//#################################################
//PWM 初始化函数
//pwm 20K 频率，中央对其PWM模式（即计数器增减模式）
//-----------------------------------------------
void InitEPWM_Hpwm_Lopen(void)
{
	char i;
	volatile struct EPWM_REGS *PWMDef[] = {&EPwm1Regs,&EPwm2Regs,&EPwm3Regs};

	InitEPwm1Gpio();
	InitEPwm2Gpio();
	InitEPwm3Gpio();

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC =0;
	EDIS;

	for(i=0;i<3;i++)
	{
		EALLOW;
		//公式 T_PWM = 2 x TBPRD x T_TBCLK
		PWMDef[i]->TBPRD = PWM_FRE;	    //频率  20K
		PWMDef[i]->CMPA.half.CMPA = 0;  //占空比
		PWMDef[i]->TBPHS.half.TBPHS = 0x0000;            // Phase is 0
		PWMDef[i]->TBCTR = 0x0000;                       // Clear counter

		// Setup TBCLK
		PWMDef[i]->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up down
		PWMDef[i]->TBCTL.bit.PHSEN = TB_ENABLE;        //  Allow each timer to be sync'ed
		PWMDef[i]->TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
		PWMDef[i]->TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT;
		PWMDef[i]->TBCTL.bit.CLKDIV =    TB_DIV1;

		PWMDef[i]->CMPCTL.bit.SHDWAMODE = CC_SHADOW;
		PWMDef[i]->CMPCTL.bit.SHDWBMODE = CC_SHADOW;
		PWMDef[i]->CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
		PWMDef[i]->CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

		//TZ1连接到IPM的报警输出引脚
		PWMDef[i]->TZSEL.bit.OSHT1 = 1;
		PWMDef[i]->TZEINT.bit.OST = 1;         //TZ中断
		PWMDef[i]->TZCTL.bit.TZA = TZ_FORCE_LO;//TZ触发后，上管输出低电平
		PWMDef[i]->TZCTL.bit.TZB = TZ_FORCE_LO;//TZ触发后，下管输出低电平

		PWMDef[i]->DBCTL.all = 0;// Init Dead-Band Generator Control Register for EPWM1-EPWM3
		PWMDef[i]->PCCTL.all = 0;// Init PWM Chopper Control Register for EPWM1-EPWM3
		EDIS;
	}

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC =1;
	EDIS;

	//#ifdef PWMADC
	//	   EPwm1Regs.ETSEL.bit.SOCAEN	= 1;		// Enable SOC on A group
	//	   EPwm1Regs.ETSEL.bit.SOCASEL	= ET_CTR_PRD;		//
	//	   EPwm1Regs.ETPS.bit.SOCAPRD 	= 1;		// Generate pulse on 1st event
	//
	//	   EPwm1Regs.ETSEL.bit.SOCBEN	= 1;		// Enable SOC on B group
	//	   EPwm1Regs.ETSEL.bit.SOCBSEL	= ET_CTR_PRD;		//
	//	   EPwm1Regs.ETPS.bit.SOCBPRD 	= 1;		// Generate pulse on 1st event
	//#endif

	Motor_TzGpioInit();  //GPIO12 作为 TZ1 的IO初始化
	EALLOW;
	PieVectTable.EPWM1_TZINT = &epwm1_tzint_isr;
	//	   PieVectTable.EPWM2_TZINT = &epwm2_tzint_isr;
	//	   PieVectTable.EPWM3_TZINT = &epwm3_tzint_isr;
	EDIS;

	//TZ中断
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;
	//		PieCtrlRegs.PIEIER2.bit.INTx2 = 1;
	//		PieCtrlRegs.PIEIER2.bit.INTx3 = 1;
	IER |= M_INT2;
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
	EPwm1Regs.CMPA.half.CMPA = MotorADJ.PWM_Val;
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

//##########################################################################
#pragma CODE_SECTION(main_isr,"ramfuncs"); //设置中断服务函数在RAM里运行，提供效率
//PWM中断服务函数，
//中断机制：在PWM中间触发ADC采集，在PWM计时归零触发中断函数
//采集各模拟量，并进行电流闭环操作（仅在电流闭环模式下）
interrupt void  main_isr(void)
{
	int tempIdc;

	//结果数据缓冲器存储的转换数据为左对齐，故需要右移4位
	intAD_Vol[AD_ISUM] = AdcRegs.ADCRESULT0>>4;

//	filter[0]=filter[1];
//	filter[1]=filter[2];
//	filter[2]=filter[3];
//	filter[3]=filter[4];
//	filter[4]=filter[5];
//	filter[5]=intAD_Vol[AD_ISUM];
//	intAD_Vol[AD_ISUM]=getAverage(filter,6);
//	if(intAD_Vol[AD_ISUM]>2800||intAD_Vol[AD_ISUM]<2000)
//		intAD_Vol[AD_ISUM]=tempIsum;
//	tempIsum=intAD_Vol[AD_ISUM];

	intAD_Vol[AD_RG] = AdcRegs.ADCRESULT1>>4;
	intAD_Vol[AD_TMP] = AdcRegs.ADCRESULT2>>4;
	intAD_Vol[3]=AdcRegs.ADCRESULT3>>4;

	//电流闭环控制
	if(Mark.Status_Bits.Idc_Loop_Flag ==1 )
	{
		tempIdc = iptr->FbkPoint;
		// 电流参考值，在 RgSetSpeed 函数中由驱动板可调电阻给定
		iptr->FbkPoint = intAD_Vol[AD_ISUM] - 2048;
		if( iptr->FbkPoint <0 ) iptr->FbkPoint  = tempIdc; // Eliminate negative values
		//iptr->SetPoint 在 RgSetSpeed 函数中给定
		LocPIDCalc(iptr,1); //电机电流PID调整

		MotorADJ.fIdcOut = iptr->Fout;
		SetPWMVal((Uint16) MotorADJ.fIdcOut); //PWM输出
	}
	EPwm1Regs.ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

