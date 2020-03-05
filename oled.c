#include "oled.h"

//******************************************** 资源声明  ********************************************//
OLED_HandleTypeDef  holed;
volatile static uint8_t OLED_GDDRAM[128][8];  //显存
//******************************************** 资源声明  ********************************************//












//*********************************** 多种接口的通信函数统一定义  ***********************************//
//多种接口都调用OLED_WriteRead_Byte(uint8_t data,uint8_t CorD) 来发送信息
#ifdef _OLED_BY_HW_I2C
void OLED_WriteRead_Byte(uint8_t data,uint8_t CorD)
{
	uint8_t buf[2]; 
	if(CorD==OLED_CMD)
		buf[0]=0x00;
	else
		buf[0]=0x40;
	
	buf[1]=data;
	
	HAL_I2C_Master_Transmit(holed.hi2cn,holed.DevAddress,(uint8_t*)buf,2,0xffff);
}

#endif 




#ifdef _OLED_BY_SW_I2C
void SW_I2C_Start_Signal(void)
{
	OLED_SDA_SET();
	OLED_SCL_SET();
	OLED_SDA_RESET();
	OLED_SCL_RESET();
}


void SW_I2C_Stop_Signals(void)
{
	OLED_SCL_SET();
	OLED_SDA_RESET();
	OLED_SDA_SET();
}

void SW_I2C_WaitAck(void) 
{
	OLED_SCL_SET();
	OLED_SCL_RESET();
}

void SW_I2C_SendByte(uint8_t data)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		OLED_SCL_RESET();
		if(data&0x80)
		{
			OLED_SDA_SET();
    }
		else
		{
			OLED_SDA_RESET();
    }
		OLED_SCL_SET();
		OLED_SCL_RESET();
		data<<=1;
  }
}

void OLED_WriteRead_Byte(uint8_t data,uint8_t CorD)
{
	SW_I2C_Start_Signal();
	SW_I2C_SendByte(holed.DevAddress);
	SW_I2C_WaitAck();
	if(CorD)
	{
		SW_I2C_SendByte(0x40);
	}
  else
	{
		SW_I2C_SendByte(0x00);
	}
	SW_I2C_WaitAck();
	SW_I2C_SendByte(data);
	SW_I2C_WaitAck();
	SW_I2C_Stop_Signals();
}



#endif 




#ifdef _OLED_BY_SPI






void OLED_WriteRead_Byte(uint8_t data,uint8_t CorD)
{
	uint8_t read;
	if(CorD==OLED_CMD)
		OLED_DC_CMD();
	else
		OLED_DC_DATA();
	
	OLED_CS_RESET();
	while(HAL_SPI_Transmit(holed.hspin,&data,1,0xffffff)!=HAL_OK);
	OLED_CS_SET();
	
	
	OLED_DC_DATA();
}
	
#endif

//*********************************** 多种接口的通信函数统一定义  ***********************************//













//************************************** 关于绘画的新函数放在这里  ***********************************//
void OLED_Refresh(void)
{
	for(uint8_t i=0;i<8;++i)
	{		
		OLED_WriteRead_Byte(0XB0+i,OLED_CMD);
		OLED_WriteRead_Byte(0X00,OLED_CMD);
		OLED_WriteRead_Byte(0X10,OLED_CMD);
		for(uint8_t j=0;j<128;++j)
		{
			OLED_WriteRead_Byte(OLED_GDDRAM[j][i],OLED_DATA);
		}
	}
}

void OLED_DrawPoint(uint8_t x,uint8_t y)
{
	OLED_GDDRAM[x][y/8] |= 0x01<< (y%8);
}




void OLED_Clear(void)
{
	for(int i=0;i<128;++i)
	{
		for(int j=0;j<8;++j)
		{
			OLED_GDDRAM[i][j]=0x00;
		}	
	}
	
}




//************************************** 关于绘画的新函数放在这里  ***********************************//






















//*********************************** 用户从这里开始自定义配置接口  ***********************************//
void OLED_Init(void)
{


	/*----------------------- 从机地址 -----------------------*/
	holed.DevAddress=0x78;
	/*----------------------- 从机地址 -----------------------*/

	/*------------------------- 选择硬件I2C的在这里配置 ---------------------*/									
	//设置指南：
	//将 hi2c1改成你所使用的i2c_n的句柄
	#ifdef _OLED_BY_HW_I2C
	holed.hi2cn=&hi2c1;
	#endif
	/*------------------------- 选择硬件I2C的在这里配置 ---------------------*/	

	/*--------------------------选择软件I2C的在这里配置 ---------------------*/
	//设置指南：
	//SCL和SDA可以使用任意一个GPIO管脚
	//最好设置成开漏上拉超高速模式
	//APB2时钟设置为36即可。超过了可能不工作
	#ifdef _OLED_BY_SW_I2C
	
	holed.SCL_PORT=GPIOA;
	holed.SCL_PIN=GPIO_PIN_5;
	holed.SDA_PORT=GPIOA;
	holed.SDA_PIN=GPIO_PIN_7;
	
	#endif	
	/*---------------------------- 选择软件I2C的在这里配置 ---------------------*/

	/*--------------------------- 选择SPI的在这里设置 -------------------------*/
	//设置指南：
	//SPI可以设置成 摩托罗拉格式，8位数据长度，高位先行，预分频2，时钟和相位可以设置成LOW-1Edge或者HIGH-2Edge 都行，失能RCC校验，NSS软件选择模式。
	//为了提高刷新率，可以使用SPI1-其使用APB2外设时钟， 选择APB1和APB2外设时钟频率最高的那个，配置到最高。如F4用SPI1可以达到42M的波特率
	//												SPI2-其使用APB1外设时钟，
	//其余RES、DC、CS 设置成推挽输出即可。
	#ifdef _OLED_BY_SPI

	holed.hspin=&hspi1;
	holed.RES_PORT=GPIOF;
	holed.RES_PIN=GPIO_PIN_13;
	holed.DC_PORT=GPIOF;
	holed.DC_PIN=GPIO_PIN_14;
	holed.CS_PORT=GPIOF;
	holed.CS_PIN=GPIO_PIN_15;
	OLED_RST_SET();   
	HAL_Delay(100);		
	OLED_RST_RESET();
	HAL_Delay(200);
	OLED_RST_SET();
	
	#endif
	/*--------------------------- 选择SPI的在这里设置 -------------------------*/	

	
	
	/*------------------------------ OLED 参数设置 ----------------------------*/
	OLED_WriteRead_Byte(0XAE,OLED_CMD);	//关闭显示
	
	OLED_WriteRead_Byte(0XD5,OLED_CMD);
	OLED_WriteRead_Byte(0X80,OLED_CMD);  //可能可以改成f0
	
	OLED_WriteRead_Byte(0XA8,OLED_CMD);
	OLED_WriteRead_Byte(0X3F,OLED_CMD);
	
	OLED_WriteRead_Byte(0XD3,OLED_CMD);
	OLED_WriteRead_Byte(0X00,OLED_CMD);
	
	OLED_WriteRead_Byte(0X40,OLED_CMD);
	
	OLED_WriteRead_Byte(0X8D,OLED_CMD);
	OLED_WriteRead_Byte(0X14,OLED_CMD);
	
	OLED_WriteRead_Byte(0X20,OLED_CMD);
	OLED_WriteRead_Byte(0X02,OLED_CMD);
	
	OLED_WriteRead_Byte(0XA1,OLED_CMD);
	
	OLED_WriteRead_Byte(0XC8,OLED_CMD);
	
	OLED_WriteRead_Byte(0XDA,OLED_CMD);
	OLED_WriteRead_Byte(0X12,OLED_CMD);

	OLED_WriteRead_Byte(0X81,OLED_CMD);
	OLED_WriteRead_Byte(0XEF,OLED_CMD);
	
	OLED_WriteRead_Byte(0XD9,OLED_CMD);
	OLED_WriteRead_Byte(0XF1,OLED_CMD);
	
	OLED_WriteRead_Byte(0XDB,OLED_CMD);
	OLED_WriteRead_Byte(0X30,OLED_CMD);
	
	OLED_WriteRead_Byte(0XA4,OLED_CMD);
	OLED_WriteRead_Byte(0XA6,OLED_CMD);
	OLED_WriteRead_Byte(0XAF,OLED_CMD);
	
	//OLED_Force_FullScreen();
	/*------------------------------ OLED 参数设置 ----------------------------*/


	OLED_Clear();
	OLED_Refresh();
}
