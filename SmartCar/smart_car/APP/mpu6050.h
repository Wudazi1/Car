#ifndef __MPU6050_H
#define __MPU6050_H

#include "system_bsp.h"
#include "soft_i2c.h"

// ==================== I2C引脚定义 ==================== //
#define MPU6050_I2C_DELAY  2   // 微秒延时（与原有实现一致）

// MPU6050 软件I2C对象（独立引脚：PE0/PE1）
extern Soft_I2C_t mpu6050_i2c;

// ==================== MPU6050寄存器地址 ==================== //
#define MPU6050_ADDR          0x68
#define MPU6050_WHO_AM_I      0x75
#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_GYRO_CONFIG   0x1B
#define MPU6050_ACCEL_CONFIG  0x1C
#define MPU6050_CONFIG        0x1A
#define MPU6050_SMPLRT_DIV    0x19
#define MPU6050_ACCEL_XOUT_H  0x3B
#define MPU6050_GYRO_XOUT_H   0x43

// ==================== 数据结构定义 ==================== //
typedef struct
{
	float roll;     // 横滚角 (X轴) 单位：度
	float pitch;    // 俯仰角 (Y轴) 单位：度
} Angles_t;

typedef struct
{
	float q0, q1, q2, q3;  // 四元数（可保留用于其他用途，但互补滤波不再使用）
} Quaternion_t;

typedef struct
{
	float ax, ay, az;   // 加速度 (m/s²)
	float gx, gy, gz;   // 角速度 (rad/s)
} IMU_Data_t;

// ==================== 外部变量声明 ==================== //
extern Angles_t angles;           // 角度输出
extern Quaternion_t quat;         // 四元数（可选）
extern IMU_Data_t imu_data;       // IMU数据
extern float roll_filtered, pitch_filtered;  // 低通滤波后角度

// ==================== 函数声明 ==================== //
void mpu6050_init(void);
void mpu6050_proc(void);

#endif
