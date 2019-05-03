

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"


int period_ms_count=0;
Uint16 oneCircleCount=6;
Uint16 hallCount=0;
Uint16 targerHallCount=0;
Uint16 lastHallState=0;
double mySpeed=0.0;
int myTime=0;
int realTime=0;
int greed=37;
timer0 timer0Base={0,0};     //����һ����ʱ�������ṹ��

__interrupt void cpu_timer0_isr(void);


//---------------------��ʱ��0��ʼ��------------------------
//1ms �ж�����
void Timer0_init()
{
	//�ж����ò���-----1,����ģ���ж�ʹ�ܣ�λ�� Timer->RegsAddr->TCR.bit.TIE = 1;
	ConfigCpuTimer(&CpuTimer0, 150, 100);//150MHz CPU Freq, 0.1 ms Period
	CpuTimer0Regs.TCR.all = 0x4001;

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
	myTime++;
	//������ٶ�
	period_ms_count++;
	if(HallEncode.HallState!=lastHallState)
	{
		lastHallState=HallEncode.HallState;
		hallCount++;
		targerHallCount++;
	}
	if(hallCount>=oneCircleCount)
	{
		mySpeed=(double)10000*10.0/period_ms_count;
		//		sciPutString(1,"1Ȧ�����ĺ�������", 0);
		//		send_number_to_USART(period_ms_count);
		sciPutString(1,"mySpeed=", 0);
		mySpeed=mySpeed*100;
		send_number_to_USART((Uint16)mySpeed/100);
		sciPutString(0,".", 0);
		send_number_to_USART((Uint16)mySpeed%100);
		sciPutString(0,"    time(s)=", 0);
		realTime+=myTime/1000;
		send_number_to_USART(realTime/10);
		sciPutString(0,".", 0);
		send_number_to_USART(realTime%10);
		MotorADJ.Now_Speed=(int)mySpeed;

		period_ms_count=0;
		hallCount=0;
		myTime=0;
	}
	if(targerHallCount>=greed){
		//sciPutString(1,"�����ʼ����",1);
		Mark.Status_Bits.MotorState = M_ACCDOWN;
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

