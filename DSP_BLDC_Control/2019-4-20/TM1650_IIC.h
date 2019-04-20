/*
 *  TM1650_IIC.h
 *
 *  Created on: 2017-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef TM1650_IIC_H_
#define TM1650_IIC_H_

#define IIC_NODE_NUM 1 //IIC节点数，2个，分别是 TM1650/AT24C02

#define AT24CO2_TIMER_OUT  3 //ms为单位
#define TM1650_TIMER_OUT   3 //ms为单位
#define IIC_RST_BIT        0X0020      //IIC 复位

//S2--S6 在TM1650扫描下的返回键码值
#define S2_CODE 0x44
#define S3_CODE 0x4C
#define S4_CODE 0x54
#define S5_CODE 0x5C
#define S6_CODE 0x64

#define S1_CODE 0x00//S1为独立按键，按下为 0x00

extern char keyVal,keyReg,LigntVal;

// Status Flags
#define I2C_AL_BIT          0x0001
#define I2C_NACK_BIT        0x0002
#define I2C_ARDY_BIT        0x0004
#define I2C_RRDY_BIT        0x0008
#define I2C_SCD_BIT         0x0020

// I2C State defines
#define IIC_IDLE               0
#define IIC_WRITE              1
#define IIC_READ_STEP1         2
#define IIC_READ_STEP2         3
#define IIC_READ_STEP3         4
#define IIC_BUS_BUSY           5
#define IIC_BUS_ERROR          6

//与TM1650有关的物理地址（需要右移动1位进行标准的IIC操作；标准IIC 的 LSB为读写位，高7位为物理地址位）
#define CMD_SEG  (0X48)>>1	//显示模式 ADDR
#define CMD_KEY  (0X49)>>1	//按键模式 ADDR

//数码管右边第一个定义为DIG1
//#define DIG1  (0X68)>>1 //DIG1 ADDR
//#define DIG2  (0X6A)>>1 //DIG2 ADDR
//#define DIG3  (0X6C)>>1 //DIG3 ADDR
//#define DIG4  (0X6E)>>1 //DIG4 ADDR

//数码管左边第一个定义为DIG1
#define DIG4  (0X68)>>1 //DIG1 ADDR
#define DIG3  (0X6A)>>1 //DIG2 ADDR
#define DIG2  (0X6C)>>1 //DIG3 ADDR
#define DIG1  (0X6E)>>1 //DIG4 ADDR

//---------------------------------------------------
//SDA采用GPIO32
#define SDAGPIO GPIO32
#define SDAMUX GpioCtrlRegs.GPBMUX1.bit.SDAGPIO
#define SDADIR GpioCtrlRegs.GPBDIR.bit.SDAGPIO
#define SDA_H()  GpioDataRegs.GPBSET.bit.SDAGPIO=1
#define SDA_L() GpioDataRegs.GPBCLEAR.bit.SDAGPIO =1

//CLK采用GPIO33
#define CLKGPIO GPIO33
#define CLKMUX GpioCtrlRegs.GPBMUX1.bit.CLKGPIO
#define CLKDIR GpioCtrlRegs.GPBDIR.bit.CLKGPIO
#define CLK_H()  GpioDataRegs.GPBSET.bit.CLKGPIO=1
#define CLK_L() GpioDataRegs.GPBCLEAR.bit.CLKGPIO =1

// I2C Message Structure
typedef struct I2CSlaveMSG{
  Uint16 IntSRC[10];            //中断源  测试用
  Uint16 IntSRCCnt;             //中断源记录  测试用
  Uint16 MasterStatus;			//自行软件定义的IIC状态
  Uint16 SlavePHYAddress;	    //IIC物理地址（硬件地址）
  Uint16 LogicAddr;             //类似于EEPROM，需要提供逻辑地址
  Uint16 Len;			        //操作数据的长度（不含物理地址）
  Uint16 MsgInBuffer[4]; //发送数组，最大4个深度
  Uint16 MsgOutBuffer[4];//接收数组，最大4个深度
  Uint16 IIC_TimerOUT;   //软件定义的超时变量，在1ms定时器中计数
}I2CSlaveMSG;


extern I2CSlaveMSG *PtrMsg[];

extern I2CSlaveMSG TM1650;
extern char *IIC_ISR_String[];
extern uint8_t SEG7Table[];
extern int  *SEG_Show_Type[];//数码管显示内容
extern int  *PtrSegType;
extern char SEG_DIG4_Cmd;
extern char keyReg;

void InitI2C_Gpio(void);
void   I2CA_Init(void);
Uint16 I2CA_Tx_STOP(struct I2CSlaveMSG *msg);
Uint16 I2CA_Rxdata_STOP(struct I2CSlaveMSG *msg);
void softResetIIC_BUS(void);

void TM1650_Send(char addr,char data);
void TM1650_Read(char addr, char *data);

void TM1650_SEG_show(char cmd,Uint16 data);

#endif /* TM1650_IIC_H_ */
