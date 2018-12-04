/*
 *  sci_base.c
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#include <stdio.h>
#include <file.h>
#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File  
#include "HK_all_include.h"

interrupt void uartRx_isr(void);

Uart_Msg SCI_Msg={0, {0},0,0};

//#################################################
//-----------------------------------------------
//���ڳ�ʼ��
// ������ baud = LSPCLK/8/((BRR+1)
//            baud @LSPCLK = 37.5MHz (150 MHz SYSCLK)��
//           ����LSPCLK�ĳ�ʼ������main����ϵͳ��ʼ������ InitSysCtrl  ��� InitPeripheralClocks ʵ�֣�Ĭ�� LSPCLK = SYSCLK/4 = 15MHz��
// 115200   8N1
// P13 ��ͼ����SCI
// P22 1.2.10.1 SCI FIFO Description������FIFOʹ��
//-----------------------------------------------
void SCI_Init()
{
	InitSciaGpio();
	SciaRegs.SCICTL1.bit.SWRESET = 0;

 	SciaRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
// baud = LSPCLK/8/((BRR+1) 
// baud @LSPCLK = 37.5MHz (150 MHz SYSCLK)
	SciaRegs.SCIHBAUD   =0x0000;
    SciaRegs.SCILBAUD    = 40;    //4-->921600bps  40--> 115200bps ; 18-->256000bps

    SciaRegs.SCICTL1.bit.SWRESET = 1;     // Relinquish SCI from Reset
    SciaRegs.SCIFFTX.bit.SCIRST=1;

	SciaRegs.SCIFFRX.bit.RXFFIL  = SCI_FIFO_LEN;  //���ý����жϵ�FIFO���
	SciaRegs.SCICTL1.bit.TXENA = 1;               //ʹ�ܷ���
	SciaRegs.SCICTL1.bit.RXENA = 1;               //ʹ�ܽ���

//	SciaRegs.SCICTL2.bit.TXINTENA =1;
//	SciaRegs.SCICTL2.bit.RXBKINTENA =1;
//  SciaRegs.SCIFFTX.bit.TXFFIENA = 0;       //��ֹ�����ж�ʹ��

	//�ж����ò���-----1
	SciaRegs.SCIFFTX.bit.SCIFFENA = 1;      //ʹ��FIFO�ж�
	SciaRegs.SCIFFRX.bit.RXFFIENA=1;
	EALLOW;
	PieVectTable.SCIRXINTA = &uartRx_isr;    //�ж����ò���-----2
	EDIS;
	PieCtrlRegs.PIEIER9.bit.INTx1 = 1;       //�ж����ò���-----3
	IER |= M_INT9;						     //�ж����ò���-----4

	SciaRegs.SCIFFCT.all=0x00;
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	SciaRegs.SCIFFRX.bit.RXFIFORESET=1;
}


//#################################################
//-----------------------------------------------
//����һ���ֽ�/�ַ�
//-----------------------------------------------
void scia_xmit(int a)
{
	Uint32 WaitTimer = 0;

	//while (SciaRegs.SCIFFTX.bit.TXFFST != 0)
	while(SciaRegs.SCICTL2.bit.TXEMPTY != 1)
	{
		WaitTimer++;
		if(WaitTimer > TIMEROUTSCI)break;
	}
	if(WaitTimer <= TIMEROUTSCI)
		SciaRegs.SCITXBUF=a;
}

//#################################################
//-----------------------------------------------
//����һ���� ����
//-----------------------------------------------
void HexPrintf(Uint16 d)
{
//	Uint16 t;
	if(d<=0xff)
	{
		scia_xmit(0);
		scia_xmit(d);
	}
	else
	{
//		t = d;
//		t = t>>8;
		scia_xmit(d>>8);
		scia_xmit(d);
	}
}

void int2str(Uint16 n, char *str)
{
    char buf[10] = "";
    int i = 0;
    int len = 0;
    int temp = n;
    while(temp)
    {
        buf[i++] = (temp % 10) + '0';  //��temp��ÿһλ�ϵ�������buf
        temp = temp / 10;
    }
    len = i;
    str[i] = 0;            //ĩβ�ǽ�����0
    while(1)
    {
        i--;
        if (buf[len-i-1] ==0)
        {
            break;
        }
        str[i] = buf[len-i-1];  //��buf��������ַ������ַ���
    }
    if (i == 0 )
    {
        str[i] = '-';          //����Ǹ��������һ������
    }
}
void int32_2str(Uint32 n, char *str)
{
    char buf[34] = "";
    int i = 0;
    int len = 0;
    int temp = n;
    while(temp)
    {
        buf[i++] = (temp % 10) + '0';  //��temp��ÿһλ�ϵ�������buf
        temp = temp / 10;
    }
    len =i;  //���n�Ǹ����������Ҫһλ���洢����
    str[i] = 0;            //ĩβ�ǽ�����0
    while(1)
    {
        i--;
        if (buf[len-i-1] ==0)
        {
            break;
        }
        str[i] = buf[len-i-1];  //��buf��������ַ������ַ���
    }
}


void send_number_to_USART(Uint16 num){
	char str[14];
	int2str(num,str);
	sciPutString(0,str, 0);
}
void send_speed_to_USART(Uint32 num){
	char str[34];
	int32_2str(num,str);
	sciPutString(0,str,0);
}

//#################################################
//-----------------------------------------------
//�����ַ���
//Enter1 �ַ���ǰ�Ļس����д���   Enter2 �ַ�����Ļس����д���
//-----------------------------------------------
void sciPutString(char Enter1,char *s, char Enter2)
{
	char i;

	for(i=0;i<Enter1;i++)
	{
		scia_xmit(0x0D);
		scia_xmit(0x0A);
	}

	while(*s!=0)
	{
		scia_xmit(*(s++));
	}

	for(i=0;i<Enter2;i++)
	{
		scia_xmit(0x0D);
		scia_xmit(0x0A);
	}
}


//#################################################
//���ڽ����жϺ���
//����FIFO���ƣ����棩
//SCI_FIFO_LEN ����Ϊ 1�����Ϊ4
//-----------------------------------------------
interrupt void uartRx_isr(void)
{
    SciaRegs.SCIFFRX.bit.RXFFINTCLR=1;      // Clear Interrupt flag
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;

	if(SciaRegs.SCIFFRX.bit.RXFFOVF == 0)//����FIFOδ���
	{
		SCI_Msg.Mark_Para.Status_Bits.rFifoDataflag = 1;

		//��������
		while(SciaRegs.SCIFFRX.bit.RXFFST)
		{
			SCI_Msg.rxData[SCI_Msg.rxWriteIndex] = SciaRegs.SCIRXBUF.all;
			SCI_Msg.rxWriteIndex=(++SCI_Msg.rxWriteIndex)%(UartRxLEN);
		}
	}
	else
	{
		//�û�����������Ӳ������Ĵ���,������ȫ��ȡ��FIFO������ݻ������FIFO
		//�������FIFO����
		SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear HW Overflow flag
		SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;  //Write 0 to reset the FIFO pointer to zero, and hold in reset.
		SciaRegs.SCIFFRX.bit.RXFIFORESET = 1 ; //Re-enable receive FIFO operation
	}
}


