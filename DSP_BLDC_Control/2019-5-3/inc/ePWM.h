/*
 * ePWM.h
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef __EPWM_H
#define __EPWM_H
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PWM_FRE  3750//SYSCLK/20000  //3750-->20K PWMƵ��

#define MAX_PWM_ZKB 1500//(PWM_FRE-PWM_FRE>>3)//���ռ�ձ�����
#define MIN_PWM_ZKB 20//(PWM_FRE>>4)         //��Сռ�ձ�����
#define BRK_PWM_ZKB 400//(PWM_FRE>>3)         //�ƶ�ռ�ձ�����
#define START_PWM_ZKB 100                     //��������ռ�ձ�
#define TEST_UVW_MAX_ZKB 300                  //����UVW PWM�����ռ�ձ�

#define MIN_IDC 10     //��С���趨��������
#define MAX_IDC 1500    //�����趨��������
#define MIN_SPEED 50   //��С���趨�ٶ�����
#define MAX_SPEED 1500  //�����趨�ٶ�����
#define OPEN_DOWM_MOS_SPEED 10  //��������ٵ����ٶ�ʱ��ȫ��3���¹ܣ��ر�3���Ϲ�
//�ڵ�λʱ���ڣ������ӵ���ٶ���CAP������£����ж�����ٶ�Ϊ��
#define MONI_SPEED_TIMEROUT 60000 //60��û��ת�ټ�¼

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


//���״̬
typedef enum
{
  M_IDLE         = 0,//����ģʽ
  M_PWM_TEST     = 1,//PWM�������ģʽ
  M_UVW_HALL_TEST= 2,//����ͻ�������ģʽ
  M_OPENLOOP     = 3,//��������ģʽ
  M_SPEEDLOOP    = 4,//�ٶȱջ�ģʽ
  M_CURRRENTLOOP = 5,//�����ջ�ģʽ
  M_HALL_PRINTF  = 6//������ӡ����
} MOTOR_MODE;


//���״̬
typedef enum
{
  M_STOP    = 0,//ֹͣ
  M_RUN     = 1,//����
  M_ACCDOWN = 2//���ٶȵ�ͣ��״̬
} MOTOR_STATE;

//M_IDLE         = 0,//����ģʽ
//M_PWM_TEST     = 1,//PWM�������ģʽ
//M_UVW_HALL_TEST= 2,//����ͻ�������ģʽ
//M_OPENLOOP     = 3,//��������ģʽ
//M_SPEEDLOOP    = 4,//�ٶȱջ�ģʽ
//M_CURRRENTLOOP = 5,//�����ջ�ģʽ
//M_HALL_PRINTF  = 6//������ӡ����

typedef union
{
  Uint32 All;
  struct{
          Uint16  EN_IPM         :1;   //IPMʹ�ܱ�ʶ
          Uint16  MotorState     :2;   //���״̬
          Uint16  TestMode       :3;   //�������ģʽ
          Uint16  Idc_Loop_Flag  :1;   //�����ջ���ʶ
          Uint16  UartTestSEGFlag:1;   //����ͨ�Ų�������ܱ�ʶ
          Uint16  DrawLineFlag   :1;   //�����ϴ��������봮��ͼ��������ʹ�ã�����ͼ�λ��۲�
        }Status_Bits;
}Mark_Para;

extern Mark_Para Mark;

//������ʹ��IO����
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

