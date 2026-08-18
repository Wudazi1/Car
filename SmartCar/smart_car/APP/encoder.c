#include "encoder.h"

#define PULSES_PER_REV 1100      // 11对磁极 x 25减速比 x 4倍频 = 1100
#define RPM_FILTER_ALPHA 0.2f    // 编码器滤波系数
#define ENCODER_MAX_COUNT 65535  // 编码器最大计数值
#define WHEEL_DIAMETER_MM 60.0f  // 轮胎直径 60mm
#define PI 3.14159265f           // 圆周率

void encoder_init(void)
{
	// 启动电机1编码模式
	HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
	// 启动电机2编码模式
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
	// 启动电机3编码模式
	HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
	// 启动电机4编码模式
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
}
 
// 获取电机1的编码差值
int16_t encoder1_read(void)
{
	static uint16_t last_count = 0;
	
	uint16_t count = __HAL_TIM_GET_COUNTER(&htim4);
	int16_t delta = (int16_t)(count - last_count);
	last_count = count;
	
	return delta;
}

// 获取电机1的转速
int encoder1_getrpm_smooth(void)
{
	static uint32_t last_time = 0;
	static float filtered_rpm = 0;
	
	uint32_t current_time = HAL_GetTick();
	
	if(last_time == 0) 
	{
		last_time = current_time;
		return 0;
	}
	
	uint32_t elapsed_ms = current_time - last_time;
	
	// 确保采样时间合理（避免除零和过大间隔）
	if(elapsed_ms < 1) elapsed_ms = 1;
	if(elapsed_ms > 100) elapsed_ms = 100;  // 最大100ms间隔
	
	int16_t pulse_count = encoder1_read();
	
	// 计算当前RPM
	int current_rpm = (int)((pulse_count * 60000.0f) / (PULSES_PER_REV * elapsed_ms));
	
	// 低通滤波
	if(filtered_rpm == 0) filtered_rpm = current_rpm;  // 初始化
	else filtered_rpm = RPM_FILTER_ALPHA * current_rpm + (1 - RPM_FILTER_ALPHA) * filtered_rpm;
	last_time = current_time;
	
	return (int)filtered_rpm;
}

// 获取电机2的编码差值
int16_t encoder2_read(void)
{
	static uint16_t last_count = 0;
	
	uint16_t count = __HAL_TIM_GET_COUNTER(&htim5);
	int16_t delta = (int16_t)(count - last_count);
	last_count = count;
	
	return delta;
}

// 获取电机2的转速
int encoder2_getrpm_smooth(void)
{
	static uint32_t last_time = 0;
	static float filtered_rpm = 0;
	
	uint32_t current_time = HAL_GetTick();
	
	if(last_time == 0) 
	{
		last_time = current_time;
		return 0;
	}
	
	uint32_t elapsed_ms = current_time - last_time;
	
	// 确保采样时间合理（避免除零和过大间隔）
	if(elapsed_ms < 1) elapsed_ms = 1;
	if(elapsed_ms > 100) elapsed_ms = 100;  // 最大100ms间隔
	
	int16_t pulse_count = encoder2_read();
	
	// 计算当前RPM
	int current_rpm = (int)((pulse_count * 60000.0f) / (PULSES_PER_REV * elapsed_ms));
	
	// 低通滤波
	if(filtered_rpm == 0) filtered_rpm = current_rpm;  // 初始化
	else filtered_rpm = RPM_FILTER_ALPHA * current_rpm + (1 - RPM_FILTER_ALPHA) * filtered_rpm;
	last_time = current_time;
	
	return (int)filtered_rpm;
}

// 获取电机3的编码差值（需反向加负号）
int16_t encoder3_read(void)
{
	static uint16_t last_count = 0;
	
	uint16_t count = __HAL_TIM_GET_COUNTER(&htim8);
	int16_t delta = (int16_t)(count - last_count);
	last_count = count;
	
	return -delta;
}

// 获取电机3的转速
int encoder3_getrpm_smooth(void)
{
	static uint32_t last_time = 0;
	static float filtered_rpm = 0;
	
	uint32_t current_time = HAL_GetTick();
	
	if(last_time == 0) 
	{
		last_time = current_time;
		return 0;
	}
	
	uint32_t elapsed_ms = current_time - last_time;
	
	// 确保采样时间合理（避免除零和过大间隔）
	if(elapsed_ms < 1) elapsed_ms = 1;
	if(elapsed_ms > 100) elapsed_ms = 100;  // 最大100ms间隔
	
	int16_t pulse_count = encoder3_read();
	
	// 计算当前RPM
	int current_rpm = (int)((pulse_count * 60000.0f) / (PULSES_PER_REV * elapsed_ms));
	
	// 低通滤波
	if(filtered_rpm == 0) filtered_rpm = current_rpm;  // 初始化
	else filtered_rpm = RPM_FILTER_ALPHA * current_rpm + (1 - RPM_FILTER_ALPHA) * filtered_rpm;
	last_time = current_time;
	
	return (int)filtered_rpm;
}

// 获取电机4的编码差值（需反向加负号）
int16_t encoder4_read(void)
{
	static uint16_t last_count = 0;
	
	uint16_t count = __HAL_TIM_GET_COUNTER(&htim2);
	int16_t delta = (int16_t)(count - last_count);
	last_count = count;
	
	return -delta;
}

// 获取电机4的转速
int encoder4_getrpm_smooth(void)
{
	static uint32_t last_time = 0;
	static float filtered_rpm = 0;
	
	uint32_t current_time = HAL_GetTick();
	
	if(last_time == 0) 
	{
		last_time = current_time;
		return 0;
	}
	
	uint32_t elapsed_ms = current_time - last_time;
	
	// 确保采样时间合理（避免除零和过大间隔）
	if(elapsed_ms < 1) elapsed_ms = 1;
	if(elapsed_ms > 100) elapsed_ms = 100;  // 最大100ms间隔
	
	int16_t pulse_count = encoder4_read();
	
	// 计算当前RPM
	int current_rpm = (int)((pulse_count * 60000.0f) / (PULSES_PER_REV * elapsed_ms));
	
	// 低通滤波
	if(filtered_rpm == 0) filtered_rpm = current_rpm;  // 初始化
	else filtered_rpm = RPM_FILTER_ALPHA * current_rpm + (1 - RPM_FILTER_ALPHA) * filtered_rpm;
	last_time = current_time;
	
	return (int)filtered_rpm;
}

// ==================== 速度计算相关 ==================== //
/**
 * @brief: 获取四个电机的平均转速（去除最高和最低值）
 * @param: 无
 * @retval: 四个电机的平均转速(RPM)
 */
int get_average_rpm(void)
{
	static int rpm_array[4];
	static int rpm_sorted[4];
	int sum = 0;
	int i, j, temp;

	// 获取四个电机的当前转速
	rpm_array[0] = encoder1_getrpm_smooth();
	rpm_array[1] = encoder2_getrpm_smooth();
	rpm_array[2] = encoder3_getrpm_smooth();
	rpm_array[3] = encoder4_getrpm_smooth();

	// 复制数组用于排序
	for(i = 0; i < 4; i++)
	{
		rpm_sorted[i] = rpm_array[i];
	}

	// 冒泡排序（从小到大）
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3 - i; j++)
		{
			if(rpm_sorted[j] > rpm_sorted[j + 1])
			{
				temp = rpm_sorted[j];
				rpm_sorted[j] = rpm_sorted[j + 1];
				rpm_sorted[j + 1] = temp;
			}
		}
	}

	// 计算中间两个的平均值（去掉最高和最低）
	sum = rpm_sorted[1] + rpm_sorted[2];

	return sum / 2;
}

/**
 * @brief: 将转速转换为线速度 (m/s)
 * @param: rpm - 电机转速 (RPM)
 * @retval: 线速度 (m/s)
 */
float rpm_to_speed_ms(int rpm)
{
	// 计算轮子周长（米）
	float wheel_circumference = (WHEEL_DIAMETER_MM / 1000.0f) * PI;

	// 计算每分钟行走的距离（米）
	float distance_per_minute = rpm * wheel_circumference;

	// 转换为米/秒
	float speed_ms = distance_per_minute / 60.0f;

	return speed_ms;
}

/**
 * @brief: 获取机器人当前的线速度（m/s）
 * @param: 无
 * @retval: 线速度 (m/s)
 */
float get_robot_speed(void)
{
	int average_rpm = get_average_rpm();
	return rpm_to_speed_ms(average_rpm);
}

// ==================== 里程计算相关 ==================== // 
// 里程统计变量（累计各电机转过的总转数）
static uint32_t motor1_total_rev = 0;  // 电机1总转数
static uint32_t motor2_total_rev = 0;  // 电机2总转数
static uint32_t motor3_total_rev = 0;  // 电机3总转数
static uint32_t motor4_total_rev = 0;  // 电机4总转数

// 上次计数值（用于计算增量）
static uint32_t motor1_last_count = 0;
static uint32_t motor2_last_count = 0;
static uint32_t motor3_last_count = 0;
static uint32_t motor4_last_count = 0;

// 累计总脉冲数
static uint32_t motor1_total_pulses = 0;  // 电机1总脉冲数
static uint32_t motor2_total_pulses = 0;  // 电机2总脉冲数
static uint32_t motor3_total_pulses = 0;  // 电机3总脉冲数
static uint32_t motor4_total_pulses = 0;  // 电机4总脉冲数

/**
 * @brief: 更新电机1的里程统计（使用增量累计方式）
 * @param: 无
 * @retval: 无
 */
void update_motor1_odometer(void)
{
	uint16_t current_count = __HAL_TIM_GET_COUNTER(&htim4);

	// 计算增量（处理溢出情况）
	int32_t delta = (int32_t)current_count - (int32_t)(motor1_last_count & 0xFFFF);

	// 如果检测到溢出（从大到小跳变）
	if(delta < -32768) delta += 65536;  // 补偿溢出
	// 如果检测到反向溢出（从小到大跳变，通常不会发生）
	else if(delta > 32768) delta -= 65536;

	// 累计脉冲数
	if(delta != 0)
	{
		// 累加绝对值，使里程只增不减
		motor1_total_pulses += (delta > 0) ? delta : -delta;
		motor1_total_rev = motor1_total_pulses / PULSES_PER_REV;
	}

	motor1_last_count = current_count;
}

/**
 * @brief: 更新电机2的里程统计（使用增量累计方式）
 * @param: 无
 * @retval: 无
 */
void update_motor2_odometer(void)
{
	uint16_t current_count = __HAL_TIM_GET_COUNTER(&htim5);

	// 计算增量（处理溢出情况）
	int32_t delta = (int32_t)current_count - (int32_t)(motor2_last_count & 0xFFFF);

	// 处理溢出
	if(delta < -32768) delta += 65536;
	else if(delta > 32768) delta -= 65536;

	// 累计脉冲数
	if(delta != 0)
	{
		// 累加绝对值，使里程只增不减
		motor2_total_pulses += (delta > 0) ? delta : -delta;
		motor2_total_rev = motor2_total_pulses / PULSES_PER_REV;
	}

	motor2_last_count = current_count;
}

/**
 * @brief: 更新电机3的里程统计（电机3是反向的，使用增量累计方式）
 * @param: 无
 * @retval: 无
 */
void update_motor3_odometer(void)
{
	uint16_t current_count = __HAL_TIM_GET_COUNTER(&htim8);

	// 电机3方向相反，增量取负
	int32_t delta = (int32_t)current_count - (int32_t)(motor3_last_count & 0xFFFF);

	// 处理溢出
	if(delta < -32768) delta += 65536;
	else if(delta > 32768) delta -= 65536;

	// 累计脉冲数（方向相反）
	if(delta != 0)
	{
		// 直接累加绝对值，不再需要反转
		motor3_total_pulses += (delta > 0) ? delta : -delta;
		motor3_total_rev = motor3_total_pulses / PULSES_PER_REV;;
	}

	motor3_last_count = current_count;
}

/**
 * @brief: 更新电机4的里程统计（电机4是反向的，使用增量累计方式）
 * @param: 无
 * @retval: 无
 */
void update_motor4_odometer(void)
{
	uint16_t current_count = __HAL_TIM_GET_COUNTER(&htim2);

	// 电机4方向相反，增量取负
	int32_t delta = (int32_t)current_count - (int32_t)(motor4_last_count & 0xFFFF);

	// 处理溢出
	if(delta < -32768) delta += 65536;
	else if(delta > 32768) delta -= 65536;

	// 累计脉冲数（方向相反）
	if(delta != 0)
	{
		// 直接累加绝对值，不再需要反转
		motor4_total_pulses += (delta > 0) ? delta : -delta;
		motor4_total_rev = motor4_total_pulses / PULSES_PER_REV;
	}

	motor4_last_count = current_count;
}

/**
 * @brief: 获取四个电机的平均转数（去除最高和最低值）
 * @param: 无
 * @retval: 四个电机的平均转数
 */
uint32_t get_average_total_rev(void)
{
	static uint32_t rev_array[4];
	static uint32_t rev_sorted[4];
	uint32_t sum = 0;
	int i, j;
	uint32_t temp;

	// 获取四个电机的总转数
	rev_array[0] = motor1_total_rev;
	rev_array[1] = motor2_total_rev;
	rev_array[2] = motor3_total_rev;
	rev_array[3] = motor4_total_rev;

	// 复制数组用于排序
	for(i = 0; i < 4; i++)
	{
		rev_sorted[i] = rev_array[i];
	}

	// 冒泡排序（从小到大）
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3 - i; j++)
		{
			if(rev_sorted[j] > rev_sorted[j + 1])
			{
				temp = rev_sorted[j];
				rev_sorted[j] = rev_sorted[j + 1];
				rev_sorted[j + 1] = temp;
			}
		}
	}

	// 计算中间两个的平均值（去掉最高和最低）
	sum = rev_sorted[1] + rev_sorted[2];

	return sum / 2;
}
