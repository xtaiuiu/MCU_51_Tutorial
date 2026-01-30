#include "reg52.h"
sbit key = P3^2;  //接INT0
sbit led = P2^0;

typedef enum
{
	KEY_STATE_IDL,				// 等待按键
	KEY_STATE_PRESS_DEBOUNCE,			// 按键消抖
	KEY_STATE_WAIT_RELEASE,		// 等待按键释放
	KEY_STATE_RELEASE_DEBOUNCE,			// 释放消抖
}t_key_state;
typedef unsigned char	u8;
typedef unsigned int	u16;

volatile t_key_state key_state = KEY_STATE_IDL;



/* ================= 系统时钟 ================= */
volatile u16 g_sys_tick = 0; // 系统时钟，单位：10ms
/* =============== Timer0 中断 =============== */
void timer0_isr(void) interrupt 1
{
	TH0 = 0xDC; // （11.0592MHz）1ms, 10ms, 50ms时，重装值分别为0xFC66， 0xDC00，0x4C00
	TL0 = 0x00;
	g_sys_tick++; // 系统时间前进一步
}

void key_scan_with_debounce(void)
{
	static u16 debounce_last_tick = 0;  //一个按键同一时刻只可能处于按键消抖或释放消抖之一，因此只需要一个tick即可。
	switch(key_state)
	{
		case KEY_STATE_IDL: 
			if(key == 0)
			{	debounce_press_last_tick = g_sys_tick;  //记录消抖开始时间
				key_state = KEY_STATE_PRESS_DEBOUNCE;
			}
			break;
		case KEY_STATE_PRESS_DEBOUNCE:
			if(g_sys_tick - debounce_press_last_tick >= 1)
			{
				debounce_press_last_tick = g_sys_tick;
				if(key == 0)
				{
					key_state = KEY_STATE_WAIT_RELEASE;
					led = 0;  //按键后，LED亮
				}
				else
					key_state = KEY_STATE_IDL;
			}				
			break;
		case KEY_STATE_WAIT_RELEASE:
			if(key == 1)
			{
				debounce_release_last_tick = g_sys_tick; //记录释放消抖开始时间
				key_state = KEY_STATE_RELEASE_DEBOUNCE;
			}
			break;
		case KEY_STATE_RELEASE_DEBOUNCE:
			if(g_sys_tick - debounce_release_last_tick >= 1)
			{
				debounce_release_last_tick = g_sys_tick;
				if(key == 1)
				{
					key_state = KEY_STATE_IDL;
					led = 1;  //按键释放后，LED灭
				}
				else
					key_state = KEY_STATE_WAIT_RELEASE;
			}
			break;
	}
}

void init_timer0(void)
{
	TMOD &= 0xF0;	// 清 Timer0 控制位
	TMOD |= 0x01; 	// Timer0 工作方式 1（16 位）
	TH0 = 0xDC;
	TL0 = 0x00;
	ET0 = 1; 	// 允许 Timer0 中断
	EA  = 1;	// 开总中断
	TR0 = 1;	// 启动 Timer0
	EX0 = 1;  // 使能中断0
}

void main(void)
{
	init_timer0();
	while(1)
	{
		key_scan_with_debounce();
	}
}
