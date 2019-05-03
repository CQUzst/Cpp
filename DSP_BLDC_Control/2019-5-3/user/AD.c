/*
 * AD.c
 *
 *  Created on: 2017-4-6
 *      Author: M�¶�������--�ƹ�
 *      ��ӭ�����Ա����̣�  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */


#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "HK_all_include.h"

int ChSel[16]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//ADCͨ�����趨
int ACQPS[16]  = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};//ADC���������趨

float RTC_val_k;        // RTC������ֵ
int   intAD_Vol[16]={0};//ADC �������


//#################################################
//ADC���ŷ����Լ���ʼ��
//10��ADC ͨ������
//-------------------------------------------------------------------------
//ChSel[0]=2;		// A2--->IfbSum  UVW��������Ͳ���ͨ��
//ChSel[1]=1;		// ChSelect: ADC A1-> Phase A Current
//ChSel[2]=9;		// ChSelect: ADC B1-> Phase B Current
//ChSel[3]=3;		// ChSelect: ADC A3-> Phase C Current
//ChSel[4]=15;	// ChSelect: ADC B7-> Phase A Voltage
//ChSel[5]=14;	// ChSelect: ADC B6-> Phase B Voltage
//ChSel[6]=12;	// ChSelect: ADC B4-> Phase C Voltage
//ChSel[7]=7;		// ChSelect: ADC A7-> DC Bus  Voltage
//ChSel[8]=6;		// A6 -> RG ������ɵ�����
//ChSel[9]=0;		// A0 -> IPM V_Tsens �������¶�RTC
//##########################################################################
//-----------------------------------------------
void ADC_MACRO_INIT_Dual(int *ChSel2,int *ACQPS)
{
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
	ADC_cal();
	EDIS;
	AdcRegs.ADCTRL3.all = 0x00E0;  /* Power up bandgap/reference/ADC circuits*/
    DELAY_US(50000);               /* Delay before converting ADC channels*/
    //���ϳ�ʼ��28335 ADC�Ĺ̶�����

 	AdcRegs.ADCTRL1.bit.ACQ_PS = ACQPS[0];
	AdcRegs.ADCTRL1.bit.CPS = 1;
	AdcRegs.ADCTRL3.bit.ADCCLKPS =  0;
	AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;   //Cascadedģʽ���ɲɼ����16ͨ�� /* 0x0 Dual Sequencer Mode, 0x1 Cascaded Mode*/
	AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 0x0;
	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 0x1;
	AdcRegs.ADCTRL2.bit.RST_SEQ2 = 0x1;
	AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1=0x1; /* enable SOC from EPWMA trigger*/

	//ͨ������
	AdcRegs.ADCCHSELSEQ1.bit.CONV00 = ChSel2[0];
	AdcRegs.ADCCHSELSEQ1.bit.CONV01 = ChSel2[1];
	AdcRegs.ADCCHSELSEQ1.bit.CONV02 = ChSel2[2];
	AdcRegs.ADCCHSELSEQ1.bit.CONV03 = ChSel2[3];
	AdcRegs.ADCCHSELSEQ2.bit.CONV04 = ChSel2[4];
	AdcRegs.ADCCHSELSEQ2.bit.CONV05 = ChSel2[5];
	AdcRegs.ADCCHSELSEQ2.bit.CONV06 = ChSel2[6];
	AdcRegs.ADCCHSELSEQ2.bit.CONV07 = ChSel2[7];

	AdcRegs.ADCCHSELSEQ3.bit.CONV08 = ChSel2[8];
	AdcRegs.ADCCHSELSEQ3.bit.CONV09 = ChSel2[9];
//	AdcRegs.ADCCHSELSEQ3.bit.CONV10 = ChSel2[10];
//	AdcRegs.ADCCHSELSEQ3.bit.CONV11 = ChSel2[11];
//	AdcRegs.ADCCHSELSEQ4.bit.CONV12 = ChSel2[12];
//	AdcRegs.ADCCHSELSEQ4.bit.CONV13 = ChSel2[13];
//	AdcRegs.ADCCHSELSEQ4.bit.CONV14 = ChSel2[14];
//	AdcRegs.ADCCHSELSEQ4.bit.CONV15 = ChSel2[15];

	AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 9;   //ͨ���� = MAX_CONV1 + 1
	EDIS;
}


//P13 iram136-1561a2.pdf �����¶ȱ��
//��0�ȿ�ʼ��¼��ÿ5���϶�һ������ֵ
float TempBook[]={
		158.3,//0
		122.3,//5
		95.23,//10
		74.73,//15
		59.07,//20
		47,//25
		37.64,//30
		30.33,//35
		24.59,//40
		20.25,//45
		16.43,//50
		13.54,//55
		11.21,//60
		9.328,//65
		7.798,//70
		6.544,//75
		5.518,//80
		4.674,//85
		3.972,//90
		3.388,//95
		2.092,//100
		2.494,//105
		2.150,//110
		1.860,//115
		1.615,//120
		1.406,//125
		1.228,//130
		1.076,//135
		0.9447,//140
		0.8321,//145
		0.7349//150
};

//##########################################################################
//2��һ�ߣ�ͨ�����������5�ȷ�Χ������¶�ֵ
//�������ΪRTC����������ֵ
//����ֵΪ�¶ȣ���ȡ��������
//##########################################################################
float GetTempMosFET(float *rIN)
{
	Uint16 i;
	for(i=0;i<31;i++)
	{
		if(TempBook[i] < *rIN)//���ҵ��ֽ���
		{
			break;
		}

	}
	return  5*(i) - 5.0*(*rIN-TempBook[i])/(TempBook[i-1]-TempBook[i]);
}


//*******************************************************************************
//��ȡIPM�����������ѹ��ת��Ϊ�¶�
void GetIpmTemp(void)
{
	intAD_Vol[AD_TMP] = intAD_Vol[AD_TMP]*0.909; // 28335 Ϊ3V���̵�ADC����Ҫ��3.3V����������0.909�� ��3/3.3��

	//4.7������ 4.7K��������
	RTC_val_k =(float)(4.7*intAD_Vol[AD_TMP])/(4096.0-intAD_Vol[AD_TMP]);//���������������ֵ
	MotorADJ.MosFETtemp = GetTempMosFET(&RTC_val_k);//��ȡ�¶ȣ�����
	MotorADJ.MosFETtempI = MotorADJ.MosFETtemp;//����
}



