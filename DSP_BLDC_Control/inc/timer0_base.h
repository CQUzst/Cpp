/*
 *   timer0_base.h
 *
 *  Created on: 2017-4-6
 *      Author: M新动力电子--科工
 *      欢迎访问淘宝店铺：  https://shop111940119.taobao.com/?spm=2013.1.1000126.d21.403d34adZSGY21
 */

#ifndef TIMER0_BASE_H_
#define TIMER0_BASE_H_

typedef struct timer0_Type{
	union
	  {
	    Uint16 All;
	    struct{
	            Uint16 OnemsdFlag     :1;  //1ms中断标志
	          }Status_Bits;
	  }Mark_Para;

	  Uint32 msCounter;                   //ms 计数
}timer0;

extern timer0 timer0Base;

void Timer0_init(void);


#endif /* TIMER0_BASE_H_ */
