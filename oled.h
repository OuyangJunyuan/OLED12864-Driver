#ifndef __OLED_H
#define __OLED_H
#include "oledfont.h"
/*----------------------------------------------- 通信方式选择 ---------------------------------------------*/
//#define _OLED_BY_HW_I2C
//#define _OLED_BY_SW_I2C
#define _OLED_BY_SPI

/*----------------------------------------------- 通信方式选择 ---------------------------------------------*/
#if (defined _OLED_BY_HW_I2C)
	#include "i2c.h"
/*---------------------------------------------- 硬件I2C设置地点 -------------------------------------------*/
	#define OLED_hi2cn &hi2c1
	#define HW_I2C_Transmit(__HANDLE,__ADDRESS,__BUFFER,__SIZE) HAL_I2C_Master_Transmit(__HANDLE,__ADDRESS,(uint8_t*)__BUFFER,__SIZE,0xffff)
	
/*---------------------------------------------- 硬件I2C设置地点 -------------------------------------------*/	
#elif (defined _OLED_BY_SW_I2C)
	#include "gpio.h"
/**-------------------------------------------- 软件I2C设置地点 -------------------------------------------
	*设置指南：
	*SCL和SDA可以使用任意一个GPIO管脚
	*最好设置成开漏上拉超高速模式
	*APB2时钟设置为36即可。超过了可能不工作
***/
	#define SCL_PORT GPIOA
	#define SCL_PIN	GPIO_PIN_5
	
	#define SDA_PORT GPIOA
	#define SDA_PIN GPIO_PIN_7
/*---------------------------------------------- 软件I2C设置地点 -------------------------------------------*/
	#define OLED_SCL_RESET() HAL_GPIO_WritePin(SCL_PORT,SCL_PIN,GPIO_PIN_RESET)
	#define OLED_SCL_SET() 	 HAL_GPIO_WritePin(SCL_PORT,SCL_PIN,GPIO_PIN_SET)

	#define OLED_SDA_RESET() HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_RESET)
	#define OLED_SDA_SET() 	 HAL_GPIO_WritePin(SDA_PORT,SDA_PIN,GPIO_PIN_SET)
#elif (defined _OLED_BY_SPI)
	#include "spi.h"
/**---------------------------------------------- 硬件SPI设置地点 -------------------------------------------
	*设置指南：
	*SPI可以设置成 摩托罗拉格式，8位数据长度，高位先行，预分频2，
	*时钟和相位可以设置成LOW-1Edge或者HIGH-2Edge 都行，失能RCC校验，NSS软件选择模式。
	*为了提高刷新率，可以使用SPI1-其使用APB2外设时钟， 
	*												SPI2-其使用APB1外设时钟，
	*选择APB1和APB2外设时钟频率最高的那个，配置到最高。如F4用SPI1可以达到42M的波特率
	*其余RES、DC、CS 设置成推挽输出即可。
***/
	
	#define OLED_hspin &hspi1
	
	#define RES_PORT GPIOB
	#define RES_PIN GPIO_PIN_0
	
	#define	DC_PORT GPIOB
	#define	DC_PIN GPIO_PIN_1
	
	#define	CS_PORT	GPIOB
	#define	CS_PIN GPIO_PIN_10
/*---------------------------------------------- 硬件SPI设置地点 -------------------------------------------*/
	
	
	#define HW_SPI_Transmit(__HANDLE,__BUFFER,__SIZE) HAL_SPI_Transmit(__HANDLE,__BUFFER,__SIZE,0xffffff)
	#define OLED_RST_SET() HAL_GPIO_WritePin(RES_PORT,RES_PIN,GPIO_PIN_SET)
	#define OLED_RST_RESET() HAL_GPIO_WritePin(RES_PORT,RES_PIN,GPIO_PIN_RESET)

	#define OLED_DC_SET() HAL_GPIO_WritePin(DC_PORT,DC_PIN,GPIO_PIN_SET)
	#define OLED_DC_RESET() HAL_GPIO_WritePin(DC_PORT,DC_PIN,GPIO_PIN_RESET)

	#define OLED_CS_SET() HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_SET)
	#define OLED_CS_RESET() HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_RESET)

	#define OLED_DC_CMD() OLED_DC_RESET()
	#define OLED_DC_DATA() OLED_DC_SET()
#else 
	#error missing macro (_OLED_BY_HW_I2C/_OLED_BY_SW_I2C/_OLED_BY_SPI)
#endif



void OLED_Init(void);
typedef struct
{
	void (*Point)(uint8_t x,uint8_t y,uint8_t flag);
	void (*Clear)(void);
	void (*Line)(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2);
	void (*Char)(uint8_t c,uint8_t size,uint8_t x,uint8_t y);
	void (*String)(uint8_t *ptr_str,uint8_t size,uint8_t x,uint8_t y);
	void (*BMP)(uint8_t (*bmp)[8]);
}DRAW_Ptr;

typedef struct
{
void (*Brightness)(uint8_t value);
void (*IsForce_FullScreen)(uint8_t is);
void (*Horizontal_Flip)(uint8_t is);
void (*vertical_Flip)(uint8_t is);
void (*Inverse_Display)(uint8_t is);
void (*Display_OFF)(void);
void (*Display_ON)(void);	
void (*Refresh)(void);
}SET_Ptr;


typedef struct
{
	void (*Init)(void);
	DRAW_Ptr Draw;
	SET_Ptr Set;
}OLED_HandleTypeDef;

extern OLED_HandleTypeDef  holed;
extern unsigned char bmp1[][8];
#define OLED_DevAddress  0x78
#define OLED_CMD 0
#define OLED_DATA 1
#define POINT_DRAW 1
#define POINT_MOVE 0
#define OLED_MAX_X 127
#define OLED_MAX_Y 63

















#define ROUND(__ROUND_X) ((int)(__ROUND_X+0.5))



#endif 





/********************************************
*重定向C语言库函数：可以print在屏幕上。
*
*********************************************/






