#ifndef __TOUCH_H__
#define __TOUCH_H__
#include "stm32f4xx_hal.h"
#include "gpio.h"              // 引入 CubeMX 生成的 GPIO 定义
#include "lcd.h"

//////////////////////////////////////////////////////////////////////////////////
// 触摸屏驱动 - 使用 CubeMX 初始化的 GPIO
// 支持 XPT2046/ADS7843 等电阻触摸屏芯片
//////////////////////////////////////////////////////////////////////////////////

// ---------- 触摸芯片引脚操作宏（使用 CubeMX 宏）----------
#define TOUCH_CS_LOW()    HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_RESET)
#define TOUCH_CS_HIGH()   HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_SET)
#define TOUCH_SCK_LOW()   HAL_GPIO_WritePin(T_SCK_GPIO_Port, T_SCK_Pin, GPIO_PIN_RESET)
#define TOUCH_SCK_HIGH()  HAL_GPIO_WritePin(T_SCK_GPIO_Port, T_SCK_Pin, GPIO_PIN_SET)
#define TOUCH_MOSI_LOW()  HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_RESET)
#define TOUCH_MOSI_HIGH() HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_SET)
#define TOUCH_MISO_READ() HAL_GPIO_ReadPin(T_MISO_GPIO_Port, T_MISO_Pin)
#define TOUCH_PEN_READ()  HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin)

// ---------- 触摸状态定义 ----------
#define TP_PRES_DOWN 0x80   // 触摸屏被按下
#define TP_CATH_PRES 0x40   // 有按键按下了

// ---------- 触摸屏控制器 ----------
#define MAX_TOUCH_NUM 5     // 最大触摸点数

typedef struct
{
    uint8_t (*init)(void);           // 初始化触摸屏控制器
    uint8_t (*scan)(uint8_t);         // 扫描触摸屏. 0, 屏幕扫描; 1, 物理坐标;
    void (*adjust)(void);            // 触摸屏校准
    uint16_t x[MAX_TOUCH_NUM];       // 当前坐标
    uint16_t y[MAX_TOUCH_NUM];       // 电容触摸屏最多5点, 电阻屏则只用 x[0],y[0]
                                      // x[4],y[4] 存储第一次按下时的坐标
    uint8_t  sta;                     // 笔的状态
                                      // b7: 按下 1/松开 0;
                                      // b6: 0, 没有按键按下; 1, 有按键按下.
                                      // b5: 保留
                                      // b4~b0: 电容触摸屏按下的点数
    float xfac;                      // x 校准系数
    float yfac;                      // y 校准系数
    int16_t xoff;                     // x 偏移量
    int16_t yoff;                     // y 偏移量
    uint8_t touchtype;                // 触摸类型
} _m_tp_dev;

extern _m_tp_dev tp_dev;              // 触摸屏控制器

// ---------- 函数声明 ----------
void TP_Write_Byte(uint8_t num);                       // 向触摸芯片写入一个字节
uint16_t TP_Read_AD(uint8_t CMD);                      // 读取 AD 转换值
uint16_t TP_Read_XOY(uint8_t xy);                      // 带滤波的单轴坐标读取
uint8_t TP_Read_XY(uint16_t *x, uint16_t *y);          // 双轴坐标读取
uint8_t TP_Read_XY2(uint16_t *x, uint16_t *y);         // 加强滤波的双轴坐标读取
void TP_Drow_Touch_Point(uint16_t x, uint16_t y, uint16_t color);  // 画触摸校准点
void TP_Draw_Big_Point(uint16_t x, uint16_t y, uint16_t color);     // 画一个大点
void TP_Save_Adjdata(void);                           // 保存校准参数
uint8_t TP_Get_Adjdata(void);                          // 读取校准参数
void TP_Adjust(void);                                  // 触摸屏校准
void TP_Adj_Info_Show(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                       uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t fac);  // 显示校准信息
uint8_t TP_Scan(uint8_t tp);                           // 扫描
uint8_t TP_Init(void);                                 // 初始化

#endif
