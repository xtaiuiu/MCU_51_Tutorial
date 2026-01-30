#include <reg52.h>
#include "music_tone.h"

sbit BEEP = P2^5;

typedef unsigned int  u16;
typedef unsigned char u8;

/* ================= 音高相关（Timer1） ================= */

u8 TH_reload;
u8 TL_reload;

/* C 调 1~7（12MHz 晶振，近似值） */
u8 TH_arr[] = {0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFB, 0xFC};
u8 TL_arr[] = {0x8C, 0x5B, 0x15, 0x67, 0x04, 0x90, 0x0C};

/* ================= 音长相关（Timer0） ================= */

volatile u16 tick_cnt = 0;
u16 note_len_ms = 0;

/* 四分音符时长（整体速度控制） */
#define BEAT_MS 500   // 可改：300 快，700 慢

/* ================= Timer1：产生音高 ================= */

void timer1_ISR(void) interrupt 3
{
    BEEP = ~BEEP;
    TH1 = TH_reload;
    TL1 = TL_reload;
}

/* ================= Timer0：1ms 节拍 ================= */

void timer0_ISR(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x18;   // 1ms @12MHz

    if (++tick_cnt >= note_len_ms)
    {
        TR1 = 0;      // 停止发声
        TR0 = 0;      // 停止节拍
        tick_cnt = 0;
    }
}

/* ================= 播放一个音符 ================= */

void play_one_tone(u8 tone, u16 duration_ms)
{
    EA = 0;
    TH_reload = TH_arr[tone - 1];
    TL_reload = TL_arr[tone - 1];
    tick_cnt = 0;
    note_len_ms = duration_ms;
    EA = 1;

    TR1 = 1;   // 启动音高定时器
    TR0 = 1;   // 启动节拍定时器

    while (TR0);   // 等待音符播放结束
}

void delay_mu_s(u16 mu_s)
{
	while(mu_s--);
}

/* ================= 主函数 ================= */

void main(void)
{
    u16 i = 0;
	char * song =  twinkle;
	u8 len = sizeof(twinkle)/sizeof(twinkle[0]);

    TMOD = 0x11;   // Timer0、Timer1 均为模式1
    EA  = 1;
    ET0 = 1;
    ET1 = 1;
	

    while (1)
    {
		for(i = 0; i < len; i++)
		{
			if (song[i] > 0)
                play_one_tone(song[i], BEAT_MS);
            else
                play_one_tone(-song[i], BEAT_MS * 2);
		}
		delay_mu_s(5000000);
    }
}
