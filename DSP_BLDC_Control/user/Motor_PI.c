
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

Uint16 OpenLoopPWMval=0;
PI_CONTROLLER pid1_idc = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pid1_spd = PI_CONTROLLER_DEFAULTS;

_iq CurrentSet = _IQ(0.01);
_iq DCbus_current=0;
_iq SpeedRef=_IQ(0.3);
_iq Spd_Idc;

SPEED_MEAS_CAP speed1 = SPEED_MEAS_CAP_DEFAULTS;

Uint16 hallFindCnt=0;//查找HALL与UVW相位关系， 绕组给定PWM时间/ ms单位
int iResVal;        //可调电阻采样的AD值

//串口上传的2个定义变量
Uint16 CRCcode;
char CmdT[20];

DEF_Motor_REG  MotorADJ={0}; //定义电机控制相关变量

//###########################################################################
//CapVal为输入从Cap引脚捕获的计数，
//2极对，机械一圈有2个测速信号
//---------------------------------------------------------------------------
//速度 RPM =  (SYSCLK/POLES/CapVal)x60， POLES极对数，即为一圈POLES组编码器脉冲

//###########################################################################
//驱动板可调电阻采样结果，转换为 设定速度、设定电流 （也就是闭环PID的给定参考）
//---------------------------------------------------------------------------
void RgSetRef(void)
{
	static int temp1=0,temp2=0,temp3=0;
	if((Mark.Status_Bits.TestMode == M_SPEEDLOOP)||(Mark.Status_Bits.TestMode ==M_CURRRENTLOOP))
	{
		iResVal = intAD_Vol[AD_RG]*0.909;  //28335的ADC量程3V缩放 量程3.3V
		if(Mark.Status_Bits.MotorState != M_ACCDOWN)
		{
			temp1 = iResVal;

			//设定电流参考值
			iptr->SetPoint  = iResVal;
			CLAMP(iptr->SetPoint,MIN_IDC,MAX_IDC);

			//设定速度参考值
			temp3 = temp1 - temp2 ;
			if((temp3>50)||(temp3<-50))//大于或者小于 50 的范围时，进行设定速度的调整
			{
				temp2 = temp1;
				MotorADJ.Set_Speed = temp1 -temp1 % 50;//设定速度的取整
			}
		}
		else  //减速过程，设置参考值等于0
		{
			temp2 =0;
			MotorADJ.Set_Speed = 0;   //减速状态下，设定速度为零
			iptr->SetPoint =0;        //设定电流为0
		}
	}
	else if(Mark.Status_Bits.TestMode == M_OPENLOOP)//开环模式下占空比的给定
	{
		MotorADJ.Set_Speed = intAD_Vol[AD_RG];  //占空比来自可调电阻的值
	}
}

PID sPID;      //速度PID
PID *sptr = &sPID;

PID currentPID;//电流PID
PID *iptr = &currentPID;

//PID 参数初始化
//0.1  0.001 0.01
void IncPIDInit(void)
{
	sptr->SumError = 0;
	sptr->LastError = 0; //Error[-1]
	sptr->PrevError = 0; //Error[-2]
	sptr->Proportion = 1.5;//0.02;  //比例常数 Proportional Const
	sptr->Integral = 0.0015;//0.001;   //积分常数 Integral Const
	sptr->Derivative = 0.0005;//0.001; //微分常数 Derivative Const
	sptr->SetPoint = 0;
	sptr->FbkPoint = 0;
	sptr-> Fout= 0;
	sptr-> Umax= MAX_PWM_ZKB;
	sptr-> Umin= 0;

	iptr->SumError = 0;
	iptr->LastError = 0; //Error[-1]
	iptr->PrevError = 0; //Error[-2]
	iptr->Proportion = 2; //比例常数 Proportional Const
	iptr->Integral = 0.1; //积分常数 Integral Const
	iptr->Derivative = 0; //微分常数 Derivative Const
	iptr->SetPoint = 0;
	iptr->FbkPoint = 0;
	iptr-> Fout= 0;
	iptr-> Umax= MAX_PWM_ZKB;
	iptr-> Umin= 0;
}

//#################################################
//-----------------------------------------------
//位置式 PID 控制，当输出达到限幅时对积分限制处理以防饱和
//-----------------------------------------------
void LocPIDCalc(PID *ptr,Uint16 flag)
{
	long tepmSumError;

	int iError,dError;
	iError = ptr->SetPoint - ptr->FbkPoint; //偏差
	tepmSumError = ptr->SumError;

	if(flag ==1)
	{
		ptr->SumError += iError;                //积分
		dError = iError - ptr->LastError;      //微分
		ptr->LastError = iError;
		//比例项  + 积分项 + 微分项/
		ptr-> Fout = (ptr->Proportion * iError + ptr->Integral * ptr->SumError + ptr->Derivative * dError);
	}
	else
	{
		ptr-> Fout = ptr->Proportion * iError *0.2;
	}

	//限幅输出
	if(ptr->Fout > ptr->Umax)
	{
		ptr-> Fout = ptr->Umax;
		ptr->SumError = tepmSumError*0.9;//防止积分饱和
	}
	else if(ptr->Fout < ptr->Umin)
	{
		ptr-> Fout = ptr->Umin;
		ptr->SumError = tepmSumError;
	}
}



//###########################################################################
//自检报警函数
// 判断霍尔传感器是否正常
// 判断是否驱动板报警
//---------------------------------------------------------------------------------------------
void DriverCheck()
{
	Uint16 HallState = ReadHall();//读取霍尔位置
	//霍尔传感器未接入、错误、控制24V电源未接入等，DSP无法正确获取霍尔位置，程序无限循环进行D401快速闪烁报警
	if((HallState==0) || (HallState==7))
	{
		//		showHanZi(0,300,&Hanzi6[0][0],15);//编码器报警

		//数码管显示0xEE07 表示 霍尔编码器错误
		TM1650_Send(DIG4, SEG7Table[0x0E]); //GID1
		TM1650_Send(DIG3, SEG7Table[0x0E]); //GID2
		TM1650_Send(DIG2, SEG7Table[0x00]); //GID3
		TM1650_Send(DIG1, SEG7Table[0x07]); //GID4

		//printf("\r\n霍尔编码器报警");
		for(;;)
		{
			DELAY_US(100000);
			D401TOGGLE();
		}
	}

	if(EPwm1Regs.TZFLG.bit.OST==0x1)//IPM TZ报警触发，请检查 过流、过温 、IPM 15V 的欠压过压
	{
		//		showHanZi(0,300,&Hanzi7[0][0],12);//IPM 报警

		//数码管显示0xEE01 表示TZ报警
		TM1650_Send(DIG4, SEG7Table[0x0E]); //GID1
		TM1650_Send(DIG3, SEG7Table[0x0E]); //GID2
		TM1650_Send(DIG2, SEG7Table[0x00]); //GID3
		TM1650_Send(DIG1, SEG7Table[0x01]); //GID4

		//		printf("\r\nIPM TZ报警触发报警，请检查 过流、过温 、IPM 15V 的欠压过压");
		for(;;)
		{
			DELAY_US(100000);
			D401TOGGLE();
		}
	}

	//可加入母线 过压欠压报警
}



//#################################################
//CRC 校验
//-----------------------------------------------
unsigned int GetCRC16(char *inPtr, unsigned int len) {
	unsigned int crc = 0xffff;
	unsigned char index;

	while (len > 0) {
		crc ^= *inPtr;
		for (index = 0; index < 8; index++) {
			if ((crc & 0x0001) == 0)
				crc = crc >> 1;
			else {
				crc = crc >> 1;
				crc ^= 0xa001;
			}
		}

		len -= 1;
		inPtr++;
	}
	return (((crc & 0x00ff) << 8) | ((crc & 0xff00) >> 8));
}


//#################################################
//串口上传数据，绘制曲线
//配合图形监控软件使用， PC端注意 通道数 和 串口波特率 2个参数设定
//上传参数，配合电机软件观测
//上传格式  ： 参数数量（2个byte为一个参数，高位在前） 参数1 参数2 。。。。。 + CRC
//-----------------------------------------------
void DrawLine()
{
	int temp;

	CmdT[0]=3;//通道数
	scia_xmit(CmdT[0]);//发送通道数

	//设定速度
	HexPrintf(MotorADJ.Set_Speed);//timer0Base.msCounter
	CmdT[1] = (MotorADJ.Set_Speed)>>8;
	CmdT[2] = (MotorADJ.Set_Speed)&0XFF;

	//实时速度
	HexPrintf(MotorADJ.Now_Speed);
	CmdT[3] = (MotorADJ.Now_Speed)>>8;
	CmdT[4] = (MotorADJ.Now_Speed)&0XFF;

	//速度PID闭环，输出结果给PWM
	temp = sptr->Fout;
	HexPrintf(temp);
	CmdT[5] = (temp)>>8;
	CmdT[6] = (temp)&0XFF;

	//发送CRC
	CRCcode = GetCRC16(&CmdT[0],(CmdT[0]<<1)+1);
	HexPrintf(CRCcode);
}
//#################################################
//-----------------------------------------------
//电机控制的几个模式处理  主函数
//-----------------------------------------------
void MotorControl(void)
{
	switch(Mark.Status_Bits.TestMode)
	{
	case M_IDLE: //空闲模式，打开三个下管，关闭三个上管
		MotorADJ.PWM_Val =0;
		openLowBrige();
		break;
	case M_PWM_TEST://PWM信号观测模式
		SetPWMVal(intAD_Vol[AD_RG]>>1);//改变输出PWM波的占空比
		break;

	case M_UVW_HALL_TEST://电机绕组和霍尔信号查找模式
		if(hallFindCnt>0)
		{
			if(MotorADJ.PWM_Val  < TEST_UVW_MAX_ZKB)
			{
				MotorADJ.PWM_Val+=1;
				SetPWMVal(MotorADJ.PWM_Val);
			}
		}
		else if(hallFindCnt==0)
		{
			//printf("\r\n%d",MotorADJ.PWM_Val);
			Mark.Status_Bits.TestMode = M_IDLE;
			HallEncode.HallState = ReadHall();
			openLowBrige();
		}
		break;

	case M_HALL_PRINTF:                       //串口打印霍尔位置，【测试使用，可以取消】
		printfHall(HallEncode.HallState);//霍尔信号的值在3个外部中断函数里读取
		break;

	case M_OPENLOOP:                                //速度开环模式(可用于测试LBDC电机的 UVW线和霍尔对应关系)
		if(Mark.Status_Bits.MotorState == M_RUN)//开环运行过程中，占空比来自可调电阻的给定
		{
			MotorADJ.PWM_OpenVal =OpenLoopPWMval;  //占空比赋值
			CLAMP(MotorADJ.PWM_OpenVal,MIN_PWM_ZKB,MAX_PWM_ZKB);
			MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;	//在每个换向函数里赋值占空比

			if( MotorADJ.Now_Speed==0)
			{
				HallEncode.HallState = ReadHall();//读取电机霍尔位置
				MOS_HX(&HallEncode.HallState);    //输出PWM开环启动电机
			}
		}
		else if(Mark.Status_Bits.MotorState == M_ACCDOWN)//占空比减小，减速停机
		{
			if(MotorADJ.PWM_OpenVal) MotorADJ.PWM_OpenVal--;    //占空比减小
			MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;	//在每个换向函数里赋值占空比

			if(MotorADJ.PWM_OpenVal==0)//速度降到一定时，下管完全打开，进行停机
			{
				Mark.Status_Bits.MotorState = M_STOP;
				openLowBrige();
			}
		}

		break;

	case M_SPEEDLOOP://速度闭环模式
		if (Mark.Status_Bits.MotorState != M_STOP) //电机运行命令使能
		{
			//电机处于停止状态，进行电机停止的判断
			if(Mark.Status_Bits.MotorState == M_ACCDOWN)
			{
				MotorADJ.PWM_Val++;
				if(MotorADJ.PWM_Val == PWM_FRE )//速度降到一定时，下管完全打开，进行停机
				{
					Mark.Status_Bits.MotorState = M_STOP;
					openLowBrige();
					sptr->LastError=0;
					sptr->SumError =0;
				}
			}
			else if (MotorADJ.OpenLoopSetUpCNT == 0) //电机启动之后，进行速度闭环运算
			{
				MotorADJ.CloseSPLoopCnt++;
				sptr->SetPoint = MotorADJ.Set_Speed;  //设定速度
				sptr->FbkPoint =  MotorADJ.Now_Speed; //反馈速度

				if(MotorADJ.CloseSPLoopCnt < 10)//P环启动
				{
					LocPIDCalc(sptr,0);  //速递闭环PID控制
					MotorADJ.PWM_Float_val = sptr->Fout;
				}
				else                           //O环启动之后，再进行PID闭环
				{
					MotorADJ.CloseSPLoopCnt = 10;
					LocPIDCalc(sptr,1);  //速递闭环PID控制
					MotorADJ.PWM_Float_val = sptr->Fout;
				}
				//PWM赋值输出
				SetPWMVal((Uint16)MotorADJ.PWM_Float_val);
			}
		}
		break;

	case M_CURRRENTLOOP://电流闭环模式
		if (Mark.Status_Bits.MotorState != M_STOP) //电机运行命令使能
		{
			if (MotorADJ.OpenLoopSetUpCNT == 0) //电机开环启动之后，进行电流闭环模式
			{
				Mark.Status_Bits.Idc_Loop_Flag = 1; //在AD中断里做电流闭环控制
			}

			if(Mark.Status_Bits.MotorState == M_ACCDOWN)  //判断电机停止
			{
				if(MotorADJ.Now_Speed < OPEN_DOWM_MOS_SPEED )//下管完全打开的时间点
				{
					Mark.Status_Bits.MotorState = M_STOP;
					openLowBrige();
				}
			}
		}
		else Mark.Status_Bits.Idc_Loop_Flag = 0;

		break;

	default:break;
	}
}

#define TIMEROUTRX 100 //单位ms，等待超时时间
char pwmString[][3]={"NC","12","23","34","45","56","61"};
char motorMode[][16]={"M_IDLE","M_PWM_TEST","M_UVW_HALL_TEST","M_OPENLOOP","M_SPEEDLOOP"};
//###########################################################################
//解析Uart接收函数
//各命令字/功能码 段的字长度不一样
//再串口调试助手的扩展列表中填入功能吗  参数，以便下发
//具体操作请参看《M新动力Motor IPM V2系列 24V BLDC电机开发板之BLDC的七个试验.pdf》
//---------------------------------------------------------------------------
void decodeUart(void)
{
	Uint16  temp1,temp2,tmp;
	double dTmp;

	if(SCI_Msg.Mark_Para.Status_Bits.rFifoDataflag == 1 )
	{
		if(SCI_Msg.timerOut >= RTU_TIMEROUT) 	  //判断接收完成
		{
			SCI_Msg.Mark_Para.Status_Bits.rFifoDataflag = 0;
			SCI_Msg.timerOut = 0;
			SCI_Msg.Mark_Para.Status_Bits.DISRevflag = 1;

			switch(SCI_Msg.rxData[0])
			{
			//3byte
			case 0x01://用于测试通信是否正常，通过数码管显示
				Mark.Status_Bits.UartTestSEGFlag = 1;
				temp1 = SCI_Msg.rxData[1];
				temp2 = SCI_Msg.rxData[2];
				tmp = 1000*((temp1>>4)%10) + 100*((SCI_Msg.rxData[1]&0x0f)%10) + 10*((temp2>>4)%10) + (SCI_Msg.rxData[2]&0x0f)%10;
				TM1650_Send(DIG4,SEG7Table[tmp/1000]);   //DIG4
				TM1650_Send(DIG3,SEG7Table[tmp/100%10]); //DIG2
				TM1650_Send(DIG2,SEG7Table[(tmp/10)%10]);//DIG3
				TM1650_Send(DIG1,SEG7Table[tmp%10]);     //DIG4
				break;

				//1byte
			case 0x02:
				Mark.Status_Bits.UartTestSEGFlag = 0;//关闭串口测试数码管
				break;

			case 0x03:
				if(Mark.Status_Bits.MotorState == M_STOP)//电机停止时，按下S1则启动电机
				{
					Mark.Status_Bits.MotorState = M_RUN;

					MotorADJ.MoniSpeedZeroCNT = 0;
					MotorADJ.OpenLoopSetUpCNT = 10;//开环启动时间ms，在定时器0中进行递减

					MotorADJ.PWM_OpenVal = START_PWM_ZKB;//设置开环启动的占空比
					MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;
					HallEncode.HallState = ReadHall();//读取电机霍尔位置
					MOS_HX(&HallEncode.HallState);    //输出PWM以启动电机
				}
				else if(Mark.Status_Bits.MotorState == M_RUN)//电机运行中，则先进入减速状态
				{
					Mark.Status_Bits.MotorState = M_ACCDOWN;
				}
				break;

			case 0x04:
				if(Mark.Status_Bits.MotorState == M_STOP)//控制反向
				{
					if(HallEncode.HallType == M_CCW)
					{
						HallEncode.HallType = M_CW;
						D400LOW();
					}
					else
					{
						HallEncode.HallType = M_CCW;//正转/逆时针转
						D400ON();                   //亮，指示正转设置
					}
				}
				break;

			case 0x05://设置开环占空比
				temp1 = SCI_Msg.rxData[1];
				temp2 = SCI_Msg.rxData[2];
				tmp = 1000*((temp1>>4)%10) + 100*((SCI_Msg.rxData[1]&0x0f)%10) + 10*((temp2>>4)%10) + (SCI_Msg.rxData[2]&0x0f)%10;
				CLAMP(tmp,10,1000);
				OpenLoopPWMval=tmp;
				break;

				//1byte
			case 0xA0:
				if(Mark.Status_Bits.MotorState == M_STOP)
				{
					Mark.Status_Bits.TestMode = M_HALL_PRINTF;
					sciPutString(1,"电机霍尔传感器的输出测试，用手顺时针、逆时针旋转电机几圈，并记录数值", 1);
					//经过测试：顺时针132645，逆时针154623
				}
				break;

				//7byte 速度PID参数给定
			case 0xB1:
				temp1 = SCI_Msg.rxData[1];
				dTmp = (temp1<<8) + SCI_Msg.rxData[2];
				sptr->Proportion = dTmp*0.0001; //比例常数 Proportional Const

				temp1 = SCI_Msg.rxData[3];
				dTmp = (temp1<<8) + SCI_Msg.rxData[4];
				sptr->Integral =   dTmp*0.0001; //积分常数 Integral Const

				temp1 = SCI_Msg.rxData[5];
				dTmp = (temp1<<8) + SCI_Msg.rxData[6];
				sptr->Derivative =  dTmp*0.0001;//微分常数 Derivative Const

				break;

				//7byte 电流PID参数给定
			case 0xB2:
				temp1 = SCI_Msg.rxData[1];
				dTmp = (temp1<<8) + SCI_Msg.rxData[2];
				iptr->Proportion = dTmp*0.0001; //比例常数 Proportional Const

				temp1 = SCI_Msg.rxData[3];
				dTmp = (temp1<<8) + SCI_Msg.rxData[4];
				iptr->Integral =   dTmp*0.0001; //积分常数 Integral Const

				temp1 = SCI_Msg.rxData[5];
				dTmp = (temp1<<8) + SCI_Msg.rxData[6];
				iptr->Derivative =  dTmp*0.0001;//微分常数 Derivative Const
				break;

				//2byte  切换模式（仅限在电机停止时，对此操作 ）
				//					  M_OPENLOOP     = 3,//开环运行模式
				//					  M_SPEEDLOOP    = 4,//速度闭环模式
				//					  M_CURRRENTLOOP = 5,//电流闭环模式
			case 0xC1:
				if(Mark.Status_Bits.MotorState == M_STOP)
				{
					if((SCI_Msg.rxData[1]>=3)||(SCI_Msg.rxData[1]<=5))
						Mark.Status_Bits.TestMode = SCI_Msg.rxData[1];
				}
				break;

			default:break;
			}

			SCI_Msg.rxWriteIndex = 0;
			SCI_Msg.rxReadIndex = 0;
			SCI_Msg.Mark_Para.Status_Bits.DISRevflag = 0;
		}
	}
}


