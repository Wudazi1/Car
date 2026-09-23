/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "system_bsp.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for key_proc */
osThreadId_t key_procHandle;
const osThreadAttr_t key_proc_attributes = {
  .name = "key_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for battery_proc */
osThreadId_t battery_procHandle;
const osThreadAttr_t battery_proc_attributes = {
  .name = "battery_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow4,
};
/* Definitions for temp_proc */
osThreadId_t temp_procHandle;
const osThreadAttr_t temp_proc_attributes = {
  .name = "temp_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow6,
};
/* Definitions for eeprom_proc */
osThreadId_t eeprom_procHandle;
const osThreadAttr_t eeprom_proc_attributes = {
  .name = "eeprom_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow2,
};
/* Definitions for mpu6050_proc */
osThreadId_t mpu6050_procHandle;
const osThreadAttr_t mpu6050_proc_attributes = {
  .name = "mpu6050_proc",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for pid_proc */
osThreadId_t pid_procHandle;
const osThreadAttr_t pid_proc_attributes = {
  .name = "pid_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime6,
};
/* Definitions for ws2812_proc */
osThreadId_t ws2812_procHandle;
const osThreadAttr_t ws2812_proc_attributes = {
  .name = "ws2812_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for ps2_proc */
osThreadId_t ps2_procHandle;
const osThreadAttr_t ps2_proc_attributes = {
  .name = "ps2_proc",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for oled_proc */
osThreadId_t oled_procHandle;
const osThreadAttr_t oled_proc_attributes = {
  .name = "oled_proc",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void start_key_proc(void *argument);
void start_battery_proc(void *argument);
void start_temperature_proc(void *argument);
void start_eeprom_proc(void *argument);
void start_mpu6050_proc(void *argument);
void start_pid_proc(void *argument);
void start_ws2812_proc(void *argument);
void start_ps2_proc(void *argument);
void start_oled_proc(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of key_proc */
  key_procHandle = osThreadNew(start_key_proc, NULL, &key_proc_attributes);

  /* creation of battery_proc */
  battery_procHandle = osThreadNew(start_battery_proc, NULL, &battery_proc_attributes);

  /* creation of temp_proc */
  temp_procHandle = osThreadNew(start_temperature_proc, NULL, &temp_proc_attributes);

  /* creation of eeprom_proc */
  eeprom_procHandle = osThreadNew(start_eeprom_proc, NULL, &eeprom_proc_attributes);

  /* creation of mpu6050_proc */
  mpu6050_procHandle = osThreadNew(start_mpu6050_proc, NULL, &mpu6050_proc_attributes);

  /* creation of pid_proc */
  pid_procHandle = osThreadNew(start_pid_proc, NULL, &pid_proc_attributes);

  /* creation of ws2812_proc */
  ws2812_procHandle = osThreadNew(start_ws2812_proc, NULL, &ws2812_proc_attributes);

  /* creation of ps2_proc */
  ps2_procHandle = osThreadNew(start_ps2_proc, NULL, &ps2_proc_attributes);

  /* creation of oled_proc */
  oled_procHandle = osThreadNew(start_oled_proc, NULL, &oled_proc_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_start_key_proc */
/**
* @brief Function implementing the key_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_key_proc */
__weak void start_key_proc(void *argument)
{
  /* USER CODE BEGIN start_key_proc */
  /* Infinite loop */
  for(;;)
  {
    key_proc();

    vTaskDelay(pdMS_TO_TICKS(20)); 
  }
  /* USER CODE END start_key_proc */
}

/* USER CODE BEGIN Header_start_battery_proc */
/**
* @brief Function implementing the battery_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_battery_proc */
__weak void start_battery_proc(void *argument)
{
  /* USER CODE BEGIN start_battery_proc */
  /* Infinite loop */
  for(;;)
  {
    battery_proc();

    vTaskDelay(pdMS_TO_TICKS(1000)); 
  }
  /* USER CODE END start_battery_proc */
}

/* USER CODE BEGIN Header_start_temperature_proc */
/**
* @brief Function implementing the temp_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_temperature_proc */
__weak void start_temperature_proc(void *argument)
{
  /* USER CODE BEGIN start_temperature_proc */
  /* Infinite loop */
  for(;;)
  {
    temperature_proc();

    vTaskDelay(pdMS_TO_TICKS(500));
  }
  /* USER CODE END start_temperature_proc */
}

/* USER CODE BEGIN Header_start_eeprom_proc */
/**
* @brief Function implementing the eeprom_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_eeprom_proc */
__weak void start_eeprom_proc(void *argument)
{
  /* USER CODE BEGIN start_eeprom_proc */
  /* Infinite loop */
  for(;;)
  {
    eeprom_proc();

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  /* USER CODE END start_eeprom_proc */
}

/* USER CODE BEGIN Header_start_mpu6050_proc */
/**
* @brief Function implementing the mpu6050_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_mpu6050_proc */
__weak void start_mpu6050_proc(void *argument)
{
  /* USER CODE BEGIN start_mpu6050_proc */
  /* Infinite loop */
  for(;;)
  {
    mpu6050_proc();

    vTaskDelay(pdMS_TO_TICKS(30)); 
  }
  /* USER CODE END start_mpu6050_proc */
}

/* USER CODE BEGIN Header_start_pid_proc */
/**
* @brief Function implementing the pid_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_pid_proc */
__weak void start_pid_proc(void *argument)
{
  /* USER CODE BEGIN start_pid_proc */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(10);
  /* Infinite loop */
  for(;;)
  {
    pid_proc();

    // 绝对延时
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  /* USER CODE END start_pid_proc */
}

/* USER CODE BEGIN Header_start_ws2812_proc */
/**
* @brief Function implementing the ws2812_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_ws2812_proc */
__weak void start_ws2812_proc(void *argument)
{
  /* USER CODE BEGIN start_ws2812_proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END start_ws2812_proc */
}

/* USER CODE BEGIN Header_start_ps2_proc */
/**
* @brief Function implementing the ps2_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_ps2_proc */
__weak void start_ps2_proc(void *argument)
{
  /* USER CODE BEGIN start_ps2_proc */
  /* Infinite loop */
  for(;;)
  {
    ps2_proc();

    vTaskDelay(pdMS_TO_TICKS(20));
  }
  /* USER CODE END start_ps2_proc */
}

/* USER CODE BEGIN Header_start_oled_proc */
/**
* @brief Function implementing the oled_proc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_oled_proc */
__weak void start_oled_proc(void *argument)
{
  /* USER CODE BEGIN start_oled_proc */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END start_oled_proc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

