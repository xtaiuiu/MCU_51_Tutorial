#include "reg52.h"
typedef unsigned int u16;
typedef unsigned char u8;

sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit BEEP = P2^5;
sbit KEY  = P3^2;

u16 g_beep_cnt = 0;
u16 g_led1_cnt = 0;  //为LED1 统计经过了多少个100us，当该值达到5000时，LED1 闪烁并重置该值
bit g_beep_ctl = 1;
bit key_flag = 0;  // key 是否按下的 flag
u16 g_key_cnt = 0;  // key 消抖时延计数，单位100us.

void delay_10us(u16 ten_us) // 当传入 ten_us = 1时，大约延时 10 us.
{
	while(ten_us--);
}


//采用定时器实现，要求：只使用T0一个计时器

void timer0_serv(void) interrupt 1
{
	/****************************************
	*** 蜂鸣器控制
	****************************************/
	if(1 == g_beep_ctl)
	{
		BEEP = ~BEEP;  //每隔 100us 蜂鸣器电平翻转一次，即可发出频率为5000的音频
		if(++g_beep_cnt > 60000)
		{
			BEEP = 0; // 关闭蜂鸣器
			g_beep_ctl = 0; // 时间到，停止蜂鸣器。下次进入此中断服务程序后，BEEP相关逻辑均失效（即不再响应）
			//TH0 = 0x3c; TL0 = 0xb0; //装入 LED1 闪烁 Timer的初值
		}
	}
	/****************************************
	*** LED1 闪烁控制
	****************************************/
	if(++g_led1_cnt >= 5000)
	{
		LED1 = ~LED1;
		g_led1_cnt = 0;
	}
	/****************************************
	*** 按键控制 LED2。 先消抖，再闪烁LED2
	****************************************/
	if(key_flag)
	{
		if(KEY == 0)
		{
			if(++g_key_cnt >= 20)
			{
				LED2 = ~LED2;
				
				/*****************************************
				* 关键：处理完一次就清。如果不清，下次Timer溢出后（可能很快溢出，此时还按着键），LED会再翻转一次
				* 时间快的话会看不清楚LED的状态有没有被翻转
				*****************************************/
				g_key_cnt = 0;
				key_flag  = 0;   
			}
		}
		else
		{
			g_key_cnt = 0;
			key_flag = 0;
		}
	}
	
	
	TH0 = 0xff; TL0 = 0x9c; //重装 Timer0 初值（Timer溢出后，不会重装初值，只会重置溢出位）
}	

void int0_serv(void) interrupt 0
{
	/****************************************
	*** 按键控制 LED2, 当P3^2处的按键按下时，将按键标志置1
	*** 此处只设置标志位，不放置其他逻辑。消抖和LED2闪烁放在Timer 0的中断服务函数中
	****************************************/
	key_flag = 1;  // 按键“疑似”被按下
	delay_10us(50000);
}

void main(void)
{
	//u8 key;
	EA = 1;
	ET0 = 1; //允许定时器中断
	EX0 = 1; //允许外部中断0中断
	IT0 = 1; //下降沿触发，避免按键长时间按下后一直触发此中断
	TMOD = 0x01;  // Timer 0 采用 16位定时器模式（方式1）
	TH0 = 0xff;
	TL0 = 0x9c;
	TR0 = 1; // Timer 0 启动
	while(1)
	{
		delay_10us(50000);
	}
}