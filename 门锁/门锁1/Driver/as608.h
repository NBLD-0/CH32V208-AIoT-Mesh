#ifndef DRIVER_AS608_H_
#define DRIVER_AS608_H_

#include "debug.h"

void as608_init(void);
void PS_GetImage(void);
void PS_GenChar(u8 buffer);
void PS_RegModel(void);
void PS_StoreChar(u8 addr);
void PS_Search(void);
void PS_Empty(void);
void As608_Gpio_Init(void);

#endif
