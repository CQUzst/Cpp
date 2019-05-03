/*
 *  Motor_PI.h
 *
 *  Created on: 2017-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef MOTOR_PI_H_
#define MOTOR_PI_H_


#define POLEPAIRS 6 //电机极对数

extern Uint16 hallFindCnt;

extern _iq CurrentSet;
extern _iq DCbus_current;
extern _iq SpeedRef;
extern _iq tempIdc;

extern int iResVal;

typedef struct Motor_Type
{
	Uint16 NewCapValFlag;  //新的cap捕获值标识
	Uint32 CapVal;
	float PWM_Float_val;       //PID闭环PWM的输出
	float fIdcOut;             //PID电流输出的结果
	int Set_Speed;             //设定速度
	int Now_Speed;             //实时速度
	int16 PWM_Val;             //给定PWM寄存器占空比
	Uint16 PWM_OpenVal;        //开环启动给定PWM占空比
    Uint16 OpenLoopSetUpCNT;   //开环启动时间 ms
	Uint16 MoniSpeedZeroCNT;   //周期检测cap捕获测速值不更新的计数，1ms单位
    float MosFETtemp;          //MOSFET管子温度 浮点
    int   MosFETtempI;         //MOSFET管子温度 整数
    Uint16 CloseSPLoopCnt;     //速度闭环运行计时
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
interrupt void main_isr(void);//PWM中断函数，主中断函数


///数据结构
typedef struct PID
{
	int FbkPoint; //反馈值
	int SetPoint; //设定目标 Desired Value
	long SumError; //误差累计
	double Proportion; //比例常数 Proportional Const
	double Integral;   //积分常数 Integral Const
	double Derivative; //微分常数 Derivative Const
	int LastError; //Error[-1]
	int PrevError; //Error[-2]
	int Umax;
	int Umin;
	int Fout;
} PID;

extern PID sPID;
extern PID *sptr;

extern PID currentPID;//电流PID
extern PID *iptr;

void LocPIDCalc(PID *ptr,Uint16 flag);
void IncPIDInit(void);

unsigned int GetCRC16(char *inPtr, unsigned int len);
void DrawLine(void);
void decodeUart(void);
void DriverCheck(void);

#endif /* MOTOR_PI_H_ */
