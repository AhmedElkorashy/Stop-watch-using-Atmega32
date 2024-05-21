/*
 * main.c
 *
 *  Created on: Mar 31, 2024
 *      Author: User
 */
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "TIMERS_Interface.h"
#include "TIMERS_Private.h"
#include "DIO_Interface.h"
#include "DIO_Private.h"
#include "EXTI_Interface.h"
#include "EXTI_Private.h"
#include "GIE_Interface.h"
#include <util/delay.h>
volatile u8 Second=0;
volatile u8 Minute=0;
volatile u8 Hour=0;
void INIT0_Reset(void);
void Func_0(void);
void INIT1_Pause(void);
void Func_1(void);
void INIT2_Resume(void);
void Func_2(void);
void TIMER1_Init();
void T_Func();

int main()
{
	DDRA=0X3F;
	PORTA=0X01;
	DDRC=0X0F;
	PORTC=0X00;
	//EXTI_voidInit();
	INIT0_Reset();
	INIT1_Pause();
	INIT2_Resume();
	TIMER1_Init();
	GIE_voidEnable();
	EXTI_u8Int0SetCallBack(Func_0);
	EXTI_u8Int1SetCallBack(Func_1);
	EXTI_u8Int2SetCallBack(Func_2);
	TIMER_u8SetCallBack(T_Func,TIMER1_CTCA_VECTOR_ID);
	while(1)
	{
		/***** shift left PortA*****/
		PORTA=(1<<5);
		PORTC=Second % 10;
		_delay_ms(5);
		PORTA=(1<<4);
		PORTC=Second / 10;
		_delay_ms(5);
		PORTA=(1<<3);
		PORTC=Minute % 10;
		_delay_ms(5);
		PORTA=(1<<2);
		PORTC=Minute / 10;
		_delay_ms(5);
		PORTA=(1<<1);
		PORTC=Hour % 10;
		_delay_ms(5);
		PORTA=(1<<0);
		PORTC=Hour/10;
		_delay_ms(5);
	}

}

void INIT0_Reset(void)
{
	/******pull up*****/
	CLR_BIT(DDRD,DIO_PIN2);
	SET_BIT(PORTD,DIO_PIN2);
	/******* set Exti0 to falling*****/
	MCUCR|=(1<MCUCR_ISC01);
	GICR|=(1<<GICR_INT0);

}
void Func_0(void)
{
	Second=0;
	Minute=0;
	Hour=0;
}
void INIT1_Pause(void)
{
	DDRD&=~(1<<DIO_PIN3);
	MCUCR|=(1<MCUCR_ISC10)|(1<<MCUCR_ISC11);
	GICR|=(1<<GICR_INT1);
	//DIO_u8SetPinDirection(DIO_PORTD,DIO_PIN3,DIO_PIN_INPUT);
}
void Func_1(void)
{
	/*******Clear timsk to stop out compare*****/
	CLR_BIT(TIMSK,TIMSK_OCIE1A);
}
void INIT2_Resume(void)
{
	/******pull up*****/
	CLR_BIT(DDRB,DIO_PIN2);
	SET_BIT(PORTB,DIO_PIN2);

	MCUCR|=(1<MCUCSR_ISC2);
	GICR|=(1<<GICR_INT2);
}
void Func_2(void)
{
	/*********set timsk to resume ***********/
	SET_BIT(TIMSK,TIMSK_OCIE1A);
}
void TIMER1_Init()
{
	/********* non pwm mode*****/
	SET_BIT(TCCR1A,TCCR1A_FOC1A);
	SET_BIT(TCCR1A,TCCR1A_FOC1B);
	/********** prescaler to 64***/
	SET_BIT(TCCR1B,TCCR1B_WGM12);
	SET_BIT(TCCR1B,TCCR1B_CS10);
	SET_BIT(TCCR1B,TCCR1B_CS11);
	TCNT1=0;
	/*********enable interrupt at compare match****/
	SET_BIT(TIMSK,TIMSK_OCIE1A);
}
void T_Func()
{
	Second++;
	if(Second==60)
	{
		Second=0;
		Minute++;
	}
	if(Minute==60)
	{
		Second=0;
		Minute=0;
		Hour++;
	}
	if(Hour==24)
	{
		Second=0;
		Minute=0;
		Hour=0;
	}
}
