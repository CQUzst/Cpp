/*
 *  TM1650_IIC.h
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef TM1650_IIC_H_
#define TM1650_IIC_H_

#define IIC_NODE_NUM 1 //IIC�ڵ�����2�����ֱ��� TM1650/AT24C02

#define AT24CO2_TIMER_OUT  3 //msΪ��λ
#define TM1650_TIMER_OUT   3 //msΪ��λ
#define IIC_RST_BIT        0X0020      //IIC ��λ

//S2--S6 ��TM1650ɨ���µķ��ؼ���ֵ
#define S2_CODE 0x44
#define S3_CODE 0x4C
#define S4_CODE 0x54
#define S5_CODE 0x5C
#define S6_CODE 0x64

#define S1_CODE 0x00//S1Ϊ��������������Ϊ 0x00

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

//��TM1650�йص������ַ����Ҫ���ƶ�1λ���б�׼��IIC��������׼IIC �� LSBΪ��дλ����7λΪ�����ַλ��
#define CMD_SEG  (0X48)>>1	//��ʾģʽ ADDR
#define CMD_KEY  (0X49)>>1	//����ģʽ ADDR

//������ұߵ�һ������ΪDIG1
//#define DIG1  (0X68)>>1 //DIG1 ADDR
//#define DIG2  (0X6A)>>1 //DIG2 ADDR
//#define DIG3  (0X6C)>>1 //DIG3 ADDR
//#define DIG4  (0X6E)>>1 //DIG4 ADDR

//�������ߵ�һ������ΪDIG1
#define DIG4  (0X68)>>1 //DIG1 ADDR
#define DIG3  (0X6A)>>1 //DIG2 ADDR
#define DIG2  (0X6C)>>1 //DIG3 ADDR
#define DIG1  (0X6E)>>1 //DIG4 ADDR

//---------------------------------------------------
//SDA����GPIO32
#define SDAGPIO GPIO32
#define SDAMUX GpioCtrlRegs.GPBMUX1.bit.SDAGPIO
#define SDADIR GpioCtrlRegs.GPBDIR.bit.SDAGPIO
#define SDA_H()  GpioDataRegs.GPBSET.bit.SDAGPIO=1
#define SDA_L() GpioDataRegs.GPBCLEAR.bit.SDAGPIO =1

//CLK����GPIO33
#define CLKGPIO GPIO33
#define CLKMUX GpioCtrlRegs.GPBMUX1.bit.CLKGPIO
#define CLKDIR GpioCtrlRegs.GPBDIR.bit.CLKGPIO
#define CLK_H()  GpioDataRegs.GPBSET.bit.CLKGPIO=1
#define CLK_L() GpioDataRegs.GPBCLEAR.bit.CLKGPIO =1

// I2C Message Structure
typedef struct I2CSlaveMSG{
  Uint16 IntSRC[10];            //�ж�Դ  ������
  Uint16 IntSRCCnt;             //�ж�Դ��¼  ������
  Uint16 MasterStatus;			//������������IIC״̬
  Uint16 SlavePHYAddress;	    //IIC�����ַ��Ӳ����ַ��
  Uint16 LogicAddr;             //������EEPROM����Ҫ�ṩ�߼���ַ
  Uint16 Len;			        //�������ݵĳ��ȣ����������ַ��
  Uint16 MsgInBuffer[4]; //�������飬���4�����
  Uint16 MsgOutBuffer[4];//�������飬���4�����
  Uint16 IIC_TimerOUT;   //�������ĳ�ʱ��������1ms��ʱ���м���
}I2CSlaveMSG;


extern I2CSlaveMSG *PtrMsg[];

extern I2CSlaveMSG TM1650;
extern char *IIC_ISR_String[];
extern uint8_t SEG7Table[];
extern int  *SEG_Show_Type[];//�������ʾ����
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
