																	/************************************************************
																	*字符取模的时候使用逆向逐列模式															*
																	*************************************************************
																	*注意取模需要逆向，逐列模式取模															*
																	*图片取模转换需BMP格式http://pic.55.la/											*
																	*取模用8点阵点阵模式																				*
																	*															哈工大(深圳)WTR战队*欧阳俊源*	*
																	*************************************************************/
#include "oled12864.h"


/* 资源声明 ------------------------------------------------------------------------------------------------------------------------------------*/
OLED_HandleTypeDef  holed={.Init=OLED_Init};
volatile static uint8_t OLED_GDDRAM[128][8];  //显存
/* 资源声明 ------------------------------------------------------------------------------------------------------------------------------------*/



/* 多种通信接口的函数统一定义 ------------------------------------------------------------------------------------------------------------------

    * 多种接口都调用		OLED_WriteRead_Byte(uint8_t data,uint8_t CorD) 	来发送信息
    */

#ifdef _OLED_BY_HW_I2C
void OLED_WriteRead_Byte(uint8_t data,uint8_t CorD)
{
	uint8_t buf[2];
	if(CorD==OLED_CMD)
		buf[0]=0x00;
	else
		buf[0]=0x40;

	buf[1]=data;

	HW_I2C_Transmit(OLED_hi2cn,OLED_DevAddress,buf,2);
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
	SW_I2C_SendByte(OLED_DevAddress);
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
	if(CorD==OLED_CMD)
		OLED_DC_CMD();
	else
		OLED_DC_DATA();

	OLED_CS_RESET();
	while(HW_SPI_Transmit(OLED_hspin,&data,1)!=HAL_OK);
	OLED_CS_SET();


	OLED_DC_DATA();
}

#endif

/* 多种通信接口的函数统一定义 ------------------------------------------------------------------------------------------------------------------*/





/* 绘图函数 -------------------------------------------------------------------------------------------------------------------------------------


		*		0----->X    页的字 LSB在y低的方向上
    * 	|
		*		|
		*		V
		*		Y
    */

void OLED_Refresh(void)
{//循环方式根据GDDRAM寻址方式确定，这里使用页寻址。
	for(uint8_t i=0;i<8;++i)
	{
		OLED_WriteRead_Byte(0XB0+i,OLED_CMD);
		OLED_WriteRead_Byte(0X00,OLED_CMD);
		OLED_WriteRead_Byte(0X10,OLED_CMD);
		for(uint8_t j=0;j<128;++j)
			OLED_WriteRead_Byte(OLED_GDDRAM[j][i],OLED_DATA);
	}
}
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t flag)
{
	if(flag)
		OLED_GDDRAM[x][y/8] |=  (0x01 << (y%8));
	else
		OLED_GDDRAM[x][y/8] &= (~(0x01<<(y%8)));
}


void OLED_Clear(void)
{
	for(int i=0;i<128;++i)
		for(int j=0;j<8;++j)
			OLED_GDDRAM[i][j]=0x00;

	#ifdef PRINT2SCREEN
	extern uint8_t cur_x,cur_y;
	cur_x=0;cur_y=0;
	#endif
}

void OLED_PutChar(uint8_t c,uint8_t size,uint8_t x,uint8_t y)
{

	if( !( c >=' ' &&  c<='~' ) )
		return;
	#ifdef DEFAULT_FONT
	size=DEFAULT_FONT;
	#endif
  uint8_t byte,	len,	idx1=c-' ',	y0=y,	row=1,	col=0,	row_num=size/8 + (size%8?1:0 ) ;

	len= row_num *(size/2);

	for( uint8_t idx2=0 ; idx2	<	len ; idx2=row_num*col+row-1 )
	{
		switch(size)
		{
			case 10:
				byte=asc2_set_1005[idx1][idx2];break;
			case 12:
				byte=asc2_set_1206[idx1][idx2];break;
			case 14:
				byte=asc2_set_1407[idx1][idx2];break;
			case 16:
				byte=asc2_set_1608[idx1][idx2];break;
			case 20:
				byte=asc2_set_2010[idx1][idx2];break;
			case 24:
				byte=asc2_set_2412[idx1][idx2];break;
		}
		row++;
		if(row>row_num)
			row=1;
		for(uint8_t i=0;i<8;++i)
		{
			if(byte&0x01) //逆向取模，左上角是字节低位。
				OLED_DrawPoint(x,y,POINT_DRAW);
			else
				OLED_DrawPoint(x,y,POINT_MOVE);

			//读取下一个bit
			y++;
			byte>>=1;
			if((y-y0)==size ||y>63)
			{
				x++;y=y0;
				col++;row=1;
				break;
			}
		}
	}
}




void OLED_PutString(uint8_t *ptr_str,uint8_t fontsize,uint8_t x,uint8_t y)
{
	uint8_t x0=x;
	#ifdef DEFAULT_FONT
	fontsize=DEFAULT_FONT;
	#endif
	while( *ptr_str !='\0')
	{
		//检擦坐标合法性
		if( (x +fontsize/2 > OLED_MAX_X ) || ((*ptr_str)=='\n') )
		{
			x=x0;
			if( (y+=fontsize) >OLED_MAX_Y)
				return;
		}
		if((*ptr_str)!='\n' )
		{
			OLED_PutChar(*ptr_str,fontsize,x,y);
			x+=fontsize/2;
		}
		++ptr_str;
	}
}
void OLED_PutBuf(uint8_t *buf,uint8_t bufsize,uint8_t fontsize,uint8_t x,uint8_t y)
{
	uint8_t _buf[bufsize+1];
	_buf[bufsize]='\0';
	for(int i=0;i<bufsize;++i)
		_buf[i]=buf[i];
	OLED_PutString(_buf,fontsize,x,y);
}
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
	if( (x1<-1e-6) || (x1>OLED_MAX_X) || (x2<-1e-6)||(x2>OLED_MAX_X)
			|| (y1<-1e-6)||(y1>OLED_MAX_Y)||(y2<-1e-6)||(y2>OLED_MAX_Y))
		return;
	if(x1==x2)
	{
		int sign=(y2>y1 ? 1:-1);
		for(int i=0;i<=sign*(y2-y1);++i)
		{
			OLED_DrawPoint(x1,y1+sign*i,POINT_DRAW);
		}
	}else if(y1==y2)
	{
		int sign=(x2>x1 ? 1:-1);
		for(int i=0;i<=sign*(x2-x1);++i)
		{
			OLED_DrawPoint(x1+ sign*i,y1,POINT_DRAW);
		}
	}else
	{
		int sign=(x2>x1 ? 1:-1);
		float k=((float)y2-y1)/(x2-x1);
		for(int i=0;i<=sign*(x2-x1); ++i)
		{
			OLED_DrawPoint(x1+sign*i,ROUND(y1+ k*i*sign),POINT_DRAW);
		}
	}
}


void OLED_BMP(uint8_t (*bmp)[8])
{
	for(int i=0;i<128;++i)
	{
		for(int j=0;j<8;++j)
		{
			OLED_GDDRAM[i][j]=(*bmp)[j];
		}
		bmp++;
	}
}
/* 绘图函数 -------------------------------------------------------------------------------------------------------------------------------------*/




/* 重定向流--------------------------------------------------------------------------------------------------------------------------------------*/
#ifdef PRINT2SCREEN
const uint8_t _printf_begin_x=0,_printf_begin_y=32;
uint8_t _printf_font_size=10,cur_x=_printf_begin_x,cur_y=_printf_begin_y;

int fputc(int ch, FILE *f)
{

	#ifdef DEFAULT_FONT
	_printf_font_size=DEFAULT_FONT;
	#endif
	if(cur_x+_printf_font_size/2>OLED_MAX_X ||ch=='\n')
	{
		cur_x=_printf_begin_x;
		cur_y+=_printf_font_size;
	}
	if(cur_y>OLED_MAX_Y)
	{
		cur_y=_printf_begin_y;
		holed.Draw.Clear();
	}
	if(ch!='\n')
	{
		holed.Draw.Char((uint8_t)ch,_printf_font_size,cur_x,cur_y);
		cur_x+=_printf_font_size/2;
	}
	return ch;
}
#endif
/* 重定向流--------------------------------------------------------------------------------------------------------------------------------------*/





/* OLED设置--------------------------------------------------------------------------------------------------------------------------------------*/
/*请配合数据手册食用否则看不懂的*/
void OLED_Force_FullScreen(uint8_t is){is==0? OLED_WriteRead_Byte(0XA4,OLED_CMD):OLED_WriteRead_Byte(0XA5,OLED_CMD);}
void OLED_DisplayClockDivide_and_OscillatorFrequency(uint8_t divide,uint8_t frequency){OLED_WriteRead_Byte(0XD5,OLED_CMD);OLED_WriteRead_Byte(divide| (frequency<< 4),OLED_CMD); }
void OLED_Multiplex_Ratio(uint8_t amount){OLED_WriteRead_Byte(0XA8,OLED_CMD);OLED_WriteRead_Byte(amount,OLED_CMD);}
void OLED_Offset(uint8_t offset){	OLED_WriteRead_Byte(0XD3,OLED_CMD);OLED_WriteRead_Byte(offset,OLED_CMD);}//starline 是RAM和ROW之间的映射，而offset是COM和ROW之间的映射
void OLED_StartLine(uint8_t startline){OLED_WriteRead_Byte(0X40+startline,OLED_CMD);}
void OLED_SegmentRemap_LeftTurnRight(uint8_t is){ is==0 ? OLED_WriteRead_Byte(0XA1,OLED_CMD):OLED_WriteRead_Byte(0XA0,OLED_CMD); }
void OLED_COM_Scan_Direction_UpTurnDown(uint8_t is){ is==0 ? OLED_WriteRead_Byte(0XC8,OLED_CMD):OLED_WriteRead_Byte(0XC0,OLED_CMD); }
void OLED_COM_Pins_Hardware_Configuration(){OLED_WriteRead_Byte(0XDA,OLED_CMD);OLED_WriteRead_Byte(0X12,OLED_CMD);}
void OLED_Set_Contrast(uint8_t value){OLED_WriteRead_Byte(0X81,OLED_CMD);OLED_WriteRead_Byte(value,OLED_CMD);}
void OLED_PreChargePeriod(uint8_t phase1,uint8_t phase2){OLED_WriteRead_Byte(0XD9,OLED_CMD);OLED_WriteRead_Byte(phase1|(phase2<<4),OLED_CMD);}
void OLED_Deselect_VCOMH(uint8_t value){OLED_WriteRead_Byte(0XDB,OLED_CMD);OLED_WriteRead_Byte(value<<4,OLED_CMD);}
void OLED_NormalorInverse_Display(uint8_t is){is==0? 	OLED_WriteRead_Byte(0XA6,OLED_CMD):	OLED_WriteRead_Byte(0XA7,OLED_CMD);}
void OLED_Display_OFF(void){OLED_WriteRead_Byte(0X8D,OLED_CMD);/*电荷泵关闭*/OLED_WriteRead_Byte(0X10,OLED_CMD);OLED_WriteRead_Byte(0XAE,OLED_CMD);/*关闭显示*/}
void OLED_Display_ON(void){OLED_WriteRead_Byte(0X8D,OLED_CMD);/*电荷泵开启*/OLED_WriteRead_Byte(0X14,OLED_CMD);OLED_WriteRead_Byte(0XAF,OLED_CMD);/*开启显示*/}
void OLED_AddressingMode_PAGE(void){OLED_WriteRead_Byte(0X20,OLED_CMD);OLED_WriteRead_Byte(0X02,OLED_CMD);}
void OLED_ColumnAddress(uint8_t low,uint8_t high){OLED_WriteRead_Byte(low,OLED_CMD);OLED_WriteRead_Byte(high,OLED_CMD);}
void OLED_Brightness(uint8_t value)/*设置在0-15*/{OLED_Set_Contrast(value<<4);OLED_PreChargePeriod(0x10-value,value);OLED_Deselect_VCOMH(value>>1);}
/* OLED设置--------------------------------------------------------------------------------------------------------------------------------------*/


void OLED_Init(void)
{

	#ifdef _OLED_BY_SPI
	OLED_RST_SET();
	HAL_Delay(100);
	OLED_RST_RESET();
	HAL_Delay(200);
	OLED_RST_SET();
	#endif




	holed.Set.Brightness=OLED_Brightness;
	holed.Set.Display_OFF=OLED_Display_OFF;
	holed.Set.Display_ON=OLED_Display_ON;
	holed.Set.Horizontal_Flip=OLED_SegmentRemap_LeftTurnRight;
	holed.Set.Inverse_Display=OLED_NormalorInverse_Display;
	holed.Set.IsForce_FullScreen=OLED_Force_FullScreen;
	holed.Set.Vertical_Flip=OLED_COM_Scan_Direction_UpTurnDown;
	holed.Set.Refresh=OLED_Refresh;

	holed.Draw.Clear=OLED_Clear;
	holed.Draw.Point=OLED_DrawPoint;
	holed.Draw.Char=OLED_PutChar;
	holed.Draw.String=OLED_PutString;
	holed.Draw.Line=OLED_DrawLine;
	holed.Draw.BMP=OLED_BMP;
	holed.Draw.Buffer=OLED_PutBuf;
	/*------------------------------ OLED 参数设置 ----------------------------*/
	//设置亮度调OLED_Deselect_VCOMH，OLED_PreChargePeriod，OLED_Set_Contrast
	OLED_Display_OFF();//关闭显示
	OLED_DisplayClockDivide_and_OscillatorFrequency(0x00,0x0f);//设置(显示的时钟分频系数-1)和(频率) 取值都在b0000-b1111;
	OLED_Multiplex_Ratio(0x3f); //设置使用显示器的行数，取值在0x00-0x3f(D63)之间。
	OLED_Offset(0x00);//设置行偏置取值在0x00-0x3f之间
	OLED_StartLine(0x00);//设置起始行数取值在00x00-0x3f之间
	OLED_SegmentRemap_LeftTurnRight(0);//设置列细分的重映射，即左右翻转  这里面的值和ssd1306的Datasheet描述不太一样，这是因为中景园类型的OLED的layout设计导致显示上的变化需要改回去。
	OLED_COM_Scan_Direction_UpTurnDown(0);//设置行扫描方向，上下翻转
	OLED_COM_Pins_Hardware_Configuration();
	OLED_Set_Contrast(0xff);
	OLED_PreChargePeriod(0x01,0x0f);//放电、充电CLK周期 只能是1-15。
	OLED_Deselect_VCOMH(0x0f);//0-7,VcomH电平选择
	OLED_Force_FullScreen(0);//无视GDDRAM强制全屏
	OLED_AddressingMode_PAGE();
	OLED_ColumnAddress(0x00,0x10);
	OLED_NormalorInverse_Display(0);//是否反显
	OLED_Display_ON();//开启显示
	/*------------------------------ OLED 参数设置 ----------------------------*/

	OLED_Clear();
	OLED_Refresh();
}
