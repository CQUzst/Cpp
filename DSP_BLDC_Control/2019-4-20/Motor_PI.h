/*
 *  Motor_PI.h
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef MOTOR_PI_H_
#define MOTOR_PI_H_


#define POLEPAIRS 6 //���������

extern Uint16 hallFindCnt;

extern _iq CurrentSet;
extern _iq DCbus_current;
extern _iq SpeedRef;
extern _iq tempIdc;

extern int iResVal;

typedef struct Motor_Type
{
	Uint16 NewCapValFlag;  //�µ�cap����ֵ��ʶ
	Uint32 CapVal;
	float PWM_Float_val;       //PID�ջ�PWM�����
	float fIdcOut;             //PID��������Ľ��
	int Set_Speed;             //�趨�ٶ�
	int Now_Speed;             //ʵʱ�ٶ�
	int16 PWM_Val;             //����PWM�Ĵ���ռ�ձ�
	Uint16 PWM_OpenVal;        //������������PWMռ�ձ�
    Uint16 OpenLoopSetUpCNT;   //��������ʱ�� ms
	Uint16 MoniSpeedZeroCNT;   //���ڼ��cap�������ֵ�����µļ�����1ms��λ
    float MosFETtemp;          //MOSFET�����¶� ����
    int   MosFETtempI;         //MOSFET�����¶� ����
    Uint16 CloseSPLoopCnt;     //�ٶȱջ����м�ʱ
}DEF_Motor_REG;


extern DEF_Motor_REG MotorADJ;
extern _iq  Spd_Idc;

extern Uint16 CRCcode;
extern char CmdT[];

extern PI_CONTROLLER pid1_spd;
// Instance a SPEED_PR Module
extern SPEED_MEAS_CAP speed1;
extern PI_CONTROLLER pid1_idc;

//CAP
void RgSetRef(void);
void MotorControl(void);
interrupt void main_isr(void);//PWM�жϺ��������жϺ���


///���ݽṹ
typedef struct PID
{
	int FbkPoint; //����ֵ
	int SetPoint; //�趨Ŀ�� Desired Value
	long SumError; //����ۼ�
	double Proportion; //�������� Proportional Const
	double Integral;   //���ֳ��� Integral Const
	double Derivative; //΢�ֳ��� Derivative Const
	int LastError; //Error[-1]
	int PrevError; //Error[-2]
	int Umax;
	int Umin;
	int Fout;
} PID;

extern PID sPID;
extern PID *sptr;

extern PID currentPID;//����PID
extern PID *iptr;

void LocPIDCalc(PID *ptr,Uint16 flag);
void IncPIDInit(void);

unsigned int GetCRC16(char *inPtr, unsigned int len);
void DrawLine(void);
void decodeUart(void);
void DriverCheck(void);

#endif /* MOTOR_PI_H_ */
