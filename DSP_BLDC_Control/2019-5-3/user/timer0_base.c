

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
timer0 timer0Base={0,0};     //声明一个定时器变量结构体

__interrupt void cpu_timer0_isr(void);


//---------------------定时器0初始化------------------------
//1ms 中断配置
void Timer0_init()
{
	//中断配置步骤-----1,开启模块中断使能，位于 Timer->RegsAddr->TCR.bit.TIE = 1;
	ConfigCpuTimer(&CpuTimer0, 150, 100);//150MHz CPU Freq, 0.1 ms Period
	CpuTimer0Regs.TCR.all = 0x4001;

	//中断配置步骤-----2，重映射中断服务函数
	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EDIS;
	//中断配置步骤-----3，连接CPU中断Y
	IER |= M_INT1;
	//中断配置步骤-----4，连接Y中断里的第几位
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
}


//#################################################
//定时器0中断服务函数
//-----------------------------------------------
__interrupt void cpu_timer0_isr(void)
{
	timer0Base.Mark_Para.Status_Bits.OnemsdFlag = 1;
	timer0Base.msCounter++;
	myTime++;
	//监测电机速度
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
		//		sciPutString(1,"1圈经过的毫秒数：", 0);
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
		//sciPutString(1,"电机开始减速",1);
		Mark.Status_Bits.MotorState = M_ACCDOWN;
	}

	//在 MONI_SPEED_TIMEROUT 时间内无新的测速数据，将速度值设置为零
	//	if(period_ms_count > MONI_SPEED_TIMEROUT)
	//	{
	//		MotorADJ.Now_Speed =0;
	//		period_ms_count=0;
	//	}

	//串口通讯接收数据计时
	if(SCI_Msg.Mark_Para.Status_Bits.rFifoDataflag == 1)
		SCI_Msg.timerOut++;

	//IIC判断超时的计时
	if(TM1650.MasterStatus == IIC_IDLE)
		TM1650.IIC_TimerOUT =0;
	else
		TM1650.IIC_TimerOUT++;

	if(MotorADJ.OpenLoopSetUpCNT>0) //开环启动时间
		MotorADJ.OpenLoopSetUpCNT--;

	if(hallFindCnt>0)
		hallFindCnt--;                //绕组与霍尔位置关系查找模式时，绕组通电的时间

	MotorControl();//电机控制函数，7种试验模式，默认开启是速度闭环模式，详见 Mark.Status_Bits.TestMode 的设置

	// Acknowledge this interrupt to receive more interrupts from group 1
	EALLOW;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	EDIS;

}

