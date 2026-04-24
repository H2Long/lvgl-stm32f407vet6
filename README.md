# STM32F407 + LVGL 例程说明

本项目是基于 STM32F407 微控制器的 LVGL 图形库移植例程，使用 CMake 构建系统。

## 硬件配置

- **微控制器**: STM32F407VGT6
- **LCD 屏幕**: ST7789 (240x320 竖屏)
- **接口**: FSMC (Flexible Static Memory Controller)
- **触控**: XPT2046/ADS7843 电阻触摸屏 (SPI接口)

## 目录结构

```
lvgl-sest2.0/
├── Core/
│   └── Src/
│       ├── main.c          # 主程序入口
│       ├── lcd.c           # LCD 驱动
│       ├── lcd.h           # LCD 驱动头文件
│       ├── touch.c         # 触摸屏驱动
│       ├── touch.h         # 触摸屏驱动头文件
│       ├── fsmc.c          # FSMC 初始化
│       └── ...
├── Middlewares/
│   └── LVGL/
│       ├── lv_conf.h       # LVGL 配置文件
│       ├── src/             # LVGL 核心源码
│       ├── examples/        # 移植示例
│       │   └── porting/
│       │       ├── lv_port_disp.c  # 显示驱动移植
│       │       └── lv_port_indev.c # 输入设备移植(触摸)
│       └── ...
├── cmake/
│   └── stm32cubemx/
│       └── CMakeLists.txt  # 构建配置文件
└── build/                  # 构建输出目录
```

---

## CMakeLists.txt 配置说明

### 1. 新增源文件集合

在 `cmake/stm32cubemx/CMakeLists.txt` 中添加以下内容：

```cmake
# ========== LCD 和 Touch 驱动源文件 ==========
set(LCD_Touch_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/lcd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/touch.c
)

# LCD 和 Touch 头文件路径
set(LCD_Touch_Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src
)

# ========== LVGL 头文件路径 ==========
set(LVGL_Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/examples/porting
)

# ========== LVGL 源文件 ==========
set(LVGL_Src
    # core/ - LVGL 核心模块 (必须包含 lv_indev_scroll.c)
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_event.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_group.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_indev.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/core/lv_indev_scroll.c
    # ... 其他 core 文件 ...

    # font/ - 必须包含 lv_font_montserrat_14.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/font/lv_font_montserrat_14.c
    # ... 其他字体文件 ...

    # extra/ - 必须包含日历头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/calendar/lv_calendar_header_arrow.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/src/extra/widgets/calendar/lv_calendar_header_dropdown.c
    # ... 其他 extra 文件 ...

    # porting/ - 移植文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/examples/porting/lv_port_disp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Middlewares/LVGL/examples/porting/lv_port_indev.c
)
```

### 2. 更新头文件包含

修改 `target_include_directories` 添加 LVGL 和 LCD 路径：

```cmake
target_include_directories(stm32cubemx INTERFACE
    ${MX_Include_Dirs}
    ${LCD_Touch_Inc}
    ${LVGL_Inc}
)
```

### 3. 添加源文件到项目

修改 `target_sources` 添加 LCD/Touch 和 LVGL 源文件：

```cmake
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${MX_Application_Src}
    ${LCD_Touch_Src}
    ${LVGL_Src}
)
```

### 完整 CMakeLists.txt 配置

关键配置段落：

```cmake
# CMakeLists.txt 关键部分

# 1. 源文件集合定义 (第38-47行)
set(LCD_Touch_Src ...)
set(LCD_Touch_Inc ...)
set(LVGL_Src ...)
set(LVGL_Inc ...)

# 2. 头文件路径 (第246-248行)
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE
    ${MX_Include_Dirs}
    ${LCD_Touch_Inc}
    ${LVGL_Inc}
)

# 3. 源文件添加 (第262行)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${MX_Application_Src}
    ${LCD_Touch_Src}
    ${LVGL_Src}
)
```

---

## 注册 LVGL 触摸屏幕的方法

### 1. 触摸初始化流程

在 `main.c` 中按以下顺序初始化：

```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    // 外设初始化
    MX_GPIO_Init();
    MX_FSMC_Init();
    MX_TIM6_Init();

    // ========== LVGL + 触摸初始化 ==========
    LCD_Init();                    // 1. 初始化 LCD
    tp_dev.init();                 // 2. 初始化触摸屏 (可选，lv_port_indev_init 会调用)

    lv_init();                     // 3. LVGL 核心初始化
    lv_port_disp_init();           // 4. 注册显示驱动
    lv_port_indev_init();          // 5. 注册触摸输入设备 ⭐

    HAL_TIM_Base_Start_IT(&htim6); // 6. 启动心跳定时器

    // UI 创建
    lv_obj_t *btn = lv_btn_create(lv_scr_act());

    while (1) {
        lv_timer_handler();
        HAL_Delay(5);
    }
}
```

### 2. lv_port_indev.c 触摸驱动实现

`lv_port_indev.c` 是 LVGL 输入设备的移植文件，负责将硬件触摸与 LVGL 连接：

```c
/**********************
 *  全局变量
 **********************/
static lv_indev_t * indev_touchpad;  // LVGL 输入设备句柄
static lv_coord_t last_x = 0;        // 上次 X 坐标
static lv_coord_t last_y = 0;        // 上次 Y 坐标

/**********************
 *  初始化函数
 **********************/
void lv_port_indev_init(void)
{
    // 1. 创建输入设备驱动
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);

    // 2. 设置为触摸类型
    indev_drv.type = LV_INDEV_TYPE_POINTER;

    // 3. 设置读取回调
    indev_drv.read_cb = touchpad_read;

    // 4. 注册到 LVGL
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}

/**********************
 *  触摸读取回调
 **********************/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;    // 按下状态
    } else {
        data->state = LV_INDEV_STATE_REL;   // 释放状态
    }
    data->point.x = last_x;   // X 坐标
    data->point.y = last_y;   // Y 坐标
}

/**********************
 *  硬件访问函数
 **********************/
static bool touchpad_is_pressed(void)
{
    return (tp_dev.scan(0) != 0);  // 调用 touch.c 的扫描函数
}

static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    (*x) = tp_dev.x[0];   // 获取 X 坐标
    (*y) = tp_dev.y[0];   // 获取 Y 坐标
}
```

### 3. 触摸与 LVGL 按钮交互

LVGL 会自动将触摸事件分发给焦点对象。只需创建按钮并添加事件即可：

```c
// 创建按钮
lv_obj_t *btn = lv_btn_create(lv_scr_act());
lv_obj_set_pos(btn, 80, 80);
lv_obj_set_size(btn, 160, 60);

// 添加事件回调
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);

// 按钮事件回调
static void btn_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        // 按钮被点击
    }
}
```

---

## touch.c 触摸驱动函数介绍

### 数据结构

```c
// 触摸屏控制器结构体
typedef struct {
    uint8_t (*init)(void);           // 初始化函数指针
    uint8_t (*scan)(uint8_t);        // 扫描函数指针 (0=屏幕坐标, 1=物理坐标)
    void (*adjust)(void);             // 校准函数指针
    uint16_t x[MAX_TOUCH_NUM];        // X 坐标数组
    uint16_t y[MAX_TOUCH_NUM];        // Y 坐标数组
    uint8_t  sta;                      // 状态 (b7=按下, b6=有按键)
    float xfac, yfac;                 // 校准系数
    int16_t xoff, yoff;               // 校准偏移量
    uint8_t touchtype;                // 触摸类型
} _m_tp_dev;

extern _m_tp_dev tp_dev;  // 全局触摸屏设备
```

### 核心函数

| 函数名 | 功能 | 参数 | 返回值 |
|--------|------|------|--------|
| `TP_Init()` | 初始化触摸屏 | 无 | 0=未校准, 1=已校准 |
| `TP_Scan(tp)` | 扫描触摸 | 0=屏幕坐标, 1=物理坐标 | 0=无触摸, 1=有触摸 |
| `TP_Adjust()` | 执行触摸屏校准 | 无 | 无 |
| `TP_Save_Adjdata()` | 保存校准参数 | 无 | 无 |
| `TP_Get_Adjdata()` | 获取校准参数 | 无 | 0=失败, 1=成功 |

### SPI 通信函数

| 函数名 | 功能 | 参数 | 返回值 |
|--------|------|------|--------|
| `TP_Write_Byte(num)` | SPI 写入1字节 | num: 要写入的数据 | 无 |
| `TP_Read_AD(CMD)` | 读取 ADC 值 | CMD: 指令 (0xD0/0x90) | 12位 ADC 值 |
| `TP_Read_XOY(xy)` | 带滤波读取单轴 | xy: CMD_RDX 或 CMD_RDY | 滤波后的值 |
| `TP_Read_XY(x,y)` | 读取双轴坐标 | 指针指向 x, y | 0=失败, 1=成功 |
| `TP_Read_XY2(x,y)` | 加强滤波读取 | 指针指向 x, y | 0=失败, 1=成功 |

### 辅助函数

| 函数名 | 功能 |
|--------|------|
| `TP_Drow_Touch_Point(x,y,color)` | 画校准触摸点 |
| `TP_Draw_Big_Point(x,y,color)` | 画大点 (2x2) |
| `TP_Adj_Info_Show(...)` | 显示校准信息 |

### 引脚宏定义

```c
// 使用 CubeMX 生成的 GPIO 宏
#define TOUCH_CS_LOW()    HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_RESET)
#define TOUCH_CS_HIGH()   HAL_GPIO_WritePin(T_CS_GPIO_Port, T_CS_Pin, GPIO_PIN_SET)
#define TOUCH_SCK_LOW()   HAL_GPIO_WritePin(T_SCK_GPIO_Port, T_SCK_Pin, GPIO_PIN_RESET)
#define TOUCH_SCK_HIGH()  HAL_GPIO_WritePin(T_SCK_GPIO_Port, T_SCK_Pin, GPIO_PIN_SET)
#define TOUCH_MOSI_LOW()  HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_RESET)
#define TOUCH_MOSI_HIGH() HAL_GPIO_WritePin(T_MOSI_GPIO_Port, T_MOSI_Pin, GPIO_PIN_SET)
#define TOUCH_MISO_READ() HAL_GPIO_ReadPin(T_MISO_GPIO_Port, T_MISO_Pin)
#define TOUCH_PEN_READ()  HAL_GPIO_ReadPin(T_PEN_GPIO_Port, T_PEN_Pin)
```

### 状态定义

```c
#define TP_PRES_DOWN 0x80   // 触摸屏被按下 (bit 7)
#define TP_CATH_PRES 0x40   // 有按键按下 (bit 6)
```

### 触摸校准原理

触摸坐标转换公式：
```
screen_x = xfac * raw_x + xoff
screen_y = yfac * raw_y + yoff
```

其中：
- `xfac = LCD宽度 / ADC范围`
- `yfac = LCD高度 / ADC范围`
- `xoff, yoff` 用于调整偏移

---

## main.c 初始化顺序

LVGL 的初始化必须遵循正确的顺序：

```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    // 1. 外设初始化 (重要！FSMC必须在LCD之前)
    MX_GPIO_Init();
    MX_FSMC_Init();      // ✅ FSMC 必须先于 LCD_Init()
    MX_TIM6_Init();      // 定时器用于 LVGL 心跳

    // ========== LVGL 初始化区域 ==========
    LCD_Init();                              // 2. 初始化 LCD
    tp_dev.init();                           // 3. 初始化触摸屏 (可选)
    lv_init();                               // 4. LVGL 核心初始化
    lv_port_disp_init();                     // 5. 注册显示驱动
    lv_port_indev_init();                    // 6. 注册触摸输入设备 ⭐
    lv_theme_default_init(...);              // 7. 初始化主题
    HAL_TIM_Base_Start_IT(&htim6);           // 8. 启动心跳定时器

    // ========== 创建 UI ==========
    lv_obj_t *btn = lv_btn_create(lv_scr_act());

    // ========== 主循环 ==========
    while (1) {
        lv_timer_handler();
        HAL_Delay(5);
    }
}
```

---

## 定时器中断配置

使用 TIM6 为 LVGL 提供心跳：

```c
// 在 HAL_TIM_PeriodElapsedCallback 中
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        lv_tick_inc(1);  // 每 1ms 增加一次心跳
    }
}
```

**注意**: TIM6 需要在 CubeMX 中配置为 1ms 中断。

---

## FreeRTOS 注意事项

如果使用 FreeRTOS，`lv_timer_handler()` 必须在任务中调用：

```c
// 在 FreeRTOS 任务中
void lvgl_task(void *argument)
{
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}
```

或者暂时禁用 FreeRTOS 调试 LVGL（注释掉相关代码）。

---

## 常见问题排查

| 问题 | 可能原因 | 解决方案 |
|------|---------|---------|
| 屏幕无显示 | FSMC 未初始化 | 确保 `MX_FSMC_Init()` 在 `LCD_Init()` 之前 |
| 颜色显示错误 | RGB/BGR 顺序错误 | 在 `disp_flush()` 中添加颜色转换 |
| 按钮不显示 | 未初始化主题 | 调用 `lv_theme_default_init()` |
| 触摸无反应 | SPI 通信问题 | 检查 MOSI/MISO/SCK/CS/PEN 引脚接线 |
| 触摸坐标错误 | 校准参数不对 | 调用 `TP_Adjust()` 重新校准 |
| UI 无响应 | `lv_timer_handler()` 未执行 | 确保在循环中正确调用 |

---

## 构建命令

```bash
# 使用 CMake Preset 构建
cmake --build --preset Debug

# 或直接构建
cd build
cmake ..
make
```

---

## 参考资料

- [LVGL 官方文档](https://docs.lvgl.io/)
- [STM32CubeMX 文档](https://www.st.com/en/development-tools/stm32cubemx.html)
- [FreeRTOS 文档](https://www.freertos.org/)
