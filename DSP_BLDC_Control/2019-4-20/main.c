/*
 * author:zst
 * version:12_3
 */
#include "DSP28x_Project.h"
#include "HK_all_include.h"
#include "C28x_FPU_FastRTS.h"
#include <math.h>

void InitLED(void); //��ʼ������D400  D401 2��LEDָʾ��
void InitKEY(void); //��ʼ������S1����
void scanKey(void); //���� ������������������/ֹͣ������������ʾ�����л��Ȳ���
void InitADC(void);
void InitI2C(void);
void InitpwmINT(void);

extern int tempIsum;
// Instance a QEP interface driver


void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
	while(SourceAddr < SourceEndAddr)
		*DestAddr++ = *SourceAddr++;
	return;
}

typedef struct {
	float ElecTheta;    //OutPut: ��Ƕ�
	float MechTheta;    //Output: ��е�Ƕ�
	int DirectionQep; //output: ���ת��

	int PolePairs;     //���������
	int LineEncoder;   //����һ��������������ʽ��
	int Encoder_N;     //����һ����������4��(���ݱ�Ƶ�ı���������������4��Ƶ)
	int CalibrateAngle;//���A�����������Index�ź�֮��ļнǣ��밲װ�����й�
	float Mech_Scaler;   //1/Encoder_N
	float RawTheta;      //��ʼ��λ�󣬵��ת��d��Ͷ���A������֮�����������̼���ֵ
	int Index_sync_flag;//����ͬ���źű�־λ

	float BaseRpm;        //�ת��

	float Speed_Mr_Rpm_Scaler;//Parameter:60000/(Encoder_N * T)������TΪM������ʱ��ʱ��������λms T=100
	float Speed_Mr_Rpm;   //Output: speed int r.p.m
	float Speed_Mr;       //Output: speed in per-uint
	float Position_k_1;   //Input: Current position
	float Position_k;     //Input: Last position

	float Speed_Tr_Rpm_Scaler;//(UPPS * 150e6 * 60)/(Encoder_N * CCPS)
	float Speed_Tr_Rpm;   //Output: spedd int r.p.m
	float Speed_Tr;       //Output: speed int per-uint

	void (*init)();         //QEP��ʼ������
	void (*calc)();			//QEP���㺯��
} EQEP_POS_SPEED_GET;
typedef EQEP_POS_SPEED_GET  *EQEP_POS_SPEED_GET_handle;
#define EQEP_POS_SPEED_GET_DEFAULTS {0,0,0,\
		12,15744,62976,0,1.0/62976,0,0,\
		50.0,\
		0.009527,0,0,0,0,\
		35727.87,0,0,\
		(void (*)(long))eQEP_pos_speed_get_Init,\
		(void (*)(long))eQEP_pos_speed_get_Calc}

void eQEP_pos_speed_get_Init(EQEP_POS_SPEED_GET_handle);
void eQEP_pos_speed_get_Calc(EQEP_POS_SPEED_GET_handle);

EQEP_POS_SPEED_GET myPos=EQEP_POS_SPEED_GET_DEFAULTS;

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
	//Gpio_IPM_Init();

	//ʹ��IPM���ţ��߼��͵�ƽʹ�ܣ��������ϵ�IPM��ͨ��NPN�����ܿ��Ƶ�
	//oENSET();

//	InitKEY();//������ʼ�� S1
//	InitLED();//LED��ʼ�� D400 D401
	D400ON();
	D400LOW();
	D400ON();

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
	//InitI2C();

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



//	eQEP_pos_speed_get_Init(&myPos);
//	eQEP_pos_speed_get_Calc(&myPos);

	//eQepģ��
	EQep1Regs.QUPRD=1500000;
	EQep1Regs.QDECCTL.bit.QSRC=00;		// QEP quadrature count mode

	EQep1Regs.QEPCTL.bit.FREE_SOFT=2;
	EQep1Regs.QEPCTL.bit.PCRM=00;		// PCRM=00 mode - QPOSCNT reset on index event
	EQep1Regs.QEPCTL.bit.UTE=1; 		// Unit Timeout Enable
	EQep1Regs.QEPCTL.bit.QCLM=1; 		// Latch on unit time out
	EQep1Regs.QPOSMAX=0xffffffff;
	EQep1Regs.QEPCTL.bit.QPEN=1; 		// QEP enable

	EQep1Regs.QCAPCTL.bit.UPPS=5;   	// 1/32 for unit position
	EQep1Regs.QCAPCTL.bit.CCPS=7;		// 1/128 for CAP clock
	EQep1Regs.QCAPCTL.bit.CEN=1; 		// QEP Capture Enable

	//���״̬���������ʼ��
	Mark.All=0;

	//M_OPENLOOP���ٶȿ���ģʽ��M_SPEEDLOOP�ٶȱջ�ģʽ��M_CURRRENTLOOP�����ջ�ģʽ
	Mark.Status_Bits.TestMode = M_OPENLOOP ;

	//�趨���з�����ʱ�� ��ת
	HallEncode.HallType = M_CCW;



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
			////���������
			//			sciPutString(1,"Isum=",0);
			//			send_number_to_USART(intAD_Vol[0]);
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

			}
		}

		//2����Ҫ�ĵ������ ������
		//    	MotorControl(); ��timer0_base.c �Ķ�ʱ��0�ж�
		//    	main_isr();     ePWM.c  �� 20K PWM�жϷ�����
		decodeUart();//���ڽ�������PC���͵�����

		if(Mark.Status_Bits.DrawLineFlag ==1 )
			DrawLine();  //�����ϴ����������ͼ�����ʹ�� ���������ڵ��ã�
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
	D400LOW();//�ر�D400
	D401LOW();//�ر�D401
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
void eQEP_pos_speed_get_Init(EQEP_POS_SPEED_GET *p)
{
	EQep1Regs.QUPRD=1500000;          // Unit Timer for 100Hz at 150 MHz SYSCLKOUT

	EQep1Regs.QDECCTL.bit.QSRC=00;      // QEP quadrature count mode

	EQep1Regs.QEPCTL.bit.FREE_SOFT=2;
	EQep1Regs.QEPCTL.bit.PCRM=00;       // PCRM=00 mode - QPOSCNT reset on index event
	EQep1Regs.QEPCTL.bit.UTE=1;         // Unit Timeout Enable
	EQep1Regs.QEPCTL.bit.QCLM=1;        // Latch on unit time out
	EQep1Regs.QEPCTL.bit.QPEN=1;        // QEP enable

	EQep1Regs.QCAPCTL.bit.UPPS=5;       // 1/32 for unit position
	EQep1Regs.QCAPCTL.bit.CCPS=7;       // 1/128 for CAP clock
	EQep1Regs.QCAPCTL.bit.CEN=1;        // QEP Capture Enable

	EQep1Regs.QPOSMAX=p->Encoder_N;     // Encoder_N

	// EQep1Regs.QPOSINIT=EQep2Regs.QPOSMAX - p->CalibrateAngle;

	EQep1Regs.QEPCTL.bit.SWI=1;// software generate index pulse
	//  InitEQep2Gpio();
	\
	EALLOW;                       /* Enable EALLOW*/											\
	GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 1;  /* GPIO20 is EQep2A */								\
	GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 1;  /* GPIO21 is EQep2B */								\
	GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 1;  /* GPIO23 is EQep2I  */								\
	EDIS;
}
//**********************************
/*
  @ Description:
  @ Param
  @ Return
 */
//**********************************
void eQEP_pos_speed_get_Calc(EQEP_POS_SPEED_GET *p)
{
	float tmp1;
	unsigned long t2_t1;
	//Check the rotational direction
	p->DirectionQep = EQep1Regs.QEPSTS.bit.QDF;

	//Check the position counter for EQep2
	p->RawTheta = EQep1Regs.QPOSCNT + p->CalibrateAngle;
	if(p->RawTheta < 0)
	{p->RawTheta = p->RawTheta + EQep1Regs.QPOSMAX; }
	else if(p->RawTheta > EQep1Regs.QPOSMAX)
	{p->RawTheta = p->RawTheta - EQep1Regs.QPOSMAX; }
	//Compute the mechanical angle
	p->MechTheta = p->Mech_Scaler * p->RawTheta;
	//Compute the electrical angle
	p->ElecTheta = (p->PolePairs * p->MechTheta)-floor(p->PolePairs * p->MechTheta);
	// Check an index occurrence
	if (EQep1Regs.QFLG.bit.IEL == 1)
	{
		p->Index_sync_flag = 0x00F0;
		EQep1Regs.QCLR.bit.IEL=1;                   // Clear interrupt flag
	}
	// High Speed Calcultation using QEP Position counter //M�ڧ�
	// Check unit Time out-event for speed calculation:
	// Unit Timer is configured for 100Hz in INIT function
	if(EQep1Regs.QFLG.bit.UTO==1)// If unit timeout (one 100Hz period)
	{
		p->Position_k =1.0 * EQep1Regs.QPOSLAT;
		if(p->DirectionQep==0) // POSCNT is counting down
		{
			if(p->Position_k > p->Position_k_1)
			{ tmp1 = -(p->Encoder_N - (p->Position_k - p->Position_k_1)); }
			else
			{ tmp1 = p->Position_k - p->Position_k_1;}// x2-x1 should be negative
		}
		else if(p->DirectionQep==1)    // POSCNT is counting up
		{
			if(p->Position_k < p->Position_k_1)
			{ tmp1 = p->Encoder_N - (p->Position_k_1 - p->Position_k); }
			else
			{ tmp1 = p->Position_k - p->Position_k_1;}// x2-x1 should be positive
		}

		if(tmp1 > p->Encoder_N)
		{ p->Speed_Mr_Rpm = p->BaseRpm;  }
		else if(tmp1 < -p->Encoder_N)
		{ p->Speed_Mr_Rpm = -p->BaseRpm; }
		else
		{ p->Speed_Mr_Rpm = tmp1 * p->Speed_Mr_Rpm_Scaler; }

		p->Speed_Mr = p->Speed_Mr_Rpm / p->BaseRpm;

		p->Position_k_1 = p->Position_k;

		EQep1Regs.QCLR.bit.UTO=1; // Clear interrupt flag
	}
	// Low-speed computation using QEP capture counter // T�ڧ�
	if(EQep1Regs.QEPSTS.bit.UPEVNT==1)
	{
		if(EQep2Regs.QEPSTS.bit.COEF==0)
			t2_t1 =  EQep1Regs.QCPRDLAT;
		else
			t2_t1 = 0xFFFF;

		if(p->DirectionQep==0)
			p->Speed_Tr_Rpm = -p->Speed_Tr_Rpm_Scaler / t2_t1;
		else
			p->Speed_Tr_Rpm =  p->Speed_Tr_Rpm_Scaler / t2_t1;

		if(p->Speed_Tr_Rpm > p->BaseRpm)
			p->Speed_Tr_Rpm = p->BaseRpm;
		else if(p->Speed_Tr_Rpm < -p->BaseRpm)
			p->Speed_Tr_Rpm = -p->BaseRpm;

		EQep1Regs.QEPSTS.all=0x88;
	}
}

