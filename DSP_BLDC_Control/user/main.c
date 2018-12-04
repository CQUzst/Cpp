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

void InitLED(void); //��ʼ������D400  D401 2��LEDָʾ��
void InitKEY(void); //��ʼ������S1����
void scanKey(void); //���� ������������������/ֹͣ������������ʾ�����л��Ȳ���
void InitADC(void);
void InitI2C(void);
void InitpwmINT(void);

extern int tempIsum;

//**************************������*********************************
void main(void) 
{
	//ϵͳ��ʼ��
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

	//IPM���Ƶ�IO��ʼ��
	Gpio_IPM_Init();

	//ʹ��IPM���ţ��߼��͵�ƽʹ�ܣ��������ϵ�IPM��ͨ��NPN�����ܿ��Ƶ�
	oENSET();

	InitKEY();//������ʼ�� S1
	InitLED();//LED��ʼ�� D400 D401
	D400LOW();//�ر�D400
	D401LOW();//�ر�D401

	//��ʼ��SCIA,115200bps�������жϣ�����FIFO���--1�����
	SCI_Init();

	//���Է����ַ���
	sciPutString(1,"start��", 1);
	Uint16 testNumber=14;
	send_number_to_USART(testNumber);

	//���ùٷ��⺯������ʼ��CPU��ʱ��
	InitCpuTimers();

	//��ʱ��0��ʼ������ʼ���ж�1ms����
	Timer0_init();

	//TM1650��ʼ��  IIC����
	InitI2C();

	//DSP PWM��ʼ��
	InitEPWM_Hpwm_Lopen();

	//����TZ�������
	TZ_OSHT_Ctr(1);

	//AD������ʼ��
	InitADC();

	//ӳ��PWM�ж�������ʼ��
	InitpwmINT();

	//�����źŷ�����ʼ���������жϻ���
	ExInt_Init();

	//CAP����ģ���ʼ��
	Motor_Cap_Init();

	//���״̬���������ʼ��
	Mark.All=0;

	//M_OPENLOOP���ٶȿ���ģʽ��M_SPEEDLOOP�ٶȱջ�ģʽ��M_CURRRENTLOOP�����ջ�ģʽ
	Mark.Status_Bits.TestMode = M_OPENLOOP ;

	//�趨���з�����ʱ�� ��ת
	HallEncode.HallType = M_CCW;

	D400ON();                     //D400����ָʾ��ת���ã���ָʾ��ת����

	//PID������������ʼ�����ٶ�/�����ջ�PID��������ֵ�ȳ�ʼ��
	IncPIDInit();

	//ʹ��PIEģ��
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;

	//ʹ��ȫ���ж�
	EINT;
	ERTM;

	//BIT6��BIT4Ϊ���ȵ��ڣ�BIT0��  1 ����/0�ر�
	LigntVal = 0x11;

	//�趨����
	TM1650_Send(CMD_SEG, LigntVal);

	while(1)
	{
		//1ms����
		if( timer0Base.Mark_Para.Status_Bits.OnemsdFlag == 1)
		{
			//			sciPutString(1,"Ic=",0);
			//			send_number_to_USART(intAD_Vol[3]);
			//���������
			sciPutString(1,"Isum=",0);
			send_number_to_USART(intAD_Vol[0]);
			timer0Base.Mark_Para.Status_Bits.OnemsdFlag =0;//�����λ
			//0.2s��200ms������
			if( timer0Base.msCounter > 200)//200
			{
				timer0Base.msCounter =0;
				D401TOGGLE();

				DriverCheck();//������,�жϻ������Ƿ����
				GetIpmTemp(); //����������������ķ�ѹ�����ת��Ϊ�¶�
				RgSetRef();   //������ɵ������趨 �ٶ�/������
				scanKey();	  //ɨ�谴����Ӧ


				//�������ʾ���ݣ������� scanKey() ��������S2 S3 S4
				TM1650_SEG_show(SEG_DIG4_Cmd,*PtrSegType);
				if(Mark.Status_Bits.DrawLineFlag ==1 )
					DrawLine();  //�����ϴ����������ͼ�����ʹ�� ���������ڵ��ã�
			}
		}

		//2����Ҫ�ĵ������ ������
		//    	MotorControl(); ��timer0_base.c �Ķ�ʱ��0�ж�
		//    	main_isr();     ePWM.c  �� 20K PWM�жϷ�����

		decodeUart();//���ڽ�������PC���͵�����
	}
}									








//��ʼ��LED   D400  GPIO8/PWM5A   ���	 D401  GPIO10/PWM6A  ���
void InitLED()
{
	EALLOW;
	D400MUX = 0;//GPIO����
	D400DIR = 1;//���
	D401MUX = 0;
	D401DIR = 1;
	EDIS;
}

//��ʼ���������� S100 ���ӵ�GPIO34
void InitKEY()
{
	EALLOW;
	S100MUX = 0;//GPIO����
	S100DIR = 0;//����
	S100PUD = 1;//���ڿ������������������裬����DSP�ڲ�����
	EDIS;
}
void InitADC()
{
	//P203�ϵ�ADC�ӿ�
	ChSel[0]=2;		// A2--->IfbSum
	ChSel[1]=6;		// A6 -> RG
	ChSel[2]=0;		// A0 -> IPM V_Tsens
	//	ChSel[1]=1;		// ChSelect: ADC A1-> Phase A Current
	//	ChSel[2]=9;		// ChSelect: ADC B1-> Phase B Current
	ChSel[3]=3;		// ChSelect: ADC A3-> Phase C Current�������ֻ��һ��
	ChSel[4]=15;	// ChSelect: ADC B7-> Phase A Voltage
	ChSel[5]=14;	// ChSelect: ADC B6-> Phase B Voltage
	ChSel[6]=12;	// ChSelect: ADC B4-> Phase C Voltage
	ChSel[7]=7;		// ChSelect: ADC A7-> DC Bus  Voltage
	//	ChSel[8]=6;		// A6 -> RG
	//	ChSel[9]=0;		// A0 -> IPM V_Tsens


	ADC_MACRO_INIT_Dual(ChSel,ACQPS);//nͨ��ADC��ʼ����MotorA
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;     /* Enable SOCA */
	/* �ж�����--��ҪA:PWM�м俪��������ADC�ɼ� Enable period event for SOCA */
	EPwm1Regs.ETSEL.bit.SOCASEL = 2;
	EPwm1Regs.ETPS.bit.SOCAPRD = 1;     /* Generate SOCA on the 1st event */
	EPwm1Regs.ETCLR.bit.SOCA = 1;       /* Clear SOCA flag */
}

//��ʼ��I2C
void InitI2C()
{
	softResetIIC_BUS(); //�����λIIC���豸
	InitI2C_Gpio(); //io ��ʼ��ΪIIC
	I2CA_Init();    //HW IIC��ʼ����100KHz
	PtrSegType = SEG_Show_Type[0];//�����Ĭ����ʾ������ ---�趨�ٶ�
}

//��ʼ��pwm�ж�
void InitpwmINT()
{
	EALLOW;
	PieVectTable.EPWM1_INT = &main_isr;
	EDIS;

	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
	// Enable EPWM1INT generation
	EPwm1Regs.ETSEL.bit.INTEN = 1;
	// PWM TB����Ϊ�� �����ж� Enable interrupt CNT_zero event
	EPwm1Regs.ETSEL.bit.INTSEL = 1;
	// ���ٸ�PWM���ڽ���һ���ж� Generate interrupt on the 1st event
	EPwm1Regs.ETPS.bit.INTPRD = 1;
	// Enable more interrupts
	EPwm1Regs.ETCLR.bit.INT = 1;
	IER |= M_INT3;

}
/************************************************************************/
//ɨ���ȡ������������
//����IO���� S1   GPIO34
//ɨ�谴��S2--S6 ����TM1650ɨ��
//���Թ��ܣ�
//  S1  ���--������ֹͣ;
//  S2  �������ʾ--�趨�ٶ�;
//  S3  �������ʾ--ʵʱ�ٶ�;
//  S4  �������ʾ--IPM�¶ȣ�����IR2136������Ļ���MOS����Χ�����¶ȣ�;
//  S5  ���--������ֹͣʱ���ı���ת������D400--��--��ת��  ��--��ת
//  S6  �������߹رմ��� �ϴ���������Ҫ��ϴ���ͼ�����ʹ�ã�
/************************************************************************/
Uint16 S1Flag=0;
void scanKey(void)
{
	//	char TM1650KeyVal[0x44,0x4c,0x54,0x5c,0x64,0x6c,0x74];
	if((rS100DAT()==0)  && (S1Flag==0))//S1��һ�£����õ����������ֹͣ
	{
		S1Flag=1;
		//printf("\r\nS1");
		if(Mark.Status_Bits.MotorState == M_STOP)//���ֹͣʱ������S1���������
		{
			//printf("\r\n-->���� �������");

			Mark.Status_Bits.MotorState = M_RUN;

			MotorADJ.CloseSPLoopCnt=0;

			MotorADJ.MoniSpeedZeroCNT = 0;
			MotorADJ.OpenLoopSetUpCNT = 10;//��������ʱ��ms���ڶ�ʱ��0�н��еݼ�

			MotorADJ.PWM_OpenVal = START_PWM_ZKB;//���ÿ���������ռ�ձ�
			MotorADJ.PWM_Val = PWM_FRE - MotorADJ.PWM_OpenVal ;
			HallEncode.HallState = ReadHall();//��ȡ�������λ��
			MOS_HX(&HallEncode.HallState);    //���PWM���������
		}
		else if(Mark.Status_Bits.MotorState == M_RUN)//��������У����Ƚ������״̬
		{
			//printf("\r\n-->���� ֹͣ���");
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
	//---------------TM1650��ȡɨ�谴��S2-S5����--------------------
	TM1650_Read(CMD_KEY,&keyVal);
	//S2 �ӵ�SEGA��DIG1��  <TM1650.pdf>  P4 ����ɨ����
	if(keyVal == S2_CODE)
	{
		PtrSegType = SEG_Show_Type[0];
		SEG_DIG4_Cmd =0;
	}
	//S3�ӵ�SEGB��DIG1��
	if(keyVal == S3_CODE)
	{
		PtrSegType = SEG_Show_Type[1];
		SEG_DIG4_Cmd = 0;
	}
	//S4�ӵ�SEGC��DIG1��
	if(keyVal == S4_CODE)
	{
		PtrSegType = SEG_Show_Type[2];
		SEG_DIG4_Cmd =0x0F;
	}
	//S5�ӵ�SEGD��DIG1��
	if(keyVal == S5_CODE)
	{
		if(Mark.Status_Bits.MotorState == M_STOP)
		{
			if(HallEncode.HallType == M_CCW)
			{
				HallEncode.HallType = M_CW;//��ת/˳ʱ��ת
				D400LOW();                 //��ָʾ��ת����
			}
			else
			{
				HallEncode.HallType = M_CCW;//��ת/��ʱ��ת
				D400ON();                   //����ָʾ��ת����
			}
		}
	}
	//S6�ӵ�SEGE��DIG1��
	if(keyVal == S6_CODE)
	{
		Mark.Status_Bits.DrawLineFlag ^=1 ;//�������߹ر� �����ϴ�
	}
}


