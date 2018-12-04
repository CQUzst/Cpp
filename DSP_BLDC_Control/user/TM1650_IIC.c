/*
 *  TM1650_IIC.c
 *
 *  Created on: 2017-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

char keyVal,LigntVal;
int  *SEG_Show_Type[]={&MotorADJ.Set_Speed,&MotorADJ.Now_Speed,&MotorADJ.MosFETtempI};//数码管显示内容
int  *PtrSegType = 0;
char SEG_DIG4_Cmd=0; //DIG4（靠左的） LED显示内容
char keyReg=0;

__interrupt void i2c_int1a_isr(void);

//Uint16 IntSRC[10];            //中断源            测试用
//Uint16 IntSRCCnt;             //中断源记录  测试用
//Uint16 MasterStatus;			//自行软件定义的IIC状态
//Uint16 SlavePHYAddress;	    //IIC物理地址（硬件地址）
//Uint16 LogicAddr;             //类似于EEPROM，需要提供逻辑地址
//Uint16 Len;			        //操作数据的长度（不含物理地址）
//Uint16 MsgInBuffer[I2C_MAX_BUFFER_SIZE]; //发送数组，最大4个深度
//Uint16 MsgOutBuffer[I2C_MAX_BUFFER_SIZE];//接收数组，最大4个深度
//Uint16 IIC_TimerOUT;          //软件定义的超时变量

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

//中断源注释
char *IIC_ISR_String[] = {"No","ARB",
		        "NACK","ARDY",
		        "RX","Tx",
		        "SCD","AAS"};


I2CSlaveMSG TM1650;//定义一个TM1650结构体
I2CSlaveMSG AT24C02Msg;
I2CSlaveMSG *PtrMsg[]={&TM1650,&AT24C02Msg};//定义一个指针数组，存放I2C结构体，这里包含TM1650和EEPROM

uint8_t SEG7Table[]={

    //共阴极
    0x3f,0x06,0x5b,0x4f,
    0x66,0x6d,0x7d,0x07,
    0x7f,0x6f,0x77,0x7c,
    0x39,0x5e,0x79,0x71

    //共阳极
//    0xc0,0xf9,0xa4,0xb0,
//    0x99,0x92,0x82,0xf8,
//    0x80,0x90,0x88,0x83,
//    0xc6,0xa1,0x86,0x8e,
};


/************************************************************************/
//IIC 的IO口定义
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


//IIC中断函数，中断源为P32 IIC手册说明
__interrupt void i2c_int1a_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}


#define IICFRE 100000L   //100K IIC频率
/************************************************************************/
// Initialize I2C
//100K
//使能FIFO
//不采用中断
/************************************************************************/
void I2CA_Init(void)
{
   I2caRegs.I2CMDR.all &=~ 0x0020;	// Take I2C  reset
   DELAY_US(15);//延时参数单位us

   I2caRegs.I2CPSC.all = 14;		// Prescaler - need 7-12 Mhz on module clk (150/15 = 10MHz)
   I2caRegs.I2CMDR.all = 0x0020;	// Take I2C out of reset

   I2caRegs.I2CCLKL = SYSCLK/(I2caRegs.I2CPSC.all+1)/IICFRE/2-10;// NOTE: must be non zero
   I2caRegs.I2CCLKH = SYSCLK/(I2caRegs.I2CPSC.all+1)/IICFRE/2-5;// NOTE: must be non zero

   //P21 《sprufz9d--IIC.pdf》中断源
   //P28  I2CIER寄存器说明
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



//IIC底层发送函数
//IIC Maste Mode，发送模式
//I2caRegs.I2CCNT 为发送的字节长度，不包括从器件的物理地址
//发送格式：START ADDRESS CONTROL_BYTE DA_BYTE STOP
//当写入I2caRegs.I2CMDR.all = 0x2E20，硬件IIC进行操作，STOP信号结束
Uint16 I2CA_Tx_STOP(struct I2CSlaveMSG *msg)
{
	Uint16 i;
	if(I2caRegs.I2CMDR.bit.STP==1)//等待STOP信号
	{
	  return I2C_STP_NOT_READY_ERROR;
	}
    if (I2caRegs.I2CSTR.bit.BB == 1)
    {
       return I2C_BUS_BUSY_ERROR;
    }

    I2caRegs.I2CSAR = msg->SlavePHYAddress;//设置访问从器件地址，物理地址

    //FIFO为4个字节，所以最大放入 4个 数据
    for(i=0;i<msg->Len;i++)     // 小于等于4个byte的数据
    {
    	I2caRegs.I2CDXR = msg->MsgOutBuffer[i];
    }
 	I2caRegs.I2CCNT = msg->Len; //设置发送长度，最大为4个byte
    I2caRegs.I2CMDR.all = 0x2E20;		// Send restart as master receiver
    //TRX  = 1 ,发送模式； STP = 1 发送停止位
    return I2C_SUCCESS;
}


//IIC底层接收函数
//先发送从器件的物理地址，再接收 I2CCNT 个字节
//IIC Maste Mode，接收模式
//I2caRegs.I2CCNT 为发送的字节长度，不包括从器件的物理地址，
//格式(接收4个字节为例)：START ADDRESS DATA_BYTE1 DATA_BYTE2 DATA_BYTE3 DATA_BYTE4  STOP //DATA为读取的数据
//当写入I2caRegs.I2CMDR.all = 0x2C20，硬件IIC进行操作，硬件STOP发送后，接收的数据在FIFO中
Uint16 I2CA_Rxdata_STOP(struct I2CSlaveMSG *msg)
{
	I2caRegs.I2CSAR = msg->SlavePHYAddress;
 	I2caRegs.I2CCNT = msg->Len;	      // Setup how many bytes to expect
    I2caRegs.I2CMDR.all = 0x2C20;		      //  master receiver
    //TRX  = 0 ,接收模式； STP = 1 发送停止位
    return I2C_SUCCESS;
}


//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______

/************************************************************************/
//软件复位 IIC 从设备
//AT24C02 的数据手册P6      Figure 6-1. Software reset
// START + 9CLK(SDA=1) + START +　STOP
//操作完毕之后，使能IO口味IIC外设
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

    //软件复位完毕之后，要重新初始化IIC IO 为IIC 硬件IO口 调用
    //InitI2C_Gpio();
}



/************************************************************************/
//TM1650数码管驱动芯片发送函数
//可参考  TM1650.pdf 的P5 一个完整的写时序
//发送格式：START CMD1 DATA1 STOP
/************************************************************************/
void TM1650_Send(char addr,char data)
{
	TM1650.Len  = 1; //1 个数据(不含物理地址)
	TM1650.SlavePHYAddress = addr;
	TM1650.MsgOutBuffer[0] = data;//TM1650.pdf P4   8级亮度 开启显示
	I2CA_Tx_STOP(&TM1650);//发送数据，带STOP
	TM1650.IIC_TimerOUT = 0;
	TM1650.MasterStatus = IIC_WRITE;
	while((I2caRegs.I2CSTR.all & I2C_SCD_BIT)==0)
	{
		if(TM1650.IIC_TimerOUT > TM1650_TIMER_OUT)//超时判断
		{
			softResetIIC_BUS();	                 //软件复位IIC从设备
			TM1650.MasterStatus = IIC_IDLE;
			I2caRegs.I2CMDR.bit.STP = 1;
			I2caRegs.I2CSTR.all = 0xFFFF;
			break;
		}
	}
//	TM1650.MasterStatus = IIC_IDLE;
	I2caRegs.I2CSTR.all |= I2C_SCD_BIT;//清除标志位
	DELAY_US(5);//延时参数单位us
}


/************************************************************************/
//TM1650数码管驱动芯片读取按键函数
//可参考  TM1650.pdf 的P4 键盘扫描码获得按键;
//P3 提供了读按键时序
//发送格式：START CMD1 KEYDATA STOP
/************************************************************************/
void TM1650_Read(char addr, char *data)
{
	TM1650.Len  = 1; //需要读取的数据个数：1 个
	TM1650.SlavePHYAddress = addr;
	I2CA_Rxdata_STOP(&TM1650);//发送数据，带STOP
	TM1650.IIC_TimerOUT = 0;
	TM1650.MasterStatus = IIC_READ_STEP2;
	while((I2caRegs.I2CSTR.all & I2C_SCD_BIT)==0)
	{
		if(TM1650.IIC_TimerOUT > AT24CO2_TIMER_OUT)//超时判断
		{
			softResetIIC_BUS();	                 //软件复位IIC从设备
			TM1650.MasterStatus = IIC_IDLE;
			I2caRegs.I2CMDR.bit.STP = 1;
			I2caRegs.I2CSTR.all = 0xFFFF;
//			printf("\r\n Timer OUT in %d , Soft Reset",TM1650.MasterStatus);
			break;
		}
	}

	I2caRegs.I2CSTR.all |= I2C_SCD_BIT;//清除标志位

	TM1650.IIC_TimerOUT = 0;
	TM1650.MasterStatus = IIC_READ_STEP3;
	while(I2caRegs.I2CFFRX.bit.RXFFST)//读取完毕FIFO里的数据
	{
		*data = I2caRegs.I2CDRR;
	}
//	TM1650.MasterStatus = IIC_IDLE;
	DELAY_US(15);//延时参数单位us
}



/************************************************************************/
//TM1650控制数码管显示
//若有不一样的内容要显示，则刷新TM1650
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
