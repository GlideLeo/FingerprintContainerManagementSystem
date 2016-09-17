#ifndef _USART_H
#define _USART_H

#include <stdio.h>
#include "stm32f10x.h"


void usart_Configuration(u32 USART_BaudRate, u32 USART2_BaudRate);
static void Rcc_Configuration(void);
static void UsartGPIO_Configuration(void);
static void UsartNVIC_Configuration(void);

#endif /*_USART_H*/
