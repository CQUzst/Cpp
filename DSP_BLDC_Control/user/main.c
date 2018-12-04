/*
 * author:zst
 * version:12_3
 */
#include "DSP28x_Project.h"
#include "HK_all_include.h"

void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
	while(SourceAddr < SourceEndAddr)
		*DestAddr++ = *SourceAddr++;
	return;
}

void InitLED(void); //初始化板载D400  D401 2个LED指示灯
void InitKEY(void); //初始化板载S1按键
void scanKey(void); //按键 操作函数，包括启动/停止电机，数码管显示内容切换等操作
void InitADC(void);
void InitI2C(void);
void InitpwmINT(void);

extern int tempIsum;

//**************************主函数*********************************
void main(void) 
{
	//系统初始化
	InitSysCtrl();
	DINT;
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

#ifdef _DEBUG
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash();
#endif

	//IPM控制的IO初始化
	Gpio_IPM_Init();

	//使能IPM引脚，逻辑低电平使能，驱动板上的IPM是通过NPN三极管控制的
	oENSET();

	InitKEY();//按键初始化 S1
	InitLED();//LED初始化 D400 D401
	D400LOW();//关闭D400
	D401LOW();//关闭D401

	//初始化SCIA,115200bps，接收中断，接收FIFO深度--1级深度
	SCI_Init();

	//测试发送字符串
	sciPutString(1,"start！", 1);
	Uint16 testNumber=14;
	send_number_to_USART(testNumber);

	//调用官方库函数，初始化CPU定时器
	InitCpuTimers();

	//定时器0初始化，初始化中断1ms周期
	Timer0_init();

	//TM1650初始化  IIC驱动
	InitI2C();

	//DSP PWM初始化
	InitEPWM_Hpwm_Lopen();

	//开启TZ报警检测
	TZ_OSHT_Ctr(1);

	//AD采样初始化
	InitADC();

	//映射PWM中断向量初始化
	InitpwmINT();

	//霍尔信号反馈初始化，采用中断机制
	ExInt_Init();

	//CAP测速模块初始化
	Motor_Cap_Init();

	//电机状态参数清零初始化
	Mark.All=0;

	//M_OPENLOOP，速度开环模式，M_SPEEDLOOP速度闭环模式，M_CURRRENTLOOP电流闭环模式
	Mark.Status_Bits.TestMode = M_OPENLOOP ;

	//设定运行方向，逆时针 正转
	HallEncode.HallType = M_CCW;

	D400ON();                     //D400亮，指示正转设置；灭，指示反转设置

	//PID控制器参数初始化，速度/电流闭环PID参数、幅值等初始化
	IncPIDInit();

	//使能PIE模块
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

	//使能全局中断
	EINT;
	ERTM;

	//BIT6到BIT4为亮度调节，BIT0是  1 开启/0关闭
	LigntVal = 0x11;

	//设定亮度
	TM1650_Send(CMD_SEG, LigntVal);

	while(1)
	{
		//1ms任务
		if( timer0Base.Mark_Para.Status_Bits.OnemsdFlag == 1)
		{
			//			sciPutString(1,"Ic=",0);
			//			send_number_to_USART(intAD_Vol[3]);
			//输出电流和
			sciPutString(1,"Isum=",0);
			send_number_to_USART(intAD_Vol[0]);
			timer0Base.Mark_Para.Status_Bits.OnemsdFlag =0;//清除置位
			//0.2s（200ms）任务
			if( timer0Base.msCounter > 200)//200
			{
				timer0Base.msCounter =0;
				D401TOGGLE();

				DriverCheck();//错误检测,判断霍尔线是否接入
				GetIpmTemp(); //将驱动板热敏电阻的分压输出，转换为温度
				RgSetRef();   //驱动板可调电阻设定 速度/电流等
				scanKey();	  //扫描按键响应


				//数码管显示内容，来自于 scanKey() 函数控制S2 S3 S4
				TM1650_SEG_show(SEG_DIG4_Cmd,*PtrSegType);
				if(Mark.Status_Bits.DrawLineFlag ==1 )
					DrawLine();  //串口上传参数，配合图形软件使用 （建议周期调用）
			}
		}

		//2个重要的电机控制 主函数
		//    	MotorControl(); 在timer0_base.c 的定时器0中断
		//    	main_isr();     ePWM.c  的 20K PWM中断服务函数

		decodeUart();//串口接收来自PC发送的命令
	}
}									








//初始化LED   D400  GPIO8/PWM5A   输出	 D401  GPIO10/PWM6A  输出
void InitLED()
{
	EALLOW;
	D400MUX = 0;//GPIO功能
	D400DIR = 1;//输出
	D401MUX = 0;
	D401DIR = 1;
	EDIS;
}

//初始化独立按键 S100 连接到GPIO34
void InitKEY()
{
	EALLOW;
	S100MUX = 0;//GPIO功能
	S100DIR = 0;//输入
	S100PUD = 1;//由于开发板外置了上拉电阻，禁用DSP内部上拉
	EDIS;
}
void InitADC()
{
	//P203上的ADC接口
	ChSel[0]=2;		// A2--->IfbSum
	ChSel[1]=6;		// A6 -> RG
	ChSel[2]=0;		// A0 -> IPM V_Tsens
	//	ChSel[1]=1;		// ChSelect: ADC A1-> Phase A Current
	//	ChSel[2]=9;		// ChSelect: ADC B1-> Phase B Current
	ChSel[3]=3;		// ChSelect: ADC A3-> Phase C Current三相电流只采一相
	ChSel[4]=15;	// ChSelect: ADC B7-> Phase A Voltage
	ChSel[5]=14;	// ChSelect: ADC B6-> Phase B Voltage
	ChSel[6]=12;	// ChSelect: ADC B4-> Phase C Voltage
	ChSel[7]=7;		// ChSelect: ADC A7-> DC Bus  Voltage
	//	ChSel[8]=6;		// A6 -> RG
	//	ChSel[9]=0;		// A0 -> IPM V_Tsens


	ADC_MACRO_INIT_Dual(ChSel,ACQPS);//n通道ADC初始化，MotorA
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;     /* Enable SOCA */
	/* 中断设置--重要A:PWM中间开启，触发ADC采集 Enable period event for SOCA */
	EPwm1Regs.ETSEL.bit.SOCASEL = 2;
	EPwm1Regs.ETPS.bit.SOCAPRD = 1;     /* Generate SOCA on the 1st event */
	EPwm1Regs.ETCLR.bit.SOCA = 1;       /* Clear SOCA flag */
}

//初始化I2C
void InitI2C()
{
	softResetIIC_BUS(); //软件复位IIC从设备
	InitI2C_Gpio(); //io 初始化为IIC
	I2CA_Init();    //HW IIC初始化，100KHz
	PtrSegType = SEG_Show_Type[0];//数码管默认显示的内容 ---设定速度
}

//初始化pwm中断
void InitpwmINT()
{
	EALLOW;
	PieVectTable.EPWM1_INT = &main_isr;
	EDIS;

	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
	// Enable EPWM1INT generation
	EPwm1Regs.ETSEL.bit.INTEN = 1;
	// PWM TB计数为零 进入中断 Enable interrupt CNT_zero event
	EPwm1Regs.ETSEL.bit.INTSEL = 1;
	// 多少个PWM周期进来一次中断 Generate interrupt on the 1st event
	EPwm1Regs.ETPS.bit.INTPRD = 1;
	// Enable more interrupts
	EPwm1Regs.ETCLR.bit.INT = 1;
	IER |= M_INT3;

}
/************************************************************************/
//扫描读取按键操作函数
//独立IO按键 S1   GPIO34
//扫描按键S2--S6 采用TM1650扫描
//测试功能：
//  S1  电机--启动或停止;
//  S2  数码管显示--设定速度;
//  S3  数码管显示--实时速度;
//  S4  数码管显示--IPM温度（或者IR2136驱动板的话是MOS管周围空气温度）;
//  S5  电机--当处于停止时，改变电机转动方向，D400--亮--正转；  灭--反转
//  S6  开启或者关闭串口 上传参数（需要配合串口图形软件使用）
/************************************************************************/
Uint16 S1Flag=0;
void scanKey(void)
{
	//	char TM1650KeyVal[0x44,0x4c,0x54,0x5c,0x64,0x6c,0x74];
	if((rS100DAT()==0)  && (S1Flag==0))//S1按一下，设置电机启动或者停止
	{
		S1Flag=1;
		//printf("\r\nS1");
		if(Mark.Status_Bits.MotorState == M_STOP)//电机停止时，按下S1则启动电机
		{
			//printf("\r\n-->按键 启动电机");

			Mark.Status_Bits.MotorState = M_RUN;

			MotorADJ.CloseSPLoopCnt=0;

			MotorADJ.MoniSpeedZeroCNT = 0;
			MotorADJ.OpenLoopSetUpCNT = 10;//开环启动时间ms，在定时器0中进行递减

			MotorADJ.PWM_OpenVal = START_PWM_ZKB;//设置开环启动的占空比
			MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;
			HallEncode.HallState = ReadHall();//读取电机霍尔位置
			MOS_HX(&HallEncode.HallState);    //输出PWM以启动电机
		}
		else if(Mark.Status_Bits.MotorState == M_RUN)//电机运行中，则先进入减速状态
		{
			//printf("\r\n-->按键 停止电机");
			Mark.Status_Bits.MotorState = M_ACCDOWN;
		}

	}
	else if((rS100DAT()==0)  && (S1Flag==1))
	{
		S1Flag = 1;
	}
	else
	{
		S1Flag = 0;
	}
	//---------------TM1650获取扫描按键S2-S5操作--------------------
	TM1650_Read(CMD_KEY,&keyVal);
	//S2 接到SEGA和DIG1上  <TM1650.pdf>  P4 键盘扫描码
	if(keyVal == S2_CODE)
	{
		PtrSegType = SEG_Show_Type[0];
		SEG_DIG4_Cmd =0;
	}
	//S3接到SEGB和DIG1上
	if(keyVal == S3_CODE)
	{
		PtrSegType = SEG_Show_Type[1];
		SEG_DIG4_Cmd = 0;
	}
	//S4接到SEGC和DIG1上
	if(keyVal == S4_CODE)
	{
		PtrSegType = SEG_Show_Type[2];
		SEG_DIG4_Cmd =0x0F;
	}
	//S5接到SEGD和DIG1上
	if(keyVal == S5_CODE)
	{
		if(Mark.Status_Bits.MotorState == M_STOP)
		{
			if(HallEncode.HallType == M_CCW)
			{
				HallEncode.HallType = M_CW;//反转/顺时针转
				D400LOW();                 //灭，指示反转设置
			}
			else
			{
				HallEncode.HallType = M_CCW;//正转/逆时针转
				D400ON();                   //亮，指示正转设置
			}
		}
	}
	//S6接到SEGE和DIG1上
	if(keyVal == S6_CODE)
	{
		Mark.Status_Bits.DrawLineFlag ^=1 ;//开启或者关闭 串口上传
	}
}


