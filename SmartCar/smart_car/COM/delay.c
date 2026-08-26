#include "delay.h"

/**
  * @brief  初始化DWT（数据观察点与跟踪）的CYCCNT计数器
  * @note   必须在首次使用DWT延时函数前调用一次
  * @param  无
  * @retval 无
  */
void DWT_Init(void)
{
	// 使能DWT组件的调试跟踪功能（CoreDebug->DEMCR的TRCENA位）
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	// 使能CYCCNT计数器（DWT->CTRL的CYCCNTENA位）
	// CYCCNT是一个32位自由运行计数器，每个内核时钟周期递增一次
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
  * @brief  基于DWT CYCCNT计数器的微秒级延时函数
  * @note   精度可达CPU时钟周期级，不受中断影响。需确保SystemCoreClock正确设置。
  * @param  us: 需要延时的微秒数（最大值受32位计数器限制，约59秒@72MHz）
  * @retval 无
  */
void DWT_Delay_us(uint32_t us)
{
	// 计算所需的CPU时钟周期数
	// SystemCoreClock是系统时钟频率（单位Hz），例如72MHz时SystemCoreClock = 72000000
	// 每微秒需要的周期数 = SystemCoreClock / 1000000
	// 使用64位乘法防止us过大导致32位溢出（例如us > 59秒时）
	uint32_t ticks = (uint32_t)((uint64_t)us * SystemCoreClock / 1000000UL);

	// 记录起始计数值（CYCCNT是32位寄存器，读取时自动获得当前值）
	uint32_t start = DWT->CYCCNT;

	// 确保DWT计数器已启用（如果之前已调用DWT_Init，这两行可省略，但保留无害）
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	// 等待计数器差值达到所需周期数
	// 注意：32位减法自动处理了计数器溢出回绕的情况（回绕后差值仍正确）
	while ((DWT->CYCCNT - start) < ticks)
	{
		// 空循环，等待时间到达
	}
}
