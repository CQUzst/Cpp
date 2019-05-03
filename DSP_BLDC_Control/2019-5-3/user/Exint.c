/*
 * Exint.h
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

DEF_HALL HallEncode={0,0,0};

void ExInt_Init(void);
interrupt void xint1_isr(void);
interrupt void xint2_isr(void);
interrupt void xint3_isr(void);

//###########################################################################
//�ⲿ�жϣ�˫��Ե��⣨������ �� �½��أ������л��ദ��
//����3���ⲿ�жϸ�BLDC��HALL������
//---------------------------------------------------------------------------------------------
void ExInt_Init(void)
{
	//�жϷ�����ӳ��
	EALLOW;
    PieVectTable.XINT1 = &xint1_isr;
	PieVectTable.XINT2 = &xint2_isr;
	PieVectTable.XINT3 = &xint3_isr;
	EDIS;

	//ʹ���жϾ���
	PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT4   //XINT1
	PieCtrlRegs.PIEIER1.bit.INTx5 = 1;          // Enable PIE Gropu 1 INT5   //XINT2
	PieCtrlRegs.PIEIER12.bit.INTx1 = 1;         // Enable PIE Gropu 12 INT1 //XINT3
	IER |= M_INT1;                              // Enable CPU INT1
	IER |= M_INT12;                             // Enable CPU INT12

	//3�������źŵ�IO����  GPIO14 GPIO15  GPIO48
	EALLOW;
	HALLAMUX = 0;
	HALLADIR = 0;
	HALLAQSEL = 0;

	HALLBMUX = 0;
	HALLBDIR = 0;
	HALLBQSEL = 0;

	HALLCMUX = 0;
	HALLCDIR = 0;
	HALLCQSEL = 0;
	EDIS;

	//�ж�Դ����
	EALLOW;
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = HALLANUM;
	GpioIntRegs.GPIOXINT2SEL.bit.GPIOSEL = HALLBNUM;
	GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = HALLCNUM;
	EDIS;

	XIntruptRegs.XINT1CR.bit.POLARITY = 3;      // Interrupt generatedonbothafalling edgeandarisingedge
	XIntruptRegs.XINT2CR.bit.POLARITY = 3;      // Interrupt generatedonbothafalling edgeandarisingedge
	XIntruptRegs.XINT3CR.bit.POLARITY = 3;      // Interrupt generatedonbothafalling edgeandarisingedge

	// Enable XINT1 and XINT2,XINT3
	XIntruptRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1
	XIntruptRegs.XINT2CR.bit.ENABLE = 1;        // Enable XINT2
	XIntruptRegs.XINT3CR.bit.ENABLE = 1;        // Enable XINT3
}


//###########################################################################
//�ⲿ�ж�1������
//---------------------------------------------------------------------------------------------
interrupt void xint1_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

	HallEncode.HallStatepPevent = HallEncode.HallState;
    HallEncode.HallState = ReadHall();
    MOS_HX(&HallEncode.HallState);

}

//###########################################################################
//�ⲿ�ж�2������
//---------------------------------------------------------------------------------------------
interrupt void xint2_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

	HallEncode.HallStatepPevent = HallEncode.HallState;
	HallEncode.HallState = ReadHall();
	MOS_HX(&HallEncode.HallState);

}

//###########################################################################
//�ⲿ�ж�3������
//---------------------------------------------------------------------------------------------
interrupt void xint3_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
	HallEncode.HallStatepPevent = HallEncode.HallState;
	HallEncode.HallState = ReadHall();
	MOS_HX(&HallEncode.HallState);

	HallEncode.HallCNT++; //
}

