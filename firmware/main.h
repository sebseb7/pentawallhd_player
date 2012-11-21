#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f4xx.h"

#define PENTAWALLHD
#define LED_WIDTH 24
#define LED_HEIGHT 24

void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nTime);

typedef void (*init_fun)(void);
typedef void (*deinit_fun)(void);
typedef uint8_t (*tick_fun)(void);
void setLedXY(uint8_t x, uint8_t y, uint8_t r,uint8_t g, uint8_t b);

void registerAnimation(init_fun init,tick_fun tick,deinit_fun deinit, uint16_t t, uint16_t duration);

#define CONSTRUCTOR_ATTRIBUTES	__attribute__((constructor));

#define CCM_ATTRIBUTES	__attribute__ ((section(".ccm")));

#endif
