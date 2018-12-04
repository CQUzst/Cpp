/*
 * Exint.h
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */
#ifndef __MOTOR_EXINT_H
#define __MOTOR_EXINT_H

//Hall Port
#define MOTORIPM

#define HALLANUM 14
#define HALLAMUX GpioCtrlRegs.GPAMUX1.bit.GPIO14
#define HALLADIR GpioCtrlRegs.GPADIR.bit.GPIO14
#define HALLAQSEL GpioCtrlRegs.GPAQSEL1.bit.GPIO14
#define rHADAT()  GpioDataRegs.GPADAT.bit.GPIO14

#define HALLBNUM 15
#define HALLBMUX GpioCtrlRegs.GPAMUX1.bit.GPIO15
#define HALLBDIR GpioCtrlRegs.GPADIR.bit.GPIO15
#define HALLBQSEL GpioCtrlRegs.GPAQSEL1.bit.GPIO15
#define rHBDAT()  GpioDataRegs.GPADAT.bit.GPIO15

#define HALLCNUM 48
#define HALLCMUX GpioCtrlRegs.GPBMUX2.bit.GPIO48
#define HALLCDIR GpioCtrlRegs.GPBDIR.bit.GPIO48
#define HALLCQSEL GpioCtrlRegs.GPBQSEL2.bit.GPIO48
#define rHCDAT()  GpioDataRegs.GPBDAT.bit.GPIO48

//���״̬
typedef enum
{
  M_CW    = 0,//˳ʱ��ת��    ��ת
  M_CCW   = 1//��ʱ��ת��   ��ת
} MOTOR_DIR;

typedef struct
{
	Uint16 HallState;
	Uint16 HallStatepPevent;
	Uint16 HallType;

//����HC�жϼ����� 2���Ե����һȦ2�����弴Ϊ4����Ե����
//Ϊ�˱�֤���ٵľ�ȷ�ȣ�һȦ����ͬһ������Ԫ�����źŽ��в����жϣ�����༶�Ե����װʱ�Ļ�еƫ����ɲ��ٲ���ȷ
//������뿴���ٻ��㲿��
	Uint16 HallCNT;
}DEF_HALL;

extern DEF_HALL HallEncode;

void ExInt_Init(void);

#endif //__MOTOR_EXINT_H

