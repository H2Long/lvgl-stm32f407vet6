#include "touch.h"
#include "lcd.h"
#include "stdlib.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////////////
// 触摸驱动 - 使用 CubeMX 初始化的 GPIO
// 支持 XPT2046/ADS7843 等电阻触摸屏芯片
//////////////////////////////////////////////////////////////////////////////////

_m_tp_dev tp_dev = {
    .init = TP_Init,
    .scan = TP_Scan,
    .adjust = TP_Adjust,
    .x = {0},
    .y = {0},
    .sta = 0,
    .xfac = 0.0f,
    .yfac = 0.0f,
    .xoff = 0,
    .yoff = 0,
    .touchtype = 0,
};

//默认为touchtype=0的情况
uint8_t CMD_RDX = 0xD0;
uint8_t CMD_RDY = 0x90;

//SPI写数据
//向触摸IC写入1byte数据
//num:要写入的数据
void TP_Write_Byte(uint8_t num)
{
    uint8_t count = 0;
    for(count = 0; count < 8; count++)
    {
        if(num & 0x80) TOUCH_MOSI_HIGH();
        else TOUCH_MOSI_LOW();
        num <<= 1;
        TOUCH_SCK_LOW();
        for(volatile int i = 0; i < 10; i++);  // 简单延时
        TOUCH_SCK_HIGH();  // 上升沿有效
    }
}

//SPI读数据
//从触摸IC读取adc值
//CMD:指令
//返回值:读取到的数据
uint16_t TP_Read_AD(uint8_t CMD)
{
    uint8_t count = 0;
    uint16_t Num = 0;
    TOUCH_SCK_LOW();       // 先拉低时钟
    TOUCH_MOSI_LOW();       // 拉低数据线
    TOUCH_CS_LOW();         // 片选选中触摸IC
    TP_Write_Byte(CMD);     // 发送指令
    for(volatile int i = 0; i < 60; i++);  // ADS7846转换时间最长为6us
    TOUCH_SCK_LOW();
    for(volatile int i = 0; i < 10; i++);
    TOUCH_SCK_HIGH();       // 给1个时钟，清除BUSY
    for(volatile int i = 0; i < 10; i++);
    TOUCH_SCK_LOW();
    for(count = 0; count < 16; count++)  // 读出16位数据，只有高12位有效
    {
        Num <<= 1;
        TOUCH_SCK_LOW();     // 下降沿有效
        for(volatile int i = 0; i < 10; i++);
        TOUCH_SCK_HIGH();
        if(TOUCH_MISO_READ()) Num++;
    }
    Num >>= 4;       //只有高12位有效.
    TOUCH_CS_HIGH();        // 释放片选
    return Num;
}

//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读取到的数据
#define READ_TIMES 5    //读取次数
#define LOST_VAL 1      //丢弃值
uint16_t TP_Read_XOY(uint8_t xy)
{
    uint16_t i, j;
    uint16_t buf[READ_TIMES];
    uint16_t sum = 0;
    uint16_t temp;
    for(i = 0; i < READ_TIMES; i++) buf[i] = TP_Read_AD(xy);
    for(i = 0; i < READ_TIMES - 1; i++) //排序
    {
        for(j = i + 1; j < READ_TIMES; j++)
        {
            if(buf[i] > buf[j]) //升序排列
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for(i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++) sum += buf[i];
    temp = sum / (READ_TIMES - 2 * LOST_VAL);
    return temp;
}

//读取x,y坐标
//最小值不能小于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功
uint8_t TP_Read_XY(uint16_t *x, uint16_t *y)
{
    uint16_t xtemp, ytemp;
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);
    *x = xtemp;
    *y = ytemp;
    return 1; //读取成功
}

//连续2次读取触摸IC,这两次偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.
//该函数能大大提高准确度！
//x,y:读取到的坐标值
//返回值:0,失败;1,成功
#define ERR_RANGE 50 //误差范围
uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y)
{
    uint16_t x1, y1;
    uint16_t x2, y2;
    uint8_t flag;
    flag = TP_Read_XY(&x1, &y1);
    if(flag == 0) return 0;
    flag = TP_Read_XY(&x2, &y2);
    if(flag == 0) return 0;
    if(((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //前后两次偏差在+-50内
       && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else return 0;
}

//////////////////////////////////////////////////////////////////////////////////
//与LCD相关的函数
//画一个触摸点
//用来校准用的
//x,y:坐标
//color:颜色
void TP_Drow_Touch_Point(uint16_t x, uint16_t y, uint16_t color)
{
    POINT_COLOR = color;
    LCD_DrawLine(x - 12, y, x + 13, y); //横线
    LCD_DrawLine(x, y - 12, x, y + 13); //竖线
    LCD_DrawPoint(x + 1, y + 1);
    LCD_DrawPoint(x - 1, y + 1);
    LCD_DrawPoint(x + 1, y - 1);
    LCD_DrawPoint(x - 1, y - 1);
    LCD_Draw_Circle(x, y, 6); //画中心圈
}

//画一个大点(2*2的点)
//x,y:坐标
//color:颜色
void TP_Draw_Big_Point(uint16_t x, uint16_t y, uint16_t color)
{
    POINT_COLOR = color;
    LCD_DrawPoint(x, y);   //中心点
    LCD_DrawPoint(x + 1, y);
    LCD_DrawPoint(x, y + 1);
    LCD_DrawPoint(x + 1, y + 1);
}

//////////////////////////////////////////////////////////////////////////////////
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准用，特殊场合使用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
uint8_t TP_Scan(uint8_t tp)
{
    if(TOUCH_PEN_READ() == GPIO_PIN_RESET)  // PEN==0 有按键按下
    {
        if(tp) TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]);  // 读取物理坐标
        else if(TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]))  // 读取屏幕坐标
        {
            tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff;  // 将物理坐标转换为屏幕坐标
            tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;
        }
        if((tp_dev.sta & TP_PRES_DOWN) == 0)  // 之前没有被按下
        {
            tp_dev.sta = TP_PRES_DOWN | TP_CATH_PRES;  // 按键按下
            tp_dev.x[4] = tp_dev.x[0];  // 记录第一次按下时的坐标
            tp_dev.y[4] = tp_dev.y[0];
        }
    }
    else
    {
        if(tp_dev.sta & TP_PRES_DOWN)  // 之前是按下状态
        {
            tp_dev.sta &= ~(1 << 7);  // 标记松开
        }
        else  // 之前就未按下
        {
            tp_dev.x[4] = 0;
            tp_dev.y[4] = 0;
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
        }
    }
    return tp_dev.sta & TP_PRES_DOWN;  // 返回当前的触屏状态
}

//////////////////////////////////////////////////////////////////////////
// 触摸校准参数存储 (使用默认参数，无需 EEPROM)
// 如需保存校准参数，请添加 AT24CXX 驱动文件

// 保存校准参数（使用默认值）
void TP_Save_Adjdata(void)
{
    // 默认参数 - 适用于 2.8寸 320x240 电阻屏 (典型ADC值范围 0-4095)
    tp_dev.xfac = 0.087f;      // x 校准系数: width / adc_range
    tp_dev.yfac = 0.065f;      // y 校准系数: height / adc_range
    tp_dev.xoff = 0;           // x 偏移量
    tp_dev.yoff = 0;           // y 偏移量
}

// 得到校准值（使用默认值）
// 返回值：1，成功获取数据；0，获取失败，要重新校准
uint8_t TP_Get_Adjdata(void)
{
    // 使用默认校准参数 - 适用于 2.8寸 320x240 电阻屏
    tp_dev.xfac = 0.087f;      // x 校准系数
    tp_dev.yfac = 0.065f;      // y 校准系数
    tp_dev.xoff = 0;           // x 偏移量
    tp_dev.yoff = 0;           // y 偏移量
    tp_dev.touchtype = 0;
    CMD_RDX = 0xD0;
    CMD_RDY = 0x90;
    return 1;
}

//提示字符串
const char * TP_REMIND_MSG_TBL = "Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

//显示校准信息(触摸点坐标及校正因子)
void TP_Adj_Info_Show(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                       uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t fac)
{
    POINT_COLOR = RED;
    LCD_ShowString(40, 160, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"x1:");
    LCD_ShowString(40 + 80, 160, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"y1:");
    LCD_ShowString(40, 180, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"x2:");
    LCD_ShowString(40 + 80, 180, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"y2:");
    LCD_ShowString(40, 200, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"x3:");
    LCD_ShowString(40 + 80, 200, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"y3:");
    LCD_ShowString(40, 220, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"x4:");
    LCD_ShowString(40 + 80, 220, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"y4:");
    LCD_ShowString(40, 240, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"fac is:");
    LCD_ShowNum(40 + 24, 160, x0, 4, 16);      //显示数值
    LCD_ShowNum(40 + 24 + 80, 160, y0, 4, 16); //显示数值
    LCD_ShowNum(40 + 24, 180, x1, 4, 16);     //显示数值
    LCD_ShowNum(40 + 24 + 80, 180, y1, 4, 16); //显示数值
    LCD_ShowNum(40 + 24, 200, x2, 4, 16);     //显示数值
    LCD_ShowNum(40 + 24 + 80, 200, y2, 4, 16); //显示数值
    LCD_ShowNum(40 + 24, 220, x3, 4, 16);     //显示数值
    LCD_ShowNum(40 + 24 + 80, 220, y3, 4, 16); //显示数值
    LCD_ShowNum(40 + 56, 240, fac, 3, 16);     //显示数值,该值会在95~105范围之内.
}

//触摸屏校准代码
//得到四个校准参数
void TP_Adjust(void)
{
    uint16_t pos_temp[4][2]; //坐标缓存值
    uint8_t  cnt = 0;
    uint16_t d1, d2;
    uint32_t tem1, tem2;
    double fac;
    uint16_t outtime = 0;
    cnt = 0;
    POINT_COLOR = BLUE;
    BACK_COLOR = WHITE;
    LCD_Clear(WHITE); //清屏
    POINT_COLOR = RED; //红色
    LCD_Clear(WHITE); //清屏
    POINT_COLOR = BLACK;
    LCD_ShowString(40, 40, 160, 100, 16, (uint8_t*)TP_REMIND_MSG_TBL); //显示提示信息
    TP_Drow_Touch_Point(20, 20, RED); //画点1
    tp_dev.sta = 0; //消除触摸信号
    tp_dev.xfac = 0; //xfac用来标记是否校准过,所以校准之前必须清0!以免错误
    while(1) //如果连续10秒钟没有按下,则自动退出
    {
        tp_dev.scan(1); //扫描物理坐标
        if((tp_dev.sta & 0xc0) == TP_CATH_PRES) //按键按下了一次(此时松开键了.)
        {
            outtime = 0;
            tp_dev.sta &= ~(1 << 6); //标记按键已经被处理过了.

            pos_temp[cnt][0] = tp_dev.x[0];
            pos_temp[cnt][1] = tp_dev.y[0];
            cnt++;
            switch(cnt)
            {
                case 1:
                    TP_Drow_Touch_Point(20, 20, WHITE);              //清除点1
                    TP_Drow_Touch_Point(makerbase_lcd.width - 20, 20, RED);  //画点2
                    break;
                case 2:
                    TP_Drow_Touch_Point(makerbase_lcd.width - 20, 20, WHITE);   //清除点2
                    TP_Drow_Touch_Point(20, makerbase_lcd.height - 20, RED);   //画点3
                    break;
                case 3:
                    TP_Drow_Touch_Point(20, makerbase_lcd.height - 20, WHITE);         //清除点3
                    TP_Drow_Touch_Point(makerbase_lcd.width - 20, makerbase_lcd.height - 20, RED); //画点4
                    break;
                case 4:    //全四个点已经得到
                    //对边相等检查
                    tem1 = abs(pos_temp[0][0] - pos_temp[1][0]); //x1-x2
                    tem2 = abs(pos_temp[0][1] - pos_temp[1][1]); //y1-y2
                    tem1 *= tem1;
                    tem2 *= tem2;
                    d1 = sqrt(tem1 + tem2); //得到1,2的距离

                    tem1 = abs(pos_temp[2][0] - pos_temp[3][0]); //x3-x4
                    tem2 = abs(pos_temp[2][1] - pos_temp[3][1]); //y3-y4
                    tem1 *= tem1;
                    tem2 *= tem2;
                    d2 = sqrt(tem1 + tem2); //得到3,4的距离
                    fac = (float)d1 / d2;
                    if(fac < 0.95 || fac > 1.05 || d1 == 0 || d2 == 0) //不合格
                    {
                        cnt = 0;
                        TP_Drow_Touch_Point(makerbase_lcd.width - 20, makerbase_lcd.height - 20, WHITE);   //清除点4
                        TP_Drow_Touch_Point(20, 20, RED);                 //画点1
                        TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1], pos_temp[1][0], pos_temp[1][1],
                                         pos_temp[2][0], pos_temp[2][1], pos_temp[3][0], pos_temp[3][1], fac * 100); //显示数据
                        continue;
                    }
                    tem1 = abs(pos_temp[0][0] - pos_temp[2][0]); //x1-x3
                    tem2 = abs(pos_temp[0][1] - pos_temp[2][1]); //y1-y3
                    tem1 *= tem1;
                    tem2 *= tem2;
                    d1 = sqrt(tem1 + tem2); //得到1,3的距离

                    tem1 = abs(pos_temp[1][0] - pos_temp[3][0]); //x2-x4
                    tem2 = abs(pos_temp[1][1] - pos_temp[3][1]); //y2-y4
                    tem1 *= tem1;
                    tem2 *= tem2;
                    d2 = sqrt(tem1 + tem2); //得到2,4的距离
                    fac = (float)d1 / d2;
                    if(fac < 0.95 || fac > 1.05) //不合格
                    {
                        cnt = 0;
                        TP_Drow_Touch_Point(makerbase_lcd.width - 20, makerbase_lcd.height - 20, WHITE);   //清除点4
                        TP_Drow_Touch_Point(20, 20, RED);                 //画点1
                        TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1], pos_temp[1][0], pos_temp[1][1],
                                         pos_temp[2][0], pos_temp[2][1], pos_temp[3][0], pos_temp[3][1], fac * 100); //显示数据
                        continue;
                    } //正确了

                    //对角线相等
                    tem1 = abs(pos_temp[1][0] - pos_temp[2][0]); //x1-x3
                    tem2 = abs(pos_temp[1][1] - pos_temp[2][1]); //y1-y3
                    tem1 *= tem1;
                    tem2 *= tem2;
                    d1 = sqrt(tem1 + tem2); //得到1,4的距离

                    tem1 = abs(pos_temp[0][0] - pos_temp[3][0]); //x2-x4
                    tem2 = abs(pos_temp[0][1] - pos_temp[3][1]); //y2-y4
                    tem1 *= tem1;
                    tem2 *= tem2;
                    d2 = sqrt(tem1 + tem2); //得到2,3的距离
                    fac = (float)d1 / d2;
                    if(fac < 0.95 || fac > 1.05) //不合格
                    {
                        cnt = 0;
                        TP_Drow_Touch_Point(makerbase_lcd.width - 20, makerbase_lcd.height - 20, WHITE);   //清除点4
                        TP_Drow_Touch_Point(20, 20, RED);                 //画点1
                        TP_Adj_Info_Show(pos_temp[0][0], pos_temp[0][1], pos_temp[1][0], pos_temp[1][1],
                                         pos_temp[2][0], pos_temp[2][1], pos_temp[3][0], pos_temp[3][1], fac * 100); //显示数据
                        continue;
                    } //正确了
                    //计算校准参数
                    tp_dev.xfac = (float)(makerbase_lcd.width - 40) / (pos_temp[1][0] - pos_temp[0][0]); //得到xfac
                    tp_dev.xoff = (makerbase_lcd.width - tp_dev.xfac * (pos_temp[1][0] + pos_temp[0][0])) / 2; //得到xoff

                    tp_dev.yfac = (float)(makerbase_lcd.height - 40) / (pos_temp[2][1] - pos_temp[0][1]); //得到yfac
                    tp_dev.yoff = (makerbase_lcd.height - tp_dev.yfac * (pos_temp[2][1] + pos_temp[0][1])) / 2; //得到yoff
                    if(abs(tp_dev.xfac) > 2 || abs(tp_dev.yfac) > 2) //触屏和预设的相反了.
                    {
                        cnt = 0;
                        TP_Drow_Touch_Point(makerbase_lcd.width - 20, makerbase_lcd.height - 20, WHITE);   //清除点4
                        TP_Drow_Touch_Point(20, 20, RED);                 //画点1
                        LCD_ShowString(40, 26, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"TP Need readjust!");
                        tp_dev.touchtype = !tp_dev.touchtype; //修改触屏类型.
                        if(tp_dev.touchtype) //X,Y方向与屏幕相反
                        {
                            CMD_RDX = 0X90;
                            CMD_RDY = 0XD0;
                        }
                        else             //X,Y方向与屏幕相同
                        {
                            CMD_RDX = 0XD0;
                            CMD_RDY = 0X90;
                        }
                        continue;
                    }
                    POINT_COLOR = BLUE;
                    LCD_Clear(WHITE); //清屏
                    LCD_ShowString(35, 110, makerbase_lcd.width, makerbase_lcd.height, 16, (uint8_t*)"Touch Screen Adjust OK!"); //校准完成
                    HAL_Delay(1000);
                    TP_Save_Adjdata();
                    LCD_Clear(WHITE); //清屏
                    return; //校准完成
            }
        }
        HAL_Delay(10);
        outtime++;
        if(outtime > 1000)
        {
            TP_Get_Adjdata();
            break;
        }
    }
}

//触摸屏初始化
//返回值:0,没有进行校准
//       1,进行过校准
// 注意：GPIO初始化已在 MX_GPIO_Init() 中完成，此处只做触摸相关初始化
uint8_t TP_Init(void)
{
    // 使用默认校准参数（无需 EEPROM 存储）
    TP_Get_Adjdata();

    // 首次读取初始化
    TP_Read_XY(&tp_dev.x[0], &tp_dev.y[0]);

    return 1;
}
