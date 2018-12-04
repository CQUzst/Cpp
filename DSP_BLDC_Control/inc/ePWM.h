/*
 * ePWM.h
 *
 *  Created on: 2017-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef __EPWM_H
#define __EPWM_H
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PWM_FRE  3750//SYSCLK/20000  //3750-->20K PWM频率

#define MAX_PWM_ZKB 1500//(PWM_FRE-PWM_FRE>>3)//最大占空比设置
#define MIN_PWM_ZKB 20//(PWM_FRE>>4)         //最小占空比设置
#define BRK_PWM_ZKB 400//(PWM_FRE>>3)         //制动占空比设置
#define START_PWM_ZKB 100                     //开环启动占空比
#define TEST_UVW_MAX_ZKB 300                  //测试UVW PWM输出的占空比

#define MIN_IDC 10     //最小的设定电流限制
#define MAX_IDC 1500    //最大的设定电流限制
#define MIN_SPEED 50   //最小的设定速度限制
#define MAX_SPEED 1500  //最大的设定速度限制
#define OPEN_DOWM_MOS_SPEED 10  //当电机减速到该速度时，全开3个下管，关闭3个上管
//在单位时间内，若监视电机速度无CAP捕获更新，则判定电机速度为零
#define MONI_SPEED_TIMEROUT 60000 //60秒没有转速记录

extern Uint16 Current_MOSFET_State;

void HALL_0x61(void);
void HALL_0x56(void);
void HALL_0x45(void);
void HALL_0x34(void);
void HALL_0x23(void);
void HALL_0x12(void);

void HallNull(void);
void Motor_TzGpioInit(void);

void InitEPwm_AQ_DB(void);
void openLowBrige(void);

void InitEPWM_Hpwm_Lopen(void);
void DspTzInit(void);
void TZ_OSHT_Ctr(Uint16 enFlag);


//电机状态
typedef enum
{
  M_IDLE         = 0,//空闲模式
  M_PWM_TEST     = 1,//PWM输出测试模式
  M_UVW_HALL_TEST= 2,//绕组和霍尔测试模式
  M_OPENLOOP     = 3,//开环运行模式
  M_SPEEDLOOP    = 4,//速度闭环模式
  M_CURRRENTLOOP = 5,//电流闭环模式
  M_HALL_PRINTF  = 6//霍尔打印测试
} MOTOR_MODE;


//电机状态
typedef enum
{
  M_STOP    = 0,//停止
  M_RUN     = 1,//运行
  M_ACCDOWN = 2//减速度到停机状态
} MOTOR_STATE;

//M_IDLE         = 0,//空闲模式
//M_PWM_TEST     = 1,//PWM输出测试模式
//M_UVW_HALL_TEST= 2,//绕组和霍尔测试模式
//M_OPENLOOP     = 3,//开环运行模式
//M_SPEEDLOOP    = 4,//速度闭环模式
//M_CURRRENTLOOP = 5,//电流闭环模式
//M_HALL_PRINTF  = 6//霍尔打印测试

typedef union
{
  Uint32 All;
  struct{
          Uint16  EN_IPM         :1;   //IPM使能标识
          Uint16  MotorState     :2;   //电机状态
          Uint16  TestMode       :3;   //电机测试模式
          Uint16  Idc_Loop_Flag  :1;   //电流闭环标识
          Uint16  UartTestSEGFlag:1;   //串口通信测试数码管标识
          Uint16  DrawLineFlag   :1;   //串口上传参数，与串口图形软件配合使用，进行图形化观测
        }Status_Bits;
}Mark_Para;

extern Mark_Para Mark;

//驱动板使能IO定义
#define ENGPIO GPIO52
#define oENIPMMUX GpioCtrlRegs.GPBMUX2.bit.ENGPIO
#define oENIPMDIR GpioCtrlRegs.GPBDIR.bit.ENGPIO
#define oENSET()  GpioDataRegs.GPBCLEAR.bit.ENGPIO =1
#define oENCLEAR() GpioDataRegs.GPBSET.bit.ENGPIO =1

typedef void(*pfnvoid)(void);
extern pfnvoid pfnFW[];

extern Uint16 FWHall1[];//---A
extern Uint16 FWHall2[];//---B
extern Uint16 FWHall3[];//---C
extern Uint16 FWHall4[];//---D
extern Uint16 FWHall5[];//---E
extern Uint16 FWHall6[];//---F

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Gpio_IPM_Init(void);
void Gpio_EN_IPM_Init(void);
Uint16 ReadHall(void);
void SetPWMVal(int PWMVal);
void MOS_HX(Uint16 *HallState);
void printfHall(Uint16 hallR);

#endif //END __EPWM_H

