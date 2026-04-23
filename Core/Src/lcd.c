#include "lcd.h"
#include "font.h"
//#include "usart.h"
//#include "delay.h"
#include "fsmc.h"

lcd_dev makerbase_lcd;
uint16_t POINT_COLOR = 0x0000;
uint16_t BACK_COLOR  = 0xFFFF;

extern void delay_us(uint32_t nus);

// 底层寄存器操作
void LCD_WR_REG(volatile uint16_t regval)
{
    (void)regval; // 防止未使用警告
    LCD->LCD_REG = regval;
}

void LCD_WR_DATA(volatile uint16_t data)
{
    (void)data;
    LCD->LCD_RAM = data;
}

uint16_t LCD_RD_DATA(void)
{
    volatile uint16_t ram;
    ram = LCD->LCD_RAM;
    return ram;
}

void LCD_WriteReg(volatile uint16_t LCD_Reg, volatile uint16_t LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;
    LCD->LCD_RAM = LCD_RegValue;
}

uint16_t LCD_ReadReg(volatile uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);
    delay_us(5);
    return LCD_RD_DATA();
}

void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = makerbase_lcd.wramcmd;
}

void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;
}

uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t r, g, b, rgb;
    b = (c >> 0)  & 0x1f;
    g = (c >> 5)  & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return rgb;
}

static void opt_delay(uint8_t i)
{
    while(i--);
}

uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    volatile uint16_t r = 0, g = 0, b = 0;
    if(x >= makerbase_lcd.width || y >= makerbase_lcd.height) return 0;
    LCD_SetCursor(x, y);
    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x6804 || makerbase_lcd.id == 0x5310)
        LCD_WR_REG(0x2E);
    else if(makerbase_lcd.id == 0x5510)
        LCD_WR_REG(0x2E00);
    else
        LCD_WR_REG(R34);
    if(makerbase_lcd.id == 0x9320) opt_delay(2);
    LCD_RD_DATA();
    opt_delay(2);
    r = LCD_RD_DATA();
    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x5310 || makerbase_lcd.id == 0x5510)
    {
        opt_delay(2);
        b = LCD_RD_DATA();
        g = r & 0xFF;
        g <<= 8;
    }
    if(makerbase_lcd.id == 0x9325 || makerbase_lcd.id == 0x4535 || makerbase_lcd.id == 0x4531 ||
       makerbase_lcd.id == 0xB505 || makerbase_lcd.id == 0xC505)
        return r;
    else if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x5310 || makerbase_lcd.id == 0x5510)
        return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11));
    else
        return LCD_BGR2RGB(r);
}

void LCD_DisplayOn(void)
{
    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x6804 || makerbase_lcd.id == 0x5310)
        LCD_WR_REG(0x29);
    else if(makerbase_lcd.id == 0x5510)
        LCD_WR_REG(0x2900);
    else
        LCD_WriteReg(R7, 0x0173);
}

void LCD_DisplayOff(void)
{
    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x6804 || makerbase_lcd.id == 0x5310)
        LCD_WR_REG(0x28);
    else if(makerbase_lcd.id == 0x5510)
        LCD_WR_REG(0x2800);
    else
        LCD_WriteReg(R7, 0x0);
}

void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x8552 || makerbase_lcd.id == 0x5310)
    {
        LCD_WR_REG(makerbase_lcd.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0xFF);
    }
    else if(makerbase_lcd.id == 0x6804)
    {
        if(makerbase_lcd.dir == 1) Xpos = makerbase_lcd.width - 1 - Xpos;
        LCD_WR_REG(makerbase_lcd.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0xFF);
    }
    else if(makerbase_lcd.id == 0x5510)
    {
        LCD_WR_REG(makerbase_lcd.setxcmd); LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(makerbase_lcd.setxcmd + 1); LCD_WR_DATA(Xpos & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd); LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(makerbase_lcd.setycmd + 1); LCD_WR_DATA(Ypos & 0xFF);
    }
    else
    {
        if(makerbase_lcd.dir == 1) Xpos = makerbase_lcd.width - 1 - Xpos;
        LCD_WriteReg(makerbase_lcd.setxcmd, Xpos);
        LCD_WriteReg(makerbase_lcd.setycmd, Ypos);
    }
}

void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_SetCursor(x, y);
    LCD_WriteRAM_Prepare();
    LCD->LCD_RAM = POINT_COLOR;
}

void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x8552 || makerbase_lcd.id == 0x5310)
    {
        LCD_WR_REG(makerbase_lcd.setxcmd);
        LCD_WR_DATA(x >> 8); LCD_WR_DATA(x & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd);
        LCD_WR_DATA(y >> 8); LCD_WR_DATA(y & 0xFF);
    }
    else if(makerbase_lcd.id == 0x5510)
    {
        LCD_WR_REG(makerbase_lcd.setxcmd); LCD_WR_DATA(x >> 8);
        LCD_WR_REG(makerbase_lcd.setxcmd + 1); LCD_WR_DATA(x & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd); LCD_WR_DATA(y >> 8);
        LCD_WR_REG(makerbase_lcd.setycmd + 1); LCD_WR_DATA(y & 0xFF);
    }
    else if(makerbase_lcd.id == 0x6804)
    {
        if(makerbase_lcd.dir == 1) x = makerbase_lcd.width - 1 - x;
        LCD_WR_REG(makerbase_lcd.setxcmd);
        LCD_WR_DATA(x >> 8); LCD_WR_DATA(x & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd);
        LCD_WR_DATA(y >> 8); LCD_WR_DATA(y & 0xFF);
    }
    else
    {
        if(makerbase_lcd.dir == 1) x = makerbase_lcd.width - 1 - x;
        LCD_WriteReg(makerbase_lcd.setxcmd, x);
        LCD_WriteReg(makerbase_lcd.setycmd, y);
    }
    LCD->LCD_REG = makerbase_lcd.wramcmd;
    LCD->LCD_RAM = color;
}

// 实现 LCD_Scan_Dir 函数
void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t xsize = makerbase_lcd.width;
    uint16_t ysize = makerbase_lcd.height;

    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x6804 || makerbase_lcd.id == 0x8552 ||
       makerbase_lcd.id == 0x5310 || makerbase_lcd.id == 0x5510)
    {
        switch(dir)
        {
            case L2R_U2D: regval |= (0<<7)|(0<<6)|(0<<5); break;
            case L2R_D2U: regval |= (1<<7)|(0<<6)|(0<<5); break;
            case R2L_U2D: regval |= (0<<7)|(1<<6)|(0<<5); break;
            case R2L_D2U: regval |= (1<<7)|(1<<6)|(0<<5); break;
            case U2D_L2R: regval |= (0<<7)|(0<<6)|(1<<5); break;
            case U2D_R2L: regval |= (0<<7)|(1<<6)|(1<<5); break;
            case D2U_L2R: regval |= (1<<7)|(0<<6)|(1<<5); break;
            case D2U_R2L: regval |= (1<<7)|(1<<6)|(1<<5); break;
        }
        // 写入 MADCTL 寄存器 (0x36)
        if(makerbase_lcd.id == 0x5510)
            LCD_WR_REG(0x3600);
        else
            LCD_WR_REG(0x36);
        LCD_WR_DATA(regval);

        // 设置列地址范围
        LCD_WR_REG(makerbase_lcd.setxcmd);
        LCD_WR_DATA(0); LCD_WR_DATA(0);
        LCD_WR_DATA((xsize-1)>>8); LCD_WR_DATA((xsize-1)&0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd);
        LCD_WR_DATA(0); LCD_WR_DATA(0);
        LCD_WR_DATA((ysize-1)>>8); LCD_WR_DATA((ysize-1)&0xFF);
    }
    else
    {
        switch(dir)
        {
            case L2R_U2D: regval |= (1<<5)|(1<<4)|(0<<3); break;
            case L2R_D2U: regval |= (0<<5)|(1<<4)|(0<<3); break;
            case R2L_U2D: regval |= (1<<5)|(0<<4)|(0<<3); break;
            case R2L_D2U: regval |= (0<<5)|(0<<4)|(0<<3); break;
            case U2D_L2R: regval |= (1<<5)|(1<<4)|(1<<3); break;
            case U2D_R2L: regval |= (1<<5)|(0<<4)|(1<<3); break;
            case D2U_L2R: regval |= (0<<5)|(1<<4)|(1<<3); break;
            case D2U_R2L: regval |= (0<<5)|(0<<4)|(1<<3); break;
        }
        if(makerbase_lcd.id == 0x8989)
        {
            dirreg = 0x11;
            regval |= 0x6040;
        }
        else
        {
            dirreg = 0x03;
            regval |= 1<<12;
        }
        LCD_WriteReg(dirreg, regval);
    }
}

void LCD_Display_Dir(uint8_t dir)
{
    if(dir == 0) // 竖屏
    {
        makerbase_lcd.dir = 0;
        makerbase_lcd.width = 240;
        makerbase_lcd.height = 320;
        makerbase_lcd.wramcmd = 0x2C;
        makerbase_lcd.setxcmd = 0x2A;
        makerbase_lcd.setycmd = 0x2B;
    }
    else         // 横屏
    {
        makerbase_lcd.dir = 1;
        makerbase_lcd.width = 320;
        makerbase_lcd.height = 240;
        makerbase_lcd.wramcmd = 0x2C;
        makerbase_lcd.setxcmd = 0x2A;
        makerbase_lcd.setycmd = 0x2B;
    }
    LCD_Scan_Dir(DFT_SCAN_DIR);
}

void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint16_t ex = sx + width - 1;
    uint16_t ey = sy + height - 1;
    if(makerbase_lcd.id == 0x9341 || makerbase_lcd.id == 0x5310 || makerbase_lcd.id == 0x6804)
    {
        LCD_WR_REG(makerbase_lcd.setxcmd);
        LCD_WR_DATA(sx >> 8); LCD_WR_DATA(sx & 0xFF);
        LCD_WR_DATA(ex >> 8); LCD_WR_DATA(ex & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd);
        LCD_WR_DATA(sy >> 8); LCD_WR_DATA(sy & 0xFF);
        LCD_WR_DATA(ey >> 8); LCD_WR_DATA(ey & 0xFF);
    }
    else if(makerbase_lcd.id == 0x5510)
    {
        LCD_WR_REG(makerbase_lcd.setxcmd); LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(makerbase_lcd.setxcmd + 1); LCD_WR_DATA(sx & 0xFF);
        LCD_WR_REG(makerbase_lcd.setxcmd + 2); LCD_WR_DATA(ex >> 8);
        LCD_WR_REG(makerbase_lcd.setxcmd + 3); LCD_WR_DATA(ex & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd); LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(makerbase_lcd.setycmd + 1); LCD_WR_DATA(sy & 0xFF);
        LCD_WR_REG(makerbase_lcd.setycmd + 2); LCD_WR_DATA(ey >> 8);
        LCD_WR_REG(makerbase_lcd.setycmd + 3); LCD_WR_DATA(ey & 0xFF);
    }
    else
    {
        if(makerbase_lcd.dir == 1)
        {
            uint16_t hsaval = sy;
            uint16_t heaval = ey;
            uint16_t vsaval = makerbase_lcd.width - ex - 1;
            uint16_t veaval = makerbase_lcd.width - sx - 1;
            LCD_WriteReg(0x50, hsaval);
            LCD_WriteReg(0x51, heaval);
            LCD_WriteReg(0x52, vsaval);
            LCD_WriteReg(0x53, veaval);
        }
        else
        {
            LCD_WriteReg(0x50, sx);
            LCD_WriteReg(0x51, ex);
            LCD_WriteReg(0x52, sy);
            LCD_WriteReg(0x53, ey);
        }
        LCD_SetCursor(sx, sy);
    }
}

void LCD_Clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = makerbase_lcd.width;
    totalpoint *= makerbase_lcd.height;
    LCD_SetCursor(0, 0);
    LCD_WriteRAM_Prepare();
    for(index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t xlen = ex - sx + 1;
    for(i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);
        LCD_WriteRAM_Prepare();
        for(j = 0; j < xlen; j++)
            LCD->LCD_RAM = color;
    }
}

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0) incx = 1;
    else if(delta_x == 0) incx = 0;
    else { incx = -1; delta_x = -delta_x; }
    if(delta_y > 0) incy = 1;
    else if(delta_y == 0) incy = 0;
    else { incy = -1; delta_y = -delta_y; }
    distance = (delta_x > delta_y) ? delta_x : delta_y;
    for(t = 0; t <= distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol);
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0; b = r;
    di = 3 - (r << 1);
    while(a <= b)
    {
        LCD_DrawPoint(x0+a, y0-b);
        LCD_DrawPoint(x0+b, y0-a);
        LCD_DrawPoint(x0+b, y0+a);
        LCD_DrawPoint(x0+a, y0+b);
        LCD_DrawPoint(x0-a, y0+b);
        LCD_DrawPoint(x0-b, y0+a);
        LCD_DrawPoint(x0-a, y0-b);
        LCD_DrawPoint(x0-b, y0-a);
        a++;
        if(di < 0) di += 4*a + 6;
        else
        {
            di += 10 + 4*(a - b);
            b--;
        }
    }
}

void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    num = num - ' ';
    for(t = 0; t < csize; t++)
    {
        if(size == 12) temp = asc2_1206[num][t];
        else if(size == 16) temp = asc2_1608[num][t];
        else if(size == 24) temp = asc2_2412[num][t];
        else return;
        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80) LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if(mode == 0) LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if(y >= makerbase_lcd.height) return;
            if((y - y0) == size)
            {
                y = y0;
                x++;
                if(x >= makerbase_lcd.width) return;
                break;
            }
        }
    }
}

static uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) result *= m;
    return result;
}

void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}

void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode & 0x80)
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0x01);
                else
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0x01);
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0x01);
    }
}

void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while((*p <= '~') && (*p >= ' '))
    {
        if(x >= width) { x = x0; y += size; }
        if(y >= height) break;
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}

// LCD 初始化函数（ST7789 示例）
void LCD_Init(void)
{
    // 背光引脚初始化
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); // 先关闭背光

    // 读取 LCD ID
    LCD_WR_REG(0x04);
    makerbase_lcd.id = LCD_RD_DATA();
    makerbase_lcd.id = LCD_RD_DATA();
    makerbase_lcd.id = LCD_RD_DATA();
    makerbase_lcd.id <<= 8;
    makerbase_lcd.id |= LCD_RD_DATA();

    // 若需要打印，确保 printf 已重定向，否则注释掉
    // printf("LCD ID: 0x%04X\r\n", makerbase_lcd.id);

    // 强制指定 ID（ST7789）
    makerbase_lcd.id = 0x8552;

    // ST7789 初始化序列
    LCD_WR_REG(0x11);
    HAL_Delay(120);
    LCD_WR_REG(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x05);
    LCD_WR_REG(0xB2);
    LCD_WR_DATA(0x0C); LCD_WR_DATA(0x0C); LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33); LCD_WR_DATA(0x33);
    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0x35);
    LCD_WR_REG(0xBB);
    LCD_WR_DATA(0x28);
    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x2C);
    LCD_WR_REG(0xC2);
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xC3);
    LCD_WR_DATA(0x0B);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x20);
    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0xD0);
    LCD_WR_DATA(0xA4); LCD_WR_DATA(0xA1);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0xD0); LCD_WR_DATA(0x01); LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0F); LCD_WR_DATA(0x11); LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x36); LCD_WR_DATA(0x55); LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x3A); LCD_WR_DATA(0x0B); LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x11); LCD_WR_DATA(0x20);
    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0xD0); LCD_WR_DATA(0x02); LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x0A); LCD_WR_DATA(0x0B); LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x34); LCD_WR_DATA(0x43); LCD_WR_DATA(0x4A);
    LCD_WR_DATA(0x2B); LCD_WR_DATA(0x1B); LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x22); LCD_WR_DATA(0x1F);
    LCD_WR_REG(0x29);

    LCD_Display_Dir(0); // 竖屏
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); // 点亮背光
    LCD_Clear(WHITE);
}