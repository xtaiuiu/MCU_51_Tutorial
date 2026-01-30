#include "reg52.h"
typedef unsigned int u16;
typedef unsigned char u8;

sbit LED1 = P2^0;
sbit LED2 = P2^1;
sbit BEEP = P2^5;


void delay_10us(u16 ten_us) // 当传入 ten_us = 1时，大约延时 10 us.
{
	while(ten_us--);
}
void main(void)
{
	u16 i = 0;
	u8 key = 0;
	for(i = 0; i < 10000; i++)
	{
		BEEP = ~BEEP;  // 蜂鸣器短响1秒
		delay_10us(10);
	}
	
	while(1)
	{
		key = P3 & 0x0f; //读取P3 I/O 口的低四位（即独立键盘的状态）；
		
		if(key != 0x0f) // 有按键按下时（低电平表示按下），key 的值不为0x0f; 此时，才改变LED2的值
		{
			/*
			软件消抖
			*/
			// 检测到按键按下，先延时10ms
			delay_10us(100);
			key = P3 & 0x0f; //再次读取
			if(key != 0x0f) //再次检测
			{
				LED2 = ~LED2;
				while((P3 & 0x0f) != 0x0f); //等待按键松开，防止前面的代码一直被执行，造成逻辑混乱。
			}
		}
		
		LED1 = ~LED1;  //LED 每500ms闪烁一次
		delay_10us(50000);
	}
}