																	/************************************************************
																	*字符取模的时候使用逆向逐列模式															*
																	*************************************************************
																	*注意取模需要逆向，逐列模式取模															*
																	*图片取模转换需BMP格式http://pic.55.la/											*	
																	*取模用8点阵模式																				*
																	*															哈工大(深圳)WTR战队*欧阳俊源*	*
																	*************************************************************/
																	


#ifndef __OLEDFONT_H
#define __OLEDFONT_H


/* 使用默认字号 ---------------------------------------------------*/
#define DEFAULT_FONT 16
/* 使用默认字号 ---------------------------------------------------*/

/* 选择字体 -------------------------------------------------------*/
#define SONG
//#define TERMINAL
/* 选择字体 -------------------------------------------------------*/


extern const unsigned char asc2_set_1005[][10];
extern const unsigned char asc2_set_1206[][12];
extern const unsigned char asc2_set_1407[][14];
extern const unsigned char asc2_set_1608[][16];
extern const unsigned char asc2_set_2010[][30];
extern const unsigned char asc2_set_2412[][36];
extern unsigned char bmp1[][8];
#endif

