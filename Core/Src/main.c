/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "uart.h"
#include "gpio.h"
#include "fsmc.h"
#include "software_timer.h"
#include "lcd.h"
#include "button.h"
#include "picture.h"
#include "led_7seg.h"
#include "ds3231.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_uart();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define WATCH_TIME 0
#define SET_TIME 1
#define SET_ALARM 2
int mode = 0;
int toggle_blink = 0;
int count_toggle_blink = 0;
int set_mode = 0;
int temp_hour = 0;
int temp_min = 0;
int temp_sec = 0;
int temp_date = 0;
int temp_month = 0;
int temp_year = 0;
int alarm_hour = 0;
int alarm_min = 0;
int alarm_sec = 0;
int alarm_date = 0;
int alarm_month = 0;
int alarm_year = 0;
void update_set_time();
void update_set_alarm();
void updateTime();
void display_clock();
void set_time();
void my_clock();
void normal_clock();


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
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  lcd_Clear(WHITE);
  updateTime();
  while (1)
  {
    /* USER CODE END WHILE */
    while(!flag_timer2);
	flag_timer2 = 0;
    count_toggle_blink++;
    if (count_toggle_blink > 10) {
      count_toggle_blink = 0;
      toggle_blink = 1 - toggle_blink;
    }
    button_Scan();
    ds3231_ReadTime();
    my_clock();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init() {
  timer_init();
  led7_init();
  button_init();
  lcd_init();
  uart_init_rs232();
  ds3231_init();
  setTimer2(50);
}

void normal_clock() {
  static int count = 0;
  if (count == 20) {
    lcd_Clear(WHITE);
    count = 0;
    ds3231_sec = (ds3231_sec < 59) ? ds3231_sec + 1 : 0;
    if (ds3231_sec == 0) {
      ds3231_min = (ds3231_min < 59) ? ds3231_min + 1 : 0;
      if (ds3231_min == 0) {
        ds3231_hours = (ds3231_hours < 23) ? ds3231_hours + 1 : 0;
        if (ds3231_hours == 0) {
          ds3231_day = (ds3231_day < 7) ? ds3231_day + 1 : 0;
          if (ds3231_day == 0) {
            ds3231_date = (ds3231_date < 31) ? ds3231_date + 1 : 0;
            if (ds3231_date == 0) {
              ds3231_month = (ds3231_month < 12) ? ds3231_month + 1 : 0;
              if (ds3231_month == 0) {
                ds3231_year = (ds3231_year < 99) ? ds3231_year + 1 : 0;
              }
            }
          }
        }
      }
    }
    temp_sec = ds3231_sec;
      temp_min = ds3231_min;
      temp_hour = ds3231_hours;
      temp_date = ds3231_date;
      temp_month = ds3231_month;
      temp_year = ds3231_year;
  }

  count++;
}

void updateTime() {
  ds3231_Write(ADDRESS_YEAR, 24);
  ds3231_Write(ADDRESS_MONTH, 11);
  ds3231_Write(ADDRESS_DATE, 20);
  ds3231_Write(ADDRESS_DAY, 6);
  ds3231_Write(ADDRESS_HOUR, 12);
  ds3231_Write(ADDRESS_MIN, 7);
  ds3231_Write(ADDRESS_SEC, 20);
}
void display_clock() {
  lcd_ShowIntNum(70, 100, ds3231_hours, 2, BLACK, WHITE, 24);
  lcd_ShowIntNum(110, 100, ds3231_min, 2, BLACK, WHITE, 24);
  lcd_ShowIntNum(150, 100, ds3231_sec, 2, BLACK, WHITE, 24);
  lcd_ShowIntNum(20, 130, ds3231_day, 2, YELLOW, BLACK, 24);
  lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
  lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
  lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
}
void display_temp_clock() {
  lcd_ShowIntNum(70, 100, temp_hour, 2, BLACK, WHITE, 24);
  lcd_ShowIntNum(110, 100, temp_min, 2, BLACK, WHITE, 24);
  lcd_ShowIntNum(150, 100, temp_sec, 2, BLACK, WHITE, 24);
  lcd_ShowIntNum(20, 130, ds3231_day, 2, YELLOW, BLACK, 24);
  lcd_ShowIntNum(70, 130, temp_date, 2, YELLOW, BLACK, 24);
  lcd_ShowIntNum(110, 130, temp_month, 2, YELLOW, BLACK, 24);
  lcd_ShowIntNum(150, 130, temp_year, 2, YELLOW, BLACK, 24);
}
void update_set_time() {
  ds3231_Write(ADDRESS_YEAR, temp_year);
  ds3231_Write(ADDRESS_MONTH, temp_month);
  ds3231_Write(ADDRESS_DATE, temp_date);
  ds3231_Write(ADDRESS_DAY, temp_date);
  ds3231_Write(ADDRESS_HOUR, temp_hour);
  ds3231_Write(ADDRESS_MIN, temp_min);
  ds3231_Write(ADDRESS_SEC, temp_sec);
  ds3231_year = temp_year;
  ds3231_month = temp_month;
  ds3231_date = temp_date;
  ds3231_day = temp_date;
  ds3231_hours = temp_hour;
  ds3231_min = temp_min;
  ds3231_sec = temp_sec;
}
void update_set_alarm() {
  alarm_year = temp_year;
  alarm_month = temp_month;
  alarm_date = temp_date;
  alarm_hour = temp_hour;
  alarm_min = temp_min;
  alarm_sec = temp_sec;
}
void check_alarm() {
  if (ds3231_hours == alarm_hour && ds3231_min == alarm_min && ds3231_sec == alarm_sec) {
    lcd_DrawCircle(120, 60, RED, 10, 1);
  }
}
void set_time() {
  // use 2 button, 1 for increase, 1 for choose the field
  switch (set_mode) {
    case 0:
      if(toggle_blink) {
        lcd_ShowIntNum(150, 100, ds3231_sec, 2, BLACK, BLACK, 24);
      }
      if (button_count[3] == 1) {
        temp_sec = (temp_sec < 59) ? temp_sec + 1 : 0;
      }
      if (button_count[3] > 40 && button_count[3] % 4 == 0) {
        temp_sec = (temp_sec < 59) ? temp_sec + 1 : 0;
      }
      break;
    case 1:
      if(toggle_blink) {
        lcd_ShowIntNum(110, 100, ds3231_min, 2, BLACK, BLACK, 24);
      }
      if (button_count[3] == 1) {
        temp_min = (temp_min < 59) ? temp_min + 1 : 0;
      }
      if (button_count[3] > 40 && button_count[3] % 4 == 0) {
        temp_min = (temp_min < 59) ? temp_min + 1 : 0;
      }
      break;
    case 2:
      if(toggle_blink) {
        lcd_ShowIntNum(70, 100, ds3231_hours, 2, BLACK, BLACK, 24);
      }
      if (button_count[3] == 1) {
        temp_hour = (temp_hour < 23) ? temp_hour + 1 : 0;
      }
      if (button_count[3] > 40 && button_count[3] % 4 == 0) {
        temp_hour = (temp_hour < 23) ? temp_hour + 1 : 0;
      }
      break;
    case 3:
      if(toggle_blink) {
        lcd_ShowIntNum(150, 130, ds3231_year, 2, BLACK, BLACK, 24);
      }
      if (button_count[3] == 1) {
        temp_year = (temp_year < 99) ? temp_year + 1 : 0;
      }
      if (button_count[3] > 40 && button_count[3] % 4 == 0) {
        temp_year = (temp_year < 99) ? temp_year + 1 : 0;
      }
      break;
    case 4:
      if(toggle_blink) {
        lcd_ShowIntNum(110, 130, ds3231_month, 2, BLACK, BLACK, 24);
      }
      if (button_count[3] == 1) {
        temp_month = (temp_month < 12) ? temp_month + 1 : 0;
      }
      if (button_count[3] > 40 && button_count[3] % 4 == 0) {
        temp_month = (temp_month < 12) ? temp_month + 1 : 0;
      }
      break;
    case 5:
      if(toggle_blink) {
        lcd_ShowIntNum(70, 130, ds3231_date, 2, BLACK, BLACK, 24);
      }
      if (button_count[3] == 1) {
        temp_date = (temp_date < 31) ? temp_date + 1 : 0;
      }
      if (button_count[3] > 40 && button_count[3] % 4 == 0) {
        temp_date = (temp_date < 31) ? temp_date + 1 : 0;
      }
      break;
    case 6:
      if(toggle_blink) {
        lcd_ShowIntNum(20, 130, ds3231_day, 2, BLACK, BLACK, 24);
      }
      if (button_count[3] == 1) {
        temp_date = (temp_date < 7) ? temp_date + 1 : 0;
      }
      if (button_count[3] > 40 && button_count[3] % 4 == 0) {
        temp_date = (temp_date < 7) ? temp_date + 1 : 0;
      }
      break;
  }
  
}
void my_clock() {
	static int start = 0;
	if (start == 0) {
		temp_sec = ds3231_sec;
		  temp_min = ds3231_min;
		  temp_hour = ds3231_hours;
		  temp_date = ds3231_date;
		  temp_month = ds3231_month;
		  temp_year = ds3231_year;
		  alarm_hour = ds3231_hours;
		  alarm_min = ds3231_min;
		  alarm_sec = ds3231_sec;
		  alarm_date = ds3231_date;
		  alarm_month = ds3231_month;
		  alarm_year = ds3231_year;
		  start = 2;
	}
  if (button_count[0] == 1) {
    mode = (mode < 2) ? mode + 1 : 0;
  }
  if (button_count[1] == 1) {
    if (mode == SET_TIME) {
      update_set_time();
    } else if (mode == SET_ALARM) {
      update_set_alarm();
    }
    set_mode = (set_mode < 6) ? set_mode + 1 : 0;
    temp_hour = ds3231_hours;
    temp_min = ds3231_min;
    temp_sec = ds3231_sec;
    temp_date = ds3231_date;
    temp_month = ds3231_month;
    temp_year = ds3231_year;
  }
  switch (mode) {
    case WATCH_TIME:
      normal_clock();
      check_alarm();
      display_clock();
      lcd_ShowStr(20, 30, "NORMAL MODE", WHITE, RED, 24, 0);
      break;
    case SET_TIME:
      display_temp_clock();
      set_time();
      lcd_ShowStr(20, 30, "SET TIME MODE", WHITE, RED, 24, 0);
      break;
    case SET_ALARM:
      display_temp_clock();
      set_time();
      lcd_ShowStr(20, 30, "SET ALARM MODE", WHITE, RED, 24, 0);
      break;
  }
}

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
