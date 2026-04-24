/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lcd.h"
#include "touch.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static uint32_t btn_click_count = 0;  // 按钮点击计数
static lv_obj_t * coord_label = NULL; // 坐标显示标签
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* 按钮事件回调函数 */
static void btn_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);  // 获取触发事件的按钮
    lv_obj_t * label = lv_obj_get_child(btn, 0);  // 获取按钮的标签

    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        btn_click_count++;
        // 更新标签文本显示点击次数
        static char buf[32];
        snprintf(buf, sizeof(buf), "Clicked: %lu", (unsigned long)btn_click_count);
        lv_label_set_text(label, buf);
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  LCD_Init();                             // FSMC初始化后再初始化LCD
  lv_init();                              // LVGL 初始化
  lv_port_disp_init();                    // 注册LVGL的显示任务
  lv_port_indev_init();                   // 注册LVGL的触摸输入设备
  HAL_TIM_Base_Start_IT(&htim6);          // 启动定时器为LVGL提供心跳

  // 等待触摸屏稳定
  HAL_Delay(500);
  LCD_Clear(WHITE);

  /* 创建测试按钮 */
  lv_obj_t *myBtn = lv_btn_create(lv_scr_act());           // 创建按钮
  lv_obj_set_pos(myBtn, 80, 80);                            // 设置坐标
  lv_obj_set_size(myBtn, 160, 60);                         // 设置大小
  lv_obj_add_event_cb(myBtn, btn_event_cb, LV_EVENT_ALL, NULL);  // 添加事件回调

  // 按钮上的文本
  lv_obj_t *label_btn = lv_label_create(myBtn);
  lv_label_set_text(label_btn, "Click Me!");
  lv_obj_center(label_btn);

  // 信息标签
  lv_obj_t *infoLabel = lv_label_create(lv_scr_act());
  lv_label_set_text(infoLabel, "Touch button to test!");
  lv_obj_align(infoLabel, LV_ALIGN_CENTER, 0, 0);

  // 触摸坐标显示标签
  coord_label = lv_label_create(lv_scr_act());
  lv_label_set_text(coord_label, "Touch: ---");
  lv_obj_align(coord_label, LV_ALIGN_BOTTOM_MID, 0, -10);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    lv_timer_handler();  // 处理LVGL任务

    // 更新触摸坐标显示 (调试用)
    if(coord_label != NULL) {
        static char coord_buf[32];
        uint16_t raw_x = tp_dev.x[0];
        uint16_t raw_y = tp_dev.y[0];
        if(tp_dev.sta & TP_PRES_DOWN) {
            snprintf(coord_buf, sizeof(coord_buf), "Touch: X=%d Y=%d", raw_x, raw_y);
        } else {
            snprintf(coord_buf, sizeof(coord_buf), "Touch: ---");
        }
        lv_label_set_text(coord_label, coord_buf);
    }

    HAL_Delay(10);        // 10ms延时
    /* USER CODE END 3 */
  }
  /* USER CODE END WHILE */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM6)
  {
    lv_tick_inc(1);  // 给LVGL提供1ms的心跳
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
