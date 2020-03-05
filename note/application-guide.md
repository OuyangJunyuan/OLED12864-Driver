# OLED12864Lib使用教程

参考资料：

* [ssd1306——oled驱动芯片手册]()

## 文件说明

* oled12864.c/.h主要文件
* oledfont.c/.h字体字号文件

### 文件解析

#### 1.oled12864.h

![1566203023226](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566203023226.png)

用户配置中：

1. 重定向：![1566203117091](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566203117091.png)->![1566203135926](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566203135926.png)->![1566203147814](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566203147814.png)

   重定向流可以使得printf或fputc等Cstdlib的函数的数据流重定向到屏幕输出。

   这样就可以使用printf来输出信息到屏幕上，方便调试和格式化输出。

2.  通信接口选择：有硬件SPI、硬件I2C、软件I2C的选择。

   1. 硬件SPI刷新率最高，但占用引脚多,通信速率可以达到外设时钟APXClock的1半,如F4的全速有42M bit/s
   2. 硬件I2C刷新率低，但占用引脚少，只要2根。速率只有100K~2M
   3. 软件I2C刷新率低，占用引脚低，且不受是否有SPI和I2C外设限制，使用普通GPIO口就可以调用

3. 接口资源配置

   1. 配置使用的i2c句柄或spi句柄
   2. 配置管脚



#### 2.oledfont.h

![1566203756188](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566203756188.png)

见图，如果打开默认字号，则统统默认使用默认字号

### 函数功能说明

使用的坐标系是像素坐标系(左上角0点，→x ↓y)

1. 各种通信接口最终同一使用 OLED_WriteRead_Byte(uint8_t data,uint8_t CorD) 发送指令到OLED控制器ssd1306上

2. holed.Draw.

   1. Point(x坐标，y坐标，擦写标记)   擦写标记：擦POINT_MOVE 写：POINT_DRAW![1566204159646](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566204159646.png)
   2. Line(起点x，起点y，终点x，终点y)
   3. Char(字符，字号，字符左上角坐标x，字符左上角坐标y)
   4. String("字符串"，字号，字符左上角坐标x，字符左上角坐标y) 通过判断'\0'计数故不用输入字数
   5. Buffer(字符串数组头指针，字符串数组元素个数，字号，字符左上角坐标x，字符左上角坐标y) 补'\0'调用String
   6. BMP(bmp数据数组指针)

3. holed.Set.

   1. Brightness(设置亮度) 输入值在0-15 即 0x00——0x0F之间
   2. IsForce_FullScreen(0/1)输入1强制全屏点亮(无视GDDRAM内容)
   3. Horizontal_Flip(0/1)输入1水平翻转
   4. Vertical_Flip(0/1)输入1垂直翻转
   5. Inverse_Display(0/1)反显与否 即写的是黑色还是蓝色
   6. Display_OFF()
   7. Display_ON()
   8. **<u>Refresh() 最重要的一个！！！以上所有作画需要使用本函数将GDDRAM数组内容写入oled控制器ssd1306的GDDRAM中。GDDRAM(Graphic Display Data random-access memory)</u>**

   

### 再开发说明：

#### 改变字体

![1566205140345](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205140345.png)![1566205153755](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205153755.png)

设置字体大小16x16就是库中的1608表示16高08宽，英文是中文的一半宽。

![1566205224116](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205224116.png)![1566205273319](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205273319.png)按图配置

下面这串字符是可显示字符(注意开头！前还有1个空格也算，共95个)

 !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~

复制到![1566205397780](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205397780.png)

![1566205431043](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205431043.png)选择喜欢的字体。点击![1566205407618](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205407618.png)

将生成的![1566205463119](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205463119.png)

复制黏贴到oledfont.c 中

再复制之前先到oledfont.h中![1566205713729](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205713729.png)

增加刚才自定义字体的宏名

然后

![1566205648710](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566205648710.png)

将xxx改成修改对应字体，然后将数据复制到对应字号的字体的数组中。

#### 字号自定义

已经有12、14、16、20、24的字号，更大的字体需要很大的数组储存(如1个64*64的字符就需要64 / 8 x 32 = 256字节存储空间 ）而10号字太小了糊的看不见。

如果需要添加，在以上数组中多加1个就好了。对应的在

![1566206172961](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206172961.png)

中添加新的。

#### 图片播放

图片需要时二值化的。注：图像处理可以使用![1566207514704](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566207514704.png)挺好用的。

取模软件使用图片模式。

![1566206288697](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206288697.png)

然后http://pic.55.la/网站中将图片格式转换成BMP格式的，然后生成模。将数组存下来调用

holed.Draw.BMP() 然后刷新屏幕即可。

## 实例

1. ![1566202784110](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566202784110.png)

   记得勾选生成外设初始化文件。

2. ![1566206549170](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206549170.png)->![1566206559214](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206559214.png)->![1566206576659](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206576659.png)
3. 包含头文件![1566206593039](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206593039.png)
4. 初始化![1566206602257](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206602257.png)

5. ![1566206627703](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566206627703.png)重定向到屏幕并刷新

6. 效果(默认字号)。可见只需要添加头文件，初始化，就可以打印到屏幕上方便调试。

   ![1566207153179](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566207153179.png)

7. ![1566207132982](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566207173255.png)

   ![1566207179981](C:\Users\70654\AppData\Roaming\Typora\typora-user-images\1566207179981.png)









