/*
 *   sci_base.h
 *
 *  Created on: 2017-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */
#ifndef  __SCI_BASE_H
#define  __SCI_BASE_H

//#include <stdio.h>
//#include <file.h>

#define SCIBaudRate   115200L
#define TIMEROUTSCI (Uint32)10*(SYSCLK/SCIBaudRate) //估算的等待超时时间，请根据实际修改

#define SCI_FIFO_LEN  1 //定义DSP串口FIFO深度，最大16级深度， 1-16

#define UartRxLEN 20  //接收缓存长度
#define UartTxLEN 20  //发送缓存长度

#define RTU_TIMEROUT 5 //ms

typedef struct Uart_Type{
	union
	  {
	    Uint16 All;
	    struct{
	            Uint16  UartRevFlag           :1;  //接收到数据标志
	            Uint16  HWOVFlag               :1;  //DSP硬件接收数据溢出标志

	            Uint16  rFifoDataflag            :1;  //接收内存非空
	            Uint16  rFifoFullflag            :1;  //接收内存溢出

	            Uint16  DISRevflag                 :1;  //接收关闭

	    		}Status_Bits;
	  	}Mark_Para;

	char rxData[UartRxLEN];						//接收缓存
	Uint16 rxReadIndex;                         //接收FIFO写入索引
	Uint16 rxWriteIndex;                        //接收FIFO读出索引

	Uint16 timerOut;                            //超时判断
}
Uart_Msg; 

extern Uart_Msg SCI_Msg;

//--------------------------------------------------------------------
void handleRxFIFO(void);
void SCI_Init(void);
void scia_xmit(int a);
void sciPutString(char Enter1,char *s, char Enter2);
void HexPrintf(Uint16 d);

void int2str(Uint16 n, char *str);
void int32_2str(Uint32 n, char *str);
void send_number_to_USART(Uint16 num);
void send_speed_to_USART(Uint32 num);

#endif//  __SCI_BASE_H


