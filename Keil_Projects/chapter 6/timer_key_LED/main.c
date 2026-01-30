/*******************************************
** 时间控制的LED灯
** 功能：用timer 0 控制LED 1 ~ 8 每50ms, 100ms, 150ms等翻转一次
** 作用：练习用timer生成系统时钟，并用时间差控制不同的时序，从而控制不同的外设
********************************************/
#include <reg52.h>
typedef unsigned char u8;
typedef unsigned int u16;

volatile u8 g_ms_tick = 0;  //全局时钟，单位：50 ms

void timer0_serv(void) interrupt 1  //Timer 0的中断服务函数 （系统时钟）
{
	TH0 = 0x4c;
	TL0 = 0x00;
	g_ms_tick++;
}

void led_blink(u8 idx, u16 fifty_ms) //控制 LED idx 的翻转， fifty_ms:每隔 fifty_ms 翻转一次
{
	static u16 led_last_tick[] = {0, 0, 0, 0, 0, 0, 0, 0};
	u16 diff = g_ms_tick - led_last_tick[idx]; //使用时间差，即使溢出，差值也很大，不会出问题
	if(diff >= fifty_ms)
		{
			P2 ^= (1 << idx); // 第idx位取反
			led_last_tick[idx] = g_ms_tick;
		}
}

void set_timer0(void)
{
	// Set interrupter
	EA = 1;
	ET0 = 1;
	// Set timer mode
	TMOD |= 0x01;
	//Set initial value for the timer
	TH0 = 0x4c;
	TL0 = 0x00;
	TR0 = 1;  //开启Timer 0
}

void main(void)
{
	set_timer0();
	while (1)
	{
		u8 i = 0;
		for(i = 0; i < 8; i++)
		{
			led_blink(i, (i+1)*10);
		}
	}
}
