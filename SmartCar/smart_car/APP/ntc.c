#include "ntc.h"
#include "stdio.h"
#include "oled.h"

// NTC参数 - HNTC0603-103F3450FA
#define NTC_BETA            3450.0f     // NTC热敏系数(B25 / 85值)
#define NTC_R25             10000.0f    // 25℃时NTC阻值(Ω)
#define PULLDOWN_RESISTOR   40000.0f    // 下拉电阻R8值(Ω) - 40kΩ

void NTC_ADC_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start(&hadc1);
}

static int getMiddleValue(ADC_HandleTypeDef *hadc, int N)
{
    int value_buf[N];
    int i, j, k, temp;
    
    for(i = 0; i < N; i++)
    {
        HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
        value_buf[i] = HAL_ADC_GetValue(hadc);
    }
    
    for(j = 0; j < N-1; j++)
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

float calculate_ntc_resistance(float voltage)
{
    return (3.3f * PULLDOWN_RESISTOR / voltage) - PULLDOWN_RESISTOR;
}

float calculate_temperature(float resistance)
{
    if(resistance <= 0) return -999.0f;
    
    const float T0_K = 273.15f + 25.0f;
    float ln_ratio = logf(resistance / NTC_R25);
    float reciproval_temp = 1.0f / T0_K + (1.0f / NTC_BETA) * ln_ratio;
    float temp_k = 1.0f / reciproval_temp;
    
    return temp_k - 273.15f;
}

int ntc_value = 0;
float ntc_voltage = 0.0f;
float ntc_resistance = 0.0f;
float temperature = 0.0f;
char temperture_buf[16];

void temperature_proc(void)
{
		ntc_value = getMiddleValue(&hadc1, 7);
		ntc_voltage = (ntc_value / 4095.0f) * 3.3f;
		ntc_resistance = calculate_ntc_resistance(ntc_voltage);
		temperature = calculate_temperature(ntc_resistance);

		// 显示电压
		sprintf(temperture_buf, "temperature:%.1f", temperature);
		OLED_ShowString(0,1, temperture_buf);
}
