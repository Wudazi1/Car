#include "mpu6050.h"

// ==================== 全局变量定义 ==================== //
int16_t ax_raw, ay_raw, az_raw;  // 加速度原始数据
int16_t gx_raw, gy_raw, gz_raw;  // 陀螺仪原始数据
float ax, ay, az;                // 加速度 (g)
float gx, gy, gz;                // 角速度 (°/s)

// 定义M_PI
#define M_PI 3.14159265358979323846f
#define DEG_TO_RAD (M_PI/180.0f)

// 校准参数
static float accel_offset_x = 0, accel_offset_y = 0, accel_offset_z = 0;

// 角度相关全局变量
Angles_t angles = {0};            // 最终角度输出
Quaternion_t quat = {1, 0, 0, 0}; // 初始化为单位四元数（互补滤波不需要，但保留）
IMU_Data_t imu_data = {0};        // IMU数据

// 陀螺仪零偏校准相关
static float gyro_bias_x = 0, gyro_bias_y = 0, gyro_bias_z = 0;

// 互补滤波参数
static float roll_angle = 0.0f;   // 互补滤波输出的横滚角
static float pitch_angle = 0.0f;  // 互补滤波输出的俯仰角

// 漂移抑制相关
static uint32_t last_online_bias_update = 0;

// ==================== I2C对象定义 ==================== //
// MPU6050 软件I2C对象（独立引脚：PE0/PE1）
Soft_I2C_t mpu6050_i2c = {
    .GPIO_Port = GPIOE,
    .SDA_Pin   = GPIO_PIN_1,
    .SCL_Pin   = GPIO_PIN_0
};

// ==================== MPU6050驱动实现 ==================== //
void MPU6050_Init(void)
{
    // 1. 唤醒设备
    Soft_I2C_WriteRegister(&mpu6050_i2c, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 0x00, MPU6050_I2C_DELAY);
    HAL_Delay(100);
    // 2. 设置陀螺仪量程 ±2000°/s
    Soft_I2C_WriteRegister(&mpu6050_i2c, MPU6050_ADDR, MPU6050_GYRO_CONFIG, 0x18, MPU6050_I2C_DELAY);
    // 3. 设置加速度计量程 ±2g
    Soft_I2C_WriteRegister(&mpu6050_i2c, MPU6050_ADDR, MPU6050_ACCEL_CONFIG, 0x00, MPU6050_I2C_DELAY);
    // 4. 设置采样率
    Soft_I2C_WriteRegister(&mpu6050_i2c, MPU6050_ADDR, MPU6050_SMPLRT_DIV, 0x07, MPU6050_I2C_DELAY);
    // 5. 设置滤波器
    Soft_I2C_WriteRegister(&mpu6050_i2c, MPU6050_ADDR, MPU6050_CONFIG, 0x06, MPU6050_I2C_DELAY);
}

void MPU6050_Read_Raw(void)
{
    uint8_t buffer[14];
    Soft_I2C_ReadRegisters(&mpu6050_i2c, MPU6050_ADDR, MPU6050_ACCEL_XOUT_H, buffer, 14, MPU6050_I2C_DELAY);
    ax_raw = (int16_t)((buffer[0] << 8) | buffer[1]);
    ay_raw = (int16_t)((buffer[2] << 8) | buffer[3]);
    az_raw = (int16_t)((buffer[4] << 8) | buffer[5]);
    gx_raw = (int16_t)((buffer[8] << 8) | buffer[9]);
    gy_raw = (int16_t)((buffer[10] << 8) | buffer[11]);
    gz_raw = (int16_t)((buffer[12] << 8) | buffer[13]);
}

void MPU6050_Calibration(void)
{
    float ax_sum = 0, ay_sum = 0, az_sum = 0;
    float gx_sum = 0, gy_sum = 0, gz_sum = 0;
    int samples = 100;

    printf("Calibrating...\n");

    for(int i = 0; i < samples; i++)
    {
        MPU6050_Read_Raw();
        ax_sum += (float)ax_raw;
        ay_sum += (float)ay_raw;
        az_sum += (float)az_raw;
        gx_sum += (float)gx_raw;
        gy_sum += (float)gy_raw;
        gz_sum += (float)gz_raw;
        if(i % 20 == 0) printf(".");
        HAL_Delay(10);
    }
    printf("\n");

    // 加速度计偏移（单位：g）
    accel_offset_x = (ax_sum / samples) / 16384.0f;
    accel_offset_y = (ay_sum / samples) / 16384.0f;
    accel_offset_z = ((az_sum / samples) / 16384.0f) - 1.0f;

    // 陀螺仪零偏（单位：°/s）
    gyro_bias_x = (gx_sum / samples) / 16.4f;
    gyro_bias_y = (gy_sum / samples) / 16.4f;
    gyro_bias_z = (gz_sum / samples) / 16.4f;

    printf("Calibration completed!\n");
    printf("Accel offsets: X=%.4fg, Y=%.4fg, Z=%.4fg\n",
                 accel_offset_x, accel_offset_y, accel_offset_z);
    printf("Gyro biases:   X=%.2f°/s, Y=%.2f°/s, Z=%.2f°/s\n",
                 gyro_bias_x, gyro_bias_y, gyro_bias_z);
}

// ==================== 对外接口实现 ==================== //
void mpu6050_init(void)
{
    printf("Attitude estimation initialized with complementary filter.\n");
    printf("Please keep IMU stationary for gyro calibration...\n");

    MPU6050_Init();

    OLED_ShowString(0, 1, "Calibrating...");
    MPU6050_Calibration();
    OLED_Clear();
//  buzzer(1);
    HAL_Delay(200);
//  buzzer(0);
    HAL_Delay(100);
//  buzzer(1);
    HAL_Delay(100);
//  buzzer(0);

    // 初始化互补滤波角度
    // 读取一次数据并计算初始角度
    MPU6050_Read_Raw();
    ax = ((float)ax_raw / 16384.0f) - accel_offset_x;
    ay = ((float)ay_raw / 16384.0f) - accel_offset_y;
    az = ((float)az_raw / 16384.0f) - accel_offset_z;

    // 计算初始加速度计角度
    roll_angle = atan2(ay, az) * 180.0f / M_PI;
    pitch_angle = atan2(-ax, sqrt(ay*ay + az*az)) * 180.0f / M_PI;

    angles.roll = roll_angle;
    angles.pitch = pitch_angle;
    roll_filtered = roll_angle;
    pitch_filtered = pitch_angle;
}

float roll_filtered = 0, pitch_filtered = 0.0f;
void mpu6050_proc(void)
{
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();
    float dt = 0.01f;  // 默认10ms

    if (last_time != 0)
    {
        dt = (current_time - last_time) * 0.001f;
        if (dt > 0.1f) dt = 0.01f;  // 限制最大间隔
    }
    last_time = current_time;

    // === 1. 读取原始数据 ===
    MPU6050_Read_Raw();

    // === 2. 转换为g和°/s（减去校准值） ===
    ax = ((float)ax_raw / 16384.0f) - accel_offset_x;
    ay = ((float)ay_raw / 16384.0f) - accel_offset_y;
    az = ((float)az_raw / 16384.0f) - accel_offset_z;
    gx = ((float)gx_raw / 16.4f) - gyro_bias_x;
    gy = ((float)gy_raw / 16.4f) - gyro_bias_y;
    gz = ((float)gz_raw / 16.4f) - gyro_bias_z;

    // === 3. 在线零偏估计（每5秒） ===
    if(current_time - last_online_bias_update > 5000)
    {
        float accel_sq = ax*ax + ay*ay + az*az;
        float gyro_sq = gx*gx + gy*gy + gz*gz;
        if(fabs(accel_sq - 1.0f) < 0.0025f && gyro_sq < 0.01f)
        {
            float alpha = 0.0001f;
            gyro_bias_x = (1.0f - alpha) * gyro_bias_x + alpha * gx;
            gyro_bias_y = (1.0f - alpha) * gyro_bias_y + alpha * gy;
            gyro_bias_z = (1.0f - alpha) * gyro_bias_z + alpha * gz;
            last_online_bias_update = current_time;
        }
    }

    // === 4. 静止检测 ===
    float accel_sq = ax*ax + ay*ay + az*az;
    float gyro_sq = gx*gx + gy*gy + gz*gz;
    uint8_t is_stationary = (fabs(accel_sq - 1.0f) < 0.0025f) && (gyro_sq < 0.01f);

    static uint32_t stationary_count = 0;
    static uint8_t angles_frozen = 0;
    static Angles_t frozen_angles = {0};

    if(is_stationary)
    {
        stationary_count++;
        if(stationary_count > 300) // 静止3秒
        {
            if(!angles_frozen)
            {
                frozen_angles.roll = roll_angle;
                frozen_angles.pitch = pitch_angle;
                angles_frozen = 1;
            }
        }
    }
    else
    {
        stationary_count = 0;
        angles_frozen = 0;
    }

    // === 5. 互补滤波姿态解算 ===
    // 从加速度计计算角度
    float accel_roll = atan2(ay, az) * 180.0f / M_PI;
    float accel_pitch = atan2(-ax, sqrt(ay*ay + az*az)) * 180.0f / M_PI;

    // 减小alpha，加快收敛
    float current_alpha = 0.90f;   // 原为0.98，现改为0.90

    // 互补滤波公式
    roll_angle = current_alpha * (roll_angle + gx * dt) + (1.0f - current_alpha) * accel_roll;
    pitch_angle = current_alpha * (pitch_angle + gy * dt) + (1.0f - current_alpha) * accel_pitch;

    // 静止时缓慢回归冻结值（漂移抑制）
    if(angles_frozen && stationary_count > 300)
    {
        float beta = 0.001f;  // 回归速度
        roll_angle = (1.0f - beta) * roll_angle + beta * frozen_angles.roll;
        pitch_angle = (1.0f - beta) * pitch_angle + beta * frozen_angles.pitch;
    }

    // 更新全局角度
    angles.roll = roll_angle;
    angles.pitch = pitch_angle;

    // 一阶低通滤波（可选，平滑输出）
    static float alpha_filter = 0.7f;
    roll_filtered = alpha_filter * roll_angle + (1.0f - alpha_filter) * roll_filtered;
    pitch_filtered = alpha_filter * pitch_angle + (1.0f - alpha_filter) * pitch_filtered;
}
