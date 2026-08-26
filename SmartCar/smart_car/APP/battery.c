#include "battery.h"
#include "stdio.h"
#include "oled.h"

// ADC 初始化
void ADC_init(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start(&hadc1);
}

// 中值滤波函数
int getMiddleValue(ADC_HandleTypeDef *hadc, int N)
{
	int value_buf[N];
	int i,j,k,temp;
	
	for(i = 0; i < N; i++)
	{
		HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
		value_buf[i] = HAL_ADC_GetValue(hadc);
	}
	
	// 冒泡排序
	for(j = 0; k < N-1; k++)
	{
		for(k = 0; k < N-1-j; k++)
		{
			if(value_buf[k] > value_buf[k+1])
			{
				temp = value_buf[k];
				value_buf[k] = value_buf[k+1];
				value_buf[k+1] = temp;
			}
		}
	}
	return value_buf[(N-1)/2];
}

int battery_value = 0;
float battery_voltage = 0.0f;
char display_buf[16];

void adc_proc(void)
{
	// 获取电压
	battery_value = getMiddleValue(&hadc1, 7);
	battery_voltage = battery_value / 4095.0f * 3.3f * 4;
	
	// 显示电压
	sprintf(display_buf, "Bat:%.1fV", battery_voltage);
	OLED_ShowString(0,1, display_buf);
}
