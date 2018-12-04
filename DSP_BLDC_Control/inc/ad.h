/*
 * ad.h
 *
 *  Created on: 2018-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef AD_H_
#define AD_H_

//ChSel[0]=2;		// A2--->IfbSum
//ChSel[1]=1;		// ChSelect: ADC A1-> Phase A Current
//ChSel[2]=9;		// ChSelect: ADC B1-> Phase B Current
//ChSel[3]=3;		// ChSelect: ADC A3-> Phase C Current
//ChSel[4]=15;	// ChSelect: ADC B7-> Phase A Voltage
//ChSel[5]=14;	// ChSelect: ADC B6-> Phase B Voltage
//ChSel[6]=12;	// ChSelect: ADC B4-> Phase C Voltage
//ChSel[7]=7;		// ChSelect: ADC A7-> DC Bus  Voltage
//ChSel[8]=6;		// A6 -> RG
//ChSel[9]=0;		// A0 -> IPM V_Tsens

//定义ADC的SOC位置
typedef enum
{
  AD_ISUM  = 0,//电流和
  AD_RG    = 1,//驱动板可调电阻
  AD_TMP   = 2 //IPM温度RTC输出
} ADC_INDEX;

extern int ChSel[];
extern int ACQPS[];
extern float RTC_val_k;
extern int intAD_Vol[];

void GetIpmTemp(void);
float GetTempMosFET(float *rIN);
void ADC_MACRO_INIT_Dual(int *ChSel2,int *ACQPS );

#endif /* AD_H_ */
