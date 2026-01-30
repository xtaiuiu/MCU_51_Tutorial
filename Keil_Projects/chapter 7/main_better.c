#include "reg52.h"

typedef unsigned char u8;

/* ================= 接收缓冲区 ================= */
#define RX_BUF_SIZE 16
volatile u8 rx_buf[RX_BUF_SIZE];
volatile u8 rx_head = 0;
volatile u8 rx_tail = 0;

/* ================= 发送缓冲区 ================= */
#define TX_BUF_SIZE 16
volatile u8 tx_buf[TX_BUF_SIZE];
volatile u8 tx_head = 0;
volatile u8 tx_tail = 0;

volatile bit tx_busy = 0;

/* ================= 串口初始化 ================= */
void uart_init(u8 baud)
{
    TMOD &= 0x0F;
    TMOD |= 0x20;      // Timer1 模式2
    SCON  = 0x50;      // 串口模式1，允许接收
    PCON |= 0x80;      // 波特率加倍

    TH1 = baud;
    TL1 = baud;

    ES = 1;
    EA = 1;
    TR1 = 1;
}

/* ================= 串口中断 ================= */
void uart_isr(void) interrupt 4
{
    /* 接收 */
    if (RI)
    {
        RI = 0;
        rx_buf[rx_head] = SBUF;
        rx_head = (rx_head + 1) % RX_BUF_SIZE;
    }

    /* 发送完成 */
    if (TI)
    {
        TI = 0;
        if (tx_tail != tx_head)
        {
            SBUF = tx_buf[tx_tail];
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        }
        else
        {
            tx_busy = 0;   // 真正空闲
        }
    }
}

/* ================= 主函数 ================= */
void main(void)
{
    //uart_init(0xFA);   // 11.0592MHz ≈ 9600bps
	uart_init(0xFD);   // 11.0592MHz ≈ 19200bps

    while (1)
    {
        /* 回显：RX → TX */
        if (rx_tail != rx_head)
        {
            tx_buf[tx_head] = rx_buf[rx_tail];
            tx_head = (tx_head + 1) % TX_BUF_SIZE;
            rx_tail = (rx_tail + 1) % RX_BUF_SIZE;

            if (!tx_busy)
            {
                tx_busy = 1;
                SBUF = tx_buf[tx_tail];
                tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
            }
        }
    }
}
