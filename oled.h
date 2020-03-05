#ifndef __OLED_H
#define __OLED_H

//#define _OLED_BY_HW_I2C
#define _OLED_BY_SW_I2C
//#define _OLED_BY_SPI




#if (defined _OLED_BY_HW_I2C)
	#include "i2c.h"
#elif (defined _OLED_BY_SW_I2C)  
	#include "gpio.h"
#elif (defined _OLED_BY_SPI)
	#include "spi.h"
#else 
	#error missing macro (_OLED_BY_HW_I2C/_OLED_BY_SW_I2C/_OLED_BY_SPI)
#endif


typedef struct
{
#if defined _OLED_BY_HW_I2C
	I2C_HandleTypeDef* hi2cn;
#elif defined _OLED_BY_SW_I2C
	GPIO_TypeDef  *SCL_PORT,	 *SDA_PORT;
	uint16_t 			 SCL_PIN,			SDA_PIN;
#elif defined _OLED_BY_SPI
	SPI_HandleTypeDef *hspin;
	GPIO_TypeDef	*RES_PORT,	*DC_PORT,	*CS_PORT;
	uint16_t 			 RES_PIN,		 DC_PIN,	 CS_PIN;	
#endif 
	uint16_t DevAddress;
}OLED_HandleTypeDef;


extern OLED_HandleTypeDef  holed;



#ifdef _OLED_BY_SW_I2C

	#define OLED_SCL_RESET() HAL_GPIO_WritePin(holed.SCL_PORT,holed.SCL_PIN,GPIO_PIN_RESET)
	#define OLED_SCL_SET() 	 HAL_GPIO_WritePin(holed.SCL_PORT,holed.SCL_PIN,GPIO_PIN_SET)

	#define OLED_SDA_RESET() HAL_GPIO_WritePin(holed.SDA_PORT,holed.SDA_PIN,GPIO_PIN_RESET)
	#define OLED_SDA_SET() 	 HAL_GPIO_WritePin(holed.SDA_PORT,holed.SDA_PIN,GPIO_PIN_SET)

#endif


#ifdef _OLED_BY_SPI

	#define OLED_RST_SET() HAL_GPIO_WritePin(holed.RES_PORT,holed.RES_PIN,GPIO_PIN_SET)
	#define OLED_RST_RESET() HAL_GPIO_WritePin(holed.RES_PORT,holed.RES_PIN,GPIO_PIN_RESET)

	#define OLED_DC_SET() HAL_GPIO_WritePin(holed.DC_PORT,holed.DC_PIN,GPIO_PIN_SET)
	#define OLED_DC_RESET() HAL_GPIO_WritePin(holed.DC_PORT,holed.DC_PIN,GPIO_PIN_RESET)

	#define OLED_CS_SET() HAL_GPIO_WritePin(holed.CS_PORT,holed.CS_PIN,GPIO_PIN_SET)
	#define OLED_CS_RESET() HAL_GPIO_WritePin(holed.CS_PORT,holed.CS_PIN,GPIO_PIN_RESET)

	#define OLED_DC_CMD() OLED_DC_RESET()
	#define OLED_DC_DATA() OLED_DC_SET()

#endif



#define OLED_CMD 0
#define OLED_DATA 1


void OLED_Refresh(void);
void OLED_DrawPoint(uint8_t x,uint8_t y);
void OLED_Clear(void);
void OLED_Init(void);




#endif 

