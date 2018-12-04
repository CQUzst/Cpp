/*
 * M_Cap.c
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

//###########################################################################
/* CAP����ģ�飬2����BLDC�����תһȦ2�����壬����2�ν��
 * ����IO�� GPIO48/CAP5
 * cap�жϷ���������ȡ�����ԭʼ���ݣ�������ٻ����� MeasureSpeed ������ʵ��
 * */
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

// Prototype statements for functions found within this file.
interrupt void ecap5_isr(void);

DEF_CAP mCAP;

void Motor_Cap_Init(void);
void InitECapture(void);

//###########################################################################
//Cap���ܳ�ʼ���������ж�
//GPIO48/CAP5
//---------------------------------------------------------------------------------------------
void Motor_Cap_Init(void)
{
	EALLOW;
	PieVectTable.ECAP5_INT = &ecap5_isr;
	EDIS;
	InitECapxGpio(); //��ʼ��GPIO48 ΪCAP5������������
	InitECapture();
	PieCtrlRegs.PIEIER4.bit.INTx5 = 1;
	IER |= M_INT4;
} 

//-------------------------------------------------------------------------
//IPM IO��ʼ��
//##########################################################################
void InitECapxGpio(void)
{
   EALLOW;
   GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;     //  Enable pull-up on GPIO (CAP5)
   GpioCtrlRegs.GPBQSEL2.bit.GPIO48 = 0;
   GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 1;    //  Configure GPIO48as CAP5
   EDIS;
}


//###########################################################################
//Cap���ܳ�ʼ��
//�����������
//---------------------------------------------------------------------------------------------
void InitECapture()
{
   ECap5Regs.ECEINT.all = 0x0000;             // Disable all capture interrupts
   ECap5Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
   ECap5Regs.ECCTL1.bit.CAPLDEN = 0;          // Disable CAP1-CAP4 register loads
   ECap5Regs.ECCTL2.bit.TSCTRSTOP = 0;        // Make sure the counter is stopped
   
   // Configure peripheral registers
   ECap5Regs.ECCTL2.bit.CONT_ONESHT = 0;      // Operatein continuousmode  //One-shot
   ECap5Regs.ECCTL2.bit.STOP_WRAP = 0;        // Stop at 1 events
   ECap5Regs.ECCTL1.bit.CAP1POL = 0;          // Rising edge
//   ECap1Regs.ECCTL1.bit.CAP2POL = 0;          // Rising edge
//   ECap1Regs.ECCTL1.bit.CAP3POL = 1;          // Falling edge
//   ECap1Regs.ECCTL1.bit.CAP4POL = 0;          // Rising edge

   
   //HIKE P29 7.3 Example3- Time Difference(Delta)Operation Rising Edge Trigger
   ECap5Regs.ECCTL1.bit.CTRRST1 = 1;          // Difference operation
//   ECap1Regs.ECCTL1.bit.CTRRST2 = 1;          // Difference operation         
//   ECap1Regs.ECCTL1.bit.CTRRST3 = 1;          // Difference operation         
//   ECap1Regs.ECCTL1.bit.CTRRST4 = 1;          // Difference operation        
   
   ECap5Regs.ECCTL2.bit.SYNCI_EN = 0;         // Disable sync in  //HIKE P13 Figure6.Detail sof the Counter and Synchronization Block
   ECap5Regs.ECCTL2.bit.CAP_APWM = 0; 	      //ECAPmodule
   ECap5Regs.ECCTL2.bit.SYNCO_SEL = 2;        // EC_SYNCO_DIS
   ECap5Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable capture units

   ECap5Regs.ECCTL1.bit.PRESCALE = 0;

   ECap5Regs.ECCTL2.bit.TSCTRSTOP = 1;        // Start Counter
   ECap5Regs.ECCTL2.bit.REARM = 1;            // arm one-shot
   ECap5Regs.ECCTL1.bit.CAPLDEN = 1;          // Enable CAP1-CAP4 register loads
   ECap5Regs.ECEINT.bit.CEVT1 = 1;            // 1 events = interrupt

}

//###########################################################################
//Cap�жϷ�����
//---------------------------------------------------------------------------------------------
interrupt void ecap5_isr(void)
{
	MotorADJ.NewCapValFlag = 1;
	MotorADJ.CapVal = ECap5Regs.CAP1;  //ԭʼ���ݴ��� MotorADJ.CapVal
	MotorADJ.MoniSpeedZeroCNT = 0;     //���µĲ������ݣ����� ���� ���

	ECap5Regs.ECCLR.bit.CEVT1 = 1;
	ECap5Regs.ECCLR.bit.INT = 1;
	ECap5Regs.ECCTL2.bit.REARM = 1;

	// Acknowledge this interrupt to receive more interrupts from group 4
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}


//===========================================================================
// No more.
//================================================================


