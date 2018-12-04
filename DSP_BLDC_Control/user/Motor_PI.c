
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

Uint16 hallFindCnt=0;//����HALL��UVW��λ��ϵ�� �������PWMʱ��/ ms��λ
int iResVal;        //�ɵ����������ADֵ

//�����ϴ���2���������
Uint16 CRCcode;
char CmdT[20];

DEF_Motor_REG  MotorADJ={0}; //������������ر���

//###########################################################################
//CapValΪ�����Cap���Ų���ļ�����
//2���ԣ���еһȦ��2�������ź�
//---------------------------------------------------------------------------
//�ٶ� RPM =  (SYSCLK/POLES/CapVal)x60�� POLES����������ΪһȦPOLES�����������

//###########################################################################
//������ɵ�������������ת��Ϊ �趨�ٶȡ��趨���� ��Ҳ���Ǳջ�PID�ĸ����ο���
//---------------------------------------------------------------------------
void RgSetRef(void)
{
	static int temp1=0,temp2=0,temp3=0;
	if((Mark.Status_Bits.TestMode == M_SPEEDLOOP)||(Mark.Status_Bits.TestMode ==M_CURRRENTLOOP))
	{
		iResVal = intAD_Vol[AD_RG]*0.909;  //28335��ADC����3V���� ����3.3V
		if(Mark.Status_Bits.MotorState != M_ACCDOWN)
		{
			temp1 = iResVal;

			//�趨�����ο�ֵ
			iptr->SetPoint  = iResVal;
			CLAMP(iptr->SetPoint,MIN_IDC,MAX_IDC);

			//�趨�ٶȲο�ֵ
			temp3 = temp1 - temp2 ;
			if((temp3>50)||(temp3<-50))//���ڻ���С�� 50 �ķ�Χʱ�������趨�ٶȵĵ���
			{
				temp2 = temp1;
				MotorADJ.Set_Speed = temp1 -temp1 % 50;//�趨�ٶȵ�ȡ��
			}
		}
		else  //���ٹ��̣����òο�ֵ����0
		{
			temp2 =0;
			MotorADJ.Set_Speed = 0;   //����״̬�£��趨�ٶ�Ϊ��
			iptr->SetPoint =0;        //�趨����Ϊ0
		}
	}
	else if(Mark.Status_Bits.TestMode == M_OPENLOOP)//����ģʽ��ռ�ձȵĸ���
	{
		MotorADJ.Set_Speed = intAD_Vol[AD_RG];  //ռ�ձ����Կɵ������ֵ
	}
}

PID sPID;      //�ٶ�PID
PID *sptr = &sPID;

PID currentPID;//����PID
PID *iptr = &currentPID;

//PID ������ʼ��
//0.1  0.001 0.01
void IncPIDInit(void)
{
	sptr->SumError = 0;
	sptr->LastError = 0; //Error[-1]
	sptr->PrevError = 0; //Error[-2]
	sptr->Proportion = 1.5;//0.02;  //�������� Proportional Const
	sptr->Integral = 0.0015;//0.001;   //���ֳ��� Integral Const
	sptr->Derivative = 0.0005;//0.001; //΢�ֳ��� Derivative Const
	sptr->SetPoint = 0;
	sptr->FbkPoint = 0;
	sptr-> Fout= 0;
	sptr-> Umax= MAX_PWM_ZKB;
	sptr-> Umin= 0;

	iptr->SumError = 0;
	iptr->LastError = 0; //Error[-1]
	iptr->PrevError = 0; //Error[-2]
	iptr->Proportion = 2; //�������� Proportional Const
	iptr->Integral = 0.1; //���ֳ��� Integral Const
	iptr->Derivative = 0; //΢�ֳ��� Derivative Const
	iptr->SetPoint = 0;
	iptr->FbkPoint = 0;
	iptr-> Fout= 0;
	iptr-> Umax= MAX_PWM_ZKB;
	iptr-> Umin= 0;
}

//#################################################
//-----------------------------------------------
//λ��ʽ PID ���ƣ�������ﵽ�޷�ʱ�Ի������ƴ����Է�����
//-----------------------------------------------
void LocPIDCalc(PID *ptr,Uint16 flag)
{
	long tepmSumError;

	int iError,dError;
	iError = ptr->SetPoint - ptr->FbkPoint; //ƫ��
	tepmSumError = ptr->SumError;

	if(flag ==1)
	{
		ptr->SumError += iError;                //����
		dError = iError - ptr->LastError;      //΢��
		ptr->LastError = iError;
		//������  + ������ + ΢����/
		ptr-> Fout = (ptr->Proportion * iError + ptr->Integral * ptr->SumError + ptr->Derivative * dError);
	}
	else
	{
		ptr-> Fout = ptr->Proportion * iError *0.2;
	}

	//�޷����
	if(ptr->Fout > ptr->Umax)
	{
		ptr-> Fout = ptr->Umax;
		ptr->SumError = tepmSumError*0.9;//��ֹ���ֱ���
	}
	else if(ptr->Fout < ptr->Umin)
	{
		ptr-> Fout = ptr->Umin;
		ptr->SumError = tepmSumError;
	}
}



//###########################################################################
//�Լ챨������
// �жϻ����������Ƿ�����
// �ж��Ƿ������屨��
//---------------------------------------------------------------------------------------------
void DriverCheck()
{
	Uint16 HallState = ReadHall();//��ȡ����λ��
	//����������δ���롢���󡢿���24V��Դδ����ȣ�DSP�޷���ȷ��ȡ����λ�ã���������ѭ������D401������˸����
	if((HallState==0) || (HallState==7))
	{
		//		showHanZi(0,300,&Hanzi6[0][0],15);//����������

		//�������ʾ0xEE07 ��ʾ ��������������
		TM1650_Send(DIG4, SEG7Table[0x0E]); //GID1
		TM1650_Send(DIG3, SEG7Table[0x0E]); //GID2
		TM1650_Send(DIG2, SEG7Table[0x00]); //GID3
		TM1650_Send(DIG1, SEG7Table[0x07]); //GID4

		//printf("\r\n��������������");
		for(;;)
		{
			DELAY_US(100000);
			D401TOGGLE();
		}
	}

	if(EPwm1Regs.TZFLG.bit.OST==0x1)//IPM TZ�������������� ���������� ��IPM 15V ��Ƿѹ��ѹ
	{
		//		showHanZi(0,300,&Hanzi7[0][0],12);//IPM ����

		//�������ʾ0xEE01 ��ʾTZ����
		TM1650_Send(DIG4, SEG7Table[0x0E]); //GID1
		TM1650_Send(DIG3, SEG7Table[0x0E]); //GID2
		TM1650_Send(DIG2, SEG7Table[0x00]); //GID3
		TM1650_Send(DIG1, SEG7Table[0x01]); //GID4

		//		printf("\r\nIPM TZ������������������ ���������� ��IPM 15V ��Ƿѹ��ѹ");
		for(;;)
		{
			DELAY_US(100000);
			D401TOGGLE();
		}
	}

	//�ɼ���ĸ�� ��ѹǷѹ����
}



//#################################################
//CRC У��
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
//�����ϴ����ݣ���������
//���ͼ�μ�����ʹ�ã� PC��ע�� ͨ���� �� ���ڲ����� 2�������趨
//�ϴ���������ϵ������۲�
//�ϴ���ʽ  �� ����������2��byteΪһ����������λ��ǰ�� ����1 ����2 ���������� + CRC
//-----------------------------------------------
void DrawLine()
{
	int temp;

	CmdT[0]=3;//ͨ����
	scia_xmit(CmdT[0]);//����ͨ����

	//�趨�ٶ�
	HexPrintf(MotorADJ.Set_Speed);//timer0Base.msCounter
	CmdT[1] = (MotorADJ.Set_Speed)>>8;
	CmdT[2] = (MotorADJ.Set_Speed)&0XFF;

	//ʵʱ�ٶ�
	HexPrintf(MotorADJ.Now_Speed);
	CmdT[3] = (MotorADJ.Now_Speed)>>8;
	CmdT[4] = (MotorADJ.Now_Speed)&0XFF;

	//�ٶ�PID�ջ�����������PWM
	temp = sptr->Fout;
	HexPrintf(temp);
	CmdT[5] = (temp)>>8;
	CmdT[6] = (temp)&0XFF;

	//����CRC
	CRCcode = GetCRC16(&CmdT[0],(CmdT[0]<<1)+1);
	HexPrintf(CRCcode);
}
//#################################################
//-----------------------------------------------
//������Ƶļ���ģʽ����  ������
//-----------------------------------------------
void MotorControl(void)
{
	switch(Mark.Status_Bits.TestMode)
	{
	case M_IDLE: //����ģʽ���������¹ܣ��ر������Ϲ�
		MotorADJ.PWM_Val =0;
		openLowBrige();
		break;
	case M_PWM_TEST://PWM�źŹ۲�ģʽ
		SetPWMVal(intAD_Vol[AD_RG]>>1);//�ı����PWM����ռ�ձ�
		break;

	case M_UVW_HALL_TEST://�������ͻ����źŲ���ģʽ
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

	case M_HALL_PRINTF:                       //���ڴ�ӡ����λ�ã�������ʹ�ã�����ȡ����
		printfHall(HallEncode.HallState);//�����źŵ�ֵ��3���ⲿ�жϺ������ȡ
		break;

	case M_OPENLOOP:                                //�ٶȿ���ģʽ(�����ڲ���LBDC����� UVW�ߺͻ�����Ӧ��ϵ)
		if(Mark.Status_Bits.MotorState == M_RUN)//�������й����У�ռ�ձ����Կɵ�����ĸ���
		{
			MotorADJ.PWM_OpenVal =OpenLoopPWMval;  //ռ�ձȸ�ֵ
			CLAMP(MotorADJ.PWM_OpenVal,MIN_PWM_ZKB,MAX_PWM_ZKB);
			MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;	//��ÿ���������︳ֵռ�ձ�

			if( MotorADJ.Now_Speed==0)
			{
				HallEncode.HallState = ReadHall();//��ȡ�������λ��
				MOS_HX(&HallEncode.HallState);    //���PWM�����������
			}
		}
		else if(Mark.Status_Bits.MotorState == M_ACCDOWN)//ռ�ձȼ�С������ͣ��
		{
			if(MotorADJ.PWM_OpenVal) MotorADJ.PWM_OpenVal--;    //ռ�ձȼ�С
			MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;	//��ÿ���������︳ֵռ�ձ�

			if(MotorADJ.PWM_OpenVal==0)//�ٶȽ���һ��ʱ���¹���ȫ�򿪣�����ͣ��
			{
				Mark.Status_Bits.MotorState = M_STOP;
				openLowBrige();
			}
		}

		break;

	case M_SPEEDLOOP://�ٶȱջ�ģʽ
		if (Mark.Status_Bits.MotorState != M_STOP) //�����������ʹ��
		{
			//�������ֹͣ״̬�����е��ֹͣ���ж�
			if(Mark.Status_Bits.MotorState == M_ACCDOWN)
			{
				MotorADJ.PWM_Val++;
				if(MotorADJ.PWM_Val == PWM_FRE )//�ٶȽ���һ��ʱ���¹���ȫ�򿪣�����ͣ��
				{
					Mark.Status_Bits.MotorState = M_STOP;
					openLowBrige();
					sptr->LastError=0;
					sptr->SumError =0;
				}
			}
			else if (MotorADJ.OpenLoopSetUpCNT == 0) //�������֮�󣬽����ٶȱջ�����
			{
				MotorADJ.CloseSPLoopCnt++;
				sptr->SetPoint = MotorADJ.Set_Speed;  //�趨�ٶ�
				sptr->FbkPoint =  MotorADJ.Now_Speed; //�����ٶ�

				if(MotorADJ.CloseSPLoopCnt < 10)//P������
				{
					LocPIDCalc(sptr,0);  //�ٵݱջ�PID����
					MotorADJ.PWM_Float_val = sptr->Fout;
				}
				else                           //O������֮���ٽ���PID�ջ�
				{
					MotorADJ.CloseSPLoopCnt = 10;
					LocPIDCalc(sptr,1);  //�ٵݱջ�PID����
					MotorADJ.PWM_Float_val = sptr->Fout;
				}
				//PWM��ֵ���
				SetPWMVal((Uint16)MotorADJ.PWM_Float_val);
			}
		}
		break;

	case M_CURRRENTLOOP://�����ջ�ģʽ
		if (Mark.Status_Bits.MotorState != M_STOP) //�����������ʹ��
		{
			if (MotorADJ.OpenLoopSetUpCNT == 0) //�����������֮�󣬽��е����ջ�ģʽ
			{
				Mark.Status_Bits.Idc_Loop_Flag = 1; //��AD�ж����������ջ�����
			}

			if(Mark.Status_Bits.MotorState == M_ACCDOWN)  //�жϵ��ֹͣ
			{
				if(MotorADJ.Now_Speed < OPEN_DOWM_MOS_SPEED )//�¹���ȫ�򿪵�ʱ���
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

#define TIMEROUTRX 100 //��λms���ȴ���ʱʱ��
char pwmString[][3]={"NC","12","23","34","45","56","61"};
char motorMode[][16]={"M_IDLE","M_PWM_TEST","M_UVW_HALL_TEST","M_OPENLOOP","M_SPEEDLOOP"};
//###########################################################################
//����Uart���պ���
//��������/������ �ε��ֳ��Ȳ�һ��
//�ٴ��ڵ������ֵ���չ�б������빦����  �������Ա��·�
//���������ο���M�¶���Motor IPM V2ϵ�� 24V BLDC���������֮BLDC���߸�����.pdf��
//---------------------------------------------------------------------------
void decodeUart(void)
{
	Uint16  temp1,temp2,tmp;
	double dTmp;

	if(SCI_Msg.Mark_Para.Status_Bits.rFifoDataflag == 1 )
	{
		if(SCI_Msg.timerOut >= RTU_TIMEROUT) 	  //�жϽ������
		{
			SCI_Msg.Mark_Para.Status_Bits.rFifoDataflag = 0;
			SCI_Msg.timerOut = 0;
			SCI_Msg.Mark_Para.Status_Bits.DISRevflag = 1;

			switch(SCI_Msg.rxData[0])
			{
			//3byte
			case 0x01://���ڲ���ͨ���Ƿ�������ͨ���������ʾ
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
				Mark.Status_Bits.UartTestSEGFlag = 0;//�رմ��ڲ��������
				break;

			case 0x03:
				if(Mark.Status_Bits.MotorState == M_STOP)//���ֹͣʱ������S1���������
				{
					Mark.Status_Bits.MotorState = M_RUN;

					MotorADJ.MoniSpeedZeroCNT = 0;
					MotorADJ.OpenLoopSetUpCNT = 10;//��������ʱ��ms���ڶ�ʱ��0�н��еݼ�

					MotorADJ.PWM_OpenVal = START_PWM_ZKB;//���ÿ���������ռ�ձ�
					MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;
					HallEncode.HallState = ReadHall();//��ȡ�������λ��
					MOS_HX(&HallEncode.HallState);    //���PWM���������
				}
				else if(Mark.Status_Bits.MotorState == M_RUN)//��������У����Ƚ������״̬
				{
					Mark.Status_Bits.MotorState = M_ACCDOWN;
				}
				break;

			case 0x04:
				if(Mark.Status_Bits.MotorState == M_STOP)//���Ʒ���
				{
					if(HallEncode.HallType == M_CCW)
					{
						HallEncode.HallType = M_CW;
						D400LOW();
					}
					else
					{
						HallEncode.HallType = M_CCW;//��ת/��ʱ��ת
						D400ON();                   //����ָʾ��ת����
					}
				}
				break;

			case 0x05://���ÿ���ռ�ձ�
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
					sciPutString(1,"���������������������ԣ�����˳ʱ�롢��ʱ����ת�����Ȧ������¼��ֵ", 1);
					//�������ԣ�˳ʱ��132645����ʱ��154623
				}
				break;

				//7byte �ٶ�PID��������
			case 0xB1:
				temp1 = SCI_Msg.rxData[1];
				dTmp = (temp1<<8) + SCI_Msg.rxData[2];
				sptr->Proportion = dTmp*0.0001; //�������� Proportional Const

				temp1 = SCI_Msg.rxData[3];
				dTmp = (temp1<<8) + SCI_Msg.rxData[4];
				sptr->Integral =   dTmp*0.0001; //���ֳ��� Integral Const

				temp1 = SCI_Msg.rxData[5];
				dTmp = (temp1<<8) + SCI_Msg.rxData[6];
				sptr->Derivative =  dTmp*0.0001;//΢�ֳ��� Derivative Const

				break;

				//7byte ����PID��������
			case 0xB2:
				temp1 = SCI_Msg.rxData[1];
				dTmp = (temp1<<8) + SCI_Msg.rxData[2];
				iptr->Proportion = dTmp*0.0001; //�������� Proportional Const

				temp1 = SCI_Msg.rxData[3];
				dTmp = (temp1<<8) + SCI_Msg.rxData[4];
				iptr->Integral =   dTmp*0.0001; //���ֳ��� Integral Const

				temp1 = SCI_Msg.rxData[5];
				dTmp = (temp1<<8) + SCI_Msg.rxData[6];
				iptr->Derivative =  dTmp*0.0001;//΢�ֳ��� Derivative Const
				break;

				//2byte  �л�ģʽ�������ڵ��ֹͣʱ���Դ˲��� ��
				//					  M_OPENLOOP     = 3,//��������ģʽ
				//					  M_SPEEDLOOP    = 4,//�ٶȱջ�ģʽ
				//					  M_CURRRENTLOOP = 5,//�����ջ�ģʽ
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


