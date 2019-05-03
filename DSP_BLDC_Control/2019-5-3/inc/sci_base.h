/*
 *   sci_base.h
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */
#ifndef  __SCI_BASE_H
#define  __SCI_BASE_H

//#include <stdio.h>
//#include <file.h>

#define SCIBaudRate   115200L
#define TIMEROUTSCI (Uint32)10*(SYSCLK/SCIBaudRate) //����ĵȴ���ʱʱ�䣬�����ʵ���޸�

#define SCI_FIFO_LEN  1 //����DSP����FIFO��ȣ����16����ȣ� 1-16

#define UartRxLEN 20  //���ջ��泤��
#define UartTxLEN 20  //���ͻ��泤��

#define RTU_TIMEROUT 5 //ms

typedef struct Uart_Type{
	union
	  {
	    Uint16 All;
	    struct{
	            Uint16  UartRevFlag           :1;  //���յ����ݱ�־
	            Uint16  HWOVFlag               :1;  //DSPӲ���������������־

	            Uint16  rFifoDataflag            :1;  //�����ڴ�ǿ�
	            Uint16  rFifoFullflag            :1;  //�����ڴ����

	            Uint16  DISRevflag                 :1;  //���չر�

	    		}Status_Bits;
	  	}Mark_Para;

	char rxData[UartRxLEN];						//���ջ���
	Uint16 rxReadIndex;                         //����FIFOд������
	Uint16 rxWriteIndex;                        //����FIFO��������

	Uint16 timerOut;                            //��ʱ�ж�
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


