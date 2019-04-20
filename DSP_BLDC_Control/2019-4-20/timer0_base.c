

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"


int period_ms_count=0;
Uint16 oneCircleCount=36;
Uint16 hallCount=0;
Uint16 lastHallState=0;
Uint16 mySpeed=0;

timer0 timer0Base={0,0};     //����һ����ʱ�������ṹ��

__interrupt void cpu_timer0_isr(void);


//---------------------��ʱ��0��ʼ��------------------------
//1ms �ж�����
void Timer0_init()
{
	//�ж����ò���-----1,����ģ���ж�ʹ�ܣ�λ�� Timer->RegsAddr->TCR.bit.TIE = 1;
	ConfigCpuTimer(&CpuTimer0, 150, 1000);//150MHz CPU Freq, 1 millisecond Period (in uSeconds)
	CpuTimer0Regs.TCR.all = 0x4001;		   // Use write-only instruction to set TSS bit = 0

	//�ж����ò���-----2����ӳ���жϷ�����
	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EDIS;
	//�ж����ò���-----3������CPU�ж�Y
	IER |= M_INT1;
	//�ж����ò���-----4������Y�ж���ĵڼ�λ
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
}


//#################################################
//��ʱ��0�жϷ�����
//-----------------------------------------------
__interrupt void cpu_timer0_isr(void)
{
	timer0Base.Mark_Para.Status_Bits.OnemsdFlag = 1;
	timer0Base.msCounter++;


	//������ٶ�
	period_ms_count++;
	if(HallEncode.HallState!=lastHallState)
	{
		lastHallState=HallEncode.HallState;
		hallCount++;
	}
	if(hallCount>=oneCircleCount)
	{
		mySpeed=(float)1000*60/period_ms_count;
//		sciPutString(1,"1Ȧ�����ĺ�������", 0);
//		send_number_to_USART(period_ms_count);
//		sciPutString(1,"mySpeed=", 0);
//		send_number_to_USART(mySpeed);
		MotorADJ.Now_Speed=(int)mySpeed;

		period_ms_count=0;
		hallCount=0;
	}

	//�� MONI_SPEED_TIMEROUT ʱ�������µĲ������ݣ����ٶ�ֵ����Ϊ��
//	if(period_ms_count > MONI_SPEED_TIMEROUT)
//	{
//		MotorADJ.Now_Speed =0;
//		period_ms_count=0;
//	}

	//����ͨѶ�������ݼ�ʱ
	if(SCI_Msg.Mark_Para.Status_Bits.rFifoDataflag == 1)
		SCI_Msg.timerOut++;

	//IIC�жϳ�ʱ�ļ�ʱ
	if(TM1650.MasterStatus == IIC_IDLE)
		TM1650.IIC_TimerOUT =0;
	else
		TM1650.IIC_TimerOUT++;

	if(MotorADJ.OpenLoopSetUpCNT>0) //��������ʱ��
		MotorADJ.OpenLoopSetUpCNT--;

	if(hallFindCnt>0)
		hallFindCnt--;                //���������λ�ù�ϵ����ģʽʱ������ͨ���ʱ��

	MotorControl();//������ƺ�����7������ģʽ��Ĭ�Ͽ������ٶȱջ�ģʽ����� Mark.Status_Bits.TestMode ������

	// Acknowledge this interrupt to receive more interrupts from group 1
	EALLOW;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	EDIS;

}

