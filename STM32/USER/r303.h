#ifndef _R303_H
#define _R303_H
#include <stdio.h>
#include "stm32f10x.h"
void r303_Save(unsigned int PageID);
unsigned char r303_Search(void);
void r303_ValidTempleteNum(void);
void r303_StoreChar(unsigned int PageID);
void r303_RegModel(void);
void r303_ImageToCharBuffer2(void);
void r303_ImageToCharBuffer1(void);
void r303_GetImage(void);
void r303_delay(unsigned int k);
#endif