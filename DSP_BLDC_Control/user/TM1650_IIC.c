/*
 *  TM1650_IIC.c
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

char keyVal,LigntVal;
int  *SEG_Show_Type[]={&MotorADJ.Set_Speed,&MotorADJ.Now_Speed,&MotorADJ.MosFETtempI};//�������ʾ����
int  *PtrSegType = 0;
char SEG_DIG4_Cmd=0; //DIG4������ģ� LED��ʾ����
char keyReg=0;

__interrupt void i2c_int1a_isr(void);

//Uint16 IntSRC[10];            //�ж�Դ            ������
//Uint16 IntSRCCnt;             //�ж�Դ��¼  ������
//Uint16 MasterStatus;			//������������IIC״̬
//Uint16 SlavePHYAddress;	    //IIC�����ַ��Ӳ����ַ��
//Uint16 LogicAddr;             //������EEPROM����Ҫ�ṩ�߼���ַ
//Uint16 Len;			        //�������ݵĳ��ȣ����������ַ��
//Uint16 MsgInBuffer[I2C_MAX_BUFFER_SIZE]; //�������飬���4�����
//Uint16 MsgOutBuffer[I2C_MAX_BUFFER_SIZE];//�������飬���4�����
//Uint16 IIC_TimerOUT;          //�������ĳ�ʱ����

I2CSlaveMSG ICF8591Msg[]={
		{
			{0},
			0,
			IIC_IDLE,
			0,
			0,
			0,
			{0},
			{0},
			0
		}
};


// Interrupt Source Messages
//#define I2C_NO_ISRC             0x0000
//#define I2C_ARB_ISRC            0x0001
//#define I2C_NACK_ISRC           0x0002
//#define I2C_ARDY_ISRC           0x0003
//#define I2C_RX_ISRC             0x0004
//#define I2C_TX_ISRC             0x0005
//#define I2C_SCD_ISRC            0x0006
//#define I2C_AAS_ISRC            0x0007

//�ж�Դע��
char *IIC_ISR_String[] = {"No","ARB",
		        "NACK","ARDY",
		        "RX","Tx",
		        "SCD","AAS"};


I2CSlaveMSG TM1650;//����һ��TM1650�ṹ��
I2CSlaveMSG AT24C02Msg;
I2CSlaveMSG *PtrMsg[]={&TM1650,&AT24C02Msg};//����һ��ָ�����飬���I2C�ṹ�壬�������TM1650��EEPROM

uint8_t SEG7Table[]={

    //������
    0x3f,0x06,0x5b,0x4f,
    0x66,0x6d,0x7d,0x07,
    0x7f,0x6f,0x77,0x7c,
    0x39,0x5e,0x79,0x71

    //������
//    0xc0,0xf9,0xa4,0xb0,
//    0x99,0x92,0x82,0xf8,
//    0x80,0x90,0x88,0x83,
//    0xc6,0xa1,0x86,0x8e,
};


/************************************************************************/
//IIC ��IO�ڶ���
/************************************************************************/
void InitI2C_Gpio()
{
   EALLOW;
	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)

	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
	GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation
    EDIS;
}


//IIC�жϺ������ж�ԴΪP32 IIC�ֲ�˵��
__interrupt void i2c_int1a_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}


#define IICFRE 100000L   //100K IICƵ��
/************************************************************************/
// Initialize I2C
//100K
//ʹ��FIFO
//�������ж�
/************************************************************************/
void I2CA_Init(void)
{
   I2caRegs.I2CMDR.all &=~ 0x0020;	// Take I2C  reset
   DELAY_US(15);//��ʱ������λus

   I2caRegs.I2CPSC.all = 14;		// Prescaler - need 7-12 Mhz on module clk (150/15 = 10MHz)
   I2caRegs.I2CMDR.all = 0x0020;	// Take I2C out of reset

   I2caRegs.I2CCLKL = SYSCLK/(I2caRegs.I2CPSC.all+1)/IICFRE/2-10;// NOTE: must be non zero
   I2caRegs.I2CCLKH = SYSCLK/(I2caRegs.I2CPSC.all+1)/IICFRE/2-5;// NOTE: must be non zero

   //P21 ��sprufz9d--IIC.pdf���ж�Դ
   //P28  I2CIER�Ĵ���˵��
//   I2caRegs.I2CIER.all = 0x22;		// Enable SCD & ARDY interrupts & NACK

   I2caRegs.I2CFFTX.all = 0x6000;	// Enable FIFO mode and TXFIFO
   I2caRegs.I2CFFRX.all = 0x2040;	// Enable RXFIFO, clear RXFFINT,

   EALLOW;	// This is needed to write to EALLOW protected registers
   PieVectTable.I2CINT1A = &i2c_int1a_isr;
   EDIS;   // This is needed to disable write to EALLOW protected registers

// Enable I2C interrupt 1 in the PIE: Group 8 interrupt 1
//   PieCtrlRegs.PIEIER8.bit.INTx1 = 1;

// Enable CPU INT8 whih is connected to PIE group 8
   IER |= M_INT8;
   EINT;

   return;
}



//IIC�ײ㷢�ͺ���
//IIC Maste Mode������ģʽ
//I2caRegs.I2CCNT Ϊ���͵��ֽڳ��ȣ��������������������ַ
//���͸�ʽ��START ADDRESS CONTROL_BYTE DA_BYTE STOP
//��д��I2caRegs.I2CMDR.all = 0x2E20��Ӳ��IIC���в�����STOP�źŽ���
Uint16 I2CA_Tx_STOP(struct I2CSlaveMSG *msg)
{
	Uint16 i;
	if(I2caRegs.I2CMDR.bit.STP==1)//�ȴ�STOP�ź�
	{
	  return I2C_STP_NOT_READY_ERROR;
	}
    if (I2caRegs.I2CSTR.bit.BB == 1)
    {
       return I2C_BUS_BUSY_ERROR;
    }

    I2caRegs.I2CSAR = msg->SlavePHYAddress;//���÷��ʴ�������ַ�������ַ

    //FIFOΪ4���ֽڣ����������� 4�� ����
    for(i=0;i<msg->Len;i++)     // С�ڵ���4��byte������
    {
    	I2caRegs.I2CDXR = msg->MsgOutBuffer[i];
    }
 	I2caRegs.I2CCNT = msg->Len; //���÷��ͳ��ȣ����Ϊ4��byte
    I2caRegs.I2CMDR.all = 0x2E20;		// Send restart as master receiver
    //TRX  = 1 ,����ģʽ�� STP = 1 ����ֹͣλ
    return I2C_SUCCESS;
}


//IIC�ײ���պ���
//�ȷ��ʹ������������ַ���ٽ��� I2CCNT ���ֽ�
//IIC Maste Mode������ģʽ
//I2caRegs.I2CCNT Ϊ���͵��ֽڳ��ȣ��������������������ַ��
//��ʽ(����4���ֽ�Ϊ��)��START ADDRESS DATA_BYTE1 DATA_BYTE2 DATA_BYTE3 DATA_BYTE4  STOP //DATAΪ��ȡ������
//��д��I2caRegs.I2CMDR.all = 0x2C20��Ӳ��IIC���в�����Ӳ��STOP���ͺ󣬽��յ�������FIFO��
Uint16 I2CA_Rxdata_STOP(struct I2CSlaveMSG *msg)
{
	I2caRegs.I2CSAR = msg->SlavePHYAddress;
 	I2caRegs.I2CCNT = msg->Len;	      // Setup how many bytes to expect
    I2caRegs.I2CMDR.all = 0x2C20;		      //  master receiver
    //TRX  = 0 ,����ģʽ�� STP = 1 ����ֹͣλ
    return I2C_SUCCESS;
}


//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______

/************************************************************************/
//�����λ IIC ���豸
//AT24C02 �������ֲ�P6      Figure 6-1. Software reset
// START + 9CLK(SDA=1) + START +��STOP
//�������֮��ʹ��IO��ζIIC����
/************************************************************************/
#define DELAY_FOR 10     // 60Mhz  43.5K
void softResetIIC_BUS()
{
	Uint16 i,j;
EALLOW;
	SDA_H();
	SDAMUX = 0;
	SDADIR = 1;

	SDA_L();
	CLKMUX = 0;
	CLKDIR  =1;
EDIS;

	SDA_H();for(j=0;j<DELAY_FOR;j++);
	CLK_H();for(j=0;j<DELAY_FOR;j++);
	SDA_L();for(j=0;j<DELAY_FOR;j++);
	CLK_L();for(j=0;j<DELAY_FOR;j++);
	SDA_H();for(j=0;j<DELAY_FOR;j++);

    for(i=0;i<9;i++)
    {
    	CLK_H();for(j=0;j<DELAY_FOR;j++);
    	CLK_L();for(j=0;j<DELAY_FOR;j++);
    }

    CLK_H();for(j=0;j<DELAY_FOR;j++);
    SDA_L();for(j=0;j<DELAY_FOR;j++);
    CLK_L();for(j=0;j<DELAY_FOR;j++);
    CLK_H();for(j=0;j<DELAY_FOR;j++);
    SDA_H();for(j=0;j<DELAY_FOR;j++);
    CLK_L();for(j=0;j<DELAY_FOR;j++);

    //�����λ���֮��Ҫ���³�ʼ��IIC IO ΪIIC Ӳ��IO�� ����
    //InitI2C_Gpio();
}



/************************************************************************/
//TM1650���������оƬ���ͺ���
//�ɲο�  TM1650.pdf ��P5 һ��������дʱ��
//���͸�ʽ��START CMD1 DATA1 STOP
/************************************************************************/
void TM1650_Send(char addr,char data)
{
	TM1650.Len  = 1; //1 ������(���������ַ)
	TM1650.SlavePHYAddress = addr;
	TM1650.MsgOutBuffer[0] = data;//TM1650.pdf P4   8������ ������ʾ
	I2CA_Tx_STOP(&TM1650);//�������ݣ���STOP
	TM1650.IIC_TimerOUT = 0;
	TM1650.MasterStatus = IIC_WRITE;
	while((I2caRegs.I2CSTR.all & I2C_SCD_BIT)==0)
	{
		if(TM1650.IIC_TimerOUT > TM1650_TIMER_OUT)//��ʱ�ж�
		{
			softResetIIC_BUS();	                 //�����λIIC���豸
			TM1650.MasterStatus = IIC_IDLE;
			I2caRegs.I2CMDR.bit.STP = 1;
			I2caRegs.I2CSTR.all = 0xFFFF;
			break;
		}
	}
//	TM1650.MasterStatus = IIC_IDLE;
	I2caRegs.I2CSTR.all |= I2C_SCD_BIT;//�����־λ
	DELAY_US(5);//��ʱ������λus
}


/************************************************************************/
//TM1650���������оƬ��ȡ��������
//�ɲο�  TM1650.pdf ��P4 ����ɨ�����ð���;
//P3 �ṩ�˶�����ʱ��
//���͸�ʽ��START CMD1 KEYDATA STOP
/************************************************************************/
void TM1650_Read(char addr, char *data)
{
	TM1650.Len  = 1; //��Ҫ��ȡ�����ݸ�����1 ��
	TM1650.SlavePHYAddress = addr;
	I2CA_Rxdata_STOP(&TM1650);//�������ݣ���STOP
	TM1650.IIC_TimerOUT = 0;
	TM1650.MasterStatus = IIC_READ_STEP2;
	while((I2caRegs.I2CSTR.all & I2C_SCD_BIT)==0)
	{
		if(TM1650.IIC_TimerOUT > AT24CO2_TIMER_OUT)//��ʱ�ж�
		{
			softResetIIC_BUS();	                 //�����λIIC���豸
			TM1650.MasterStatus = IIC_IDLE;
			I2caRegs.I2CMDR.bit.STP = 1;
			I2caRegs.I2CSTR.all = 0xFFFF;
//			printf("\r\n Timer OUT in %d , Soft Reset",TM1650.MasterStatus);
			break;
		}
	}

	I2caRegs.I2CSTR.all |= I2C_SCD_BIT;//�����־λ

	TM1650.IIC_TimerOUT = 0;
	TM1650.MasterStatus = IIC_READ_STEP3;
	while(I2caRegs.I2CFFRX.bit.RXFFST)//��ȡ���FIFO�������
	{
		*data = I2caRegs.I2CDRR;
	}
//	TM1650.MasterStatus = IIC_IDLE;
	DELAY_US(15);//��ʱ������λus
}



/************************************************************************/
//TM1650�����������ʾ
//���в�һ��������Ҫ��ʾ����ˢ��TM1650
/************************************************************************/
void TM1650_SEG_show(char cmd,Uint16 data)
{
	Uint16 Tmp;
	static Uint16 lastData=0;

	Tmp = data;

	if(lastData != Tmp)
	{
		if(cmd==0)
		{
			TM1650_Send(DIG4,SEG7Table[Tmp/1000]);   //DIG4
		}
		else
		{
			TM1650_Send(DIG4,SEG7Table[cmd]);        //DIG4
		}
		TM1650_Send(DIG3,SEG7Table[Tmp/100%10]); //DIG3
		TM1650_Send(DIG2,SEG7Table[(Tmp/10)%10]);//DIG2
		TM1650_Send(DIG1,SEG7Table[Tmp%10]);     //DIG1
		lastData = Tmp;
	}



}

//===========================================================================
// No more.
//===========================================================================
