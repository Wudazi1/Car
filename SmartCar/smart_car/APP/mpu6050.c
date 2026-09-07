#include "mpu6050.h"
#include "delay.h"

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

// 调试相关
static uint8_t angle_print_enabled = 0;
static uint32_t print_counter = 0;

// 滤波输出
float roll_filtered = 0, pitch_filtered = 0.0f;

// Madgwick滤波器相关
static float beta_madgwick = 0.1f;
static float q0 = 1.0f, q1 = 0, q2 = 0, q3 = 0;

// copysign函数
#ifndef copysign
static float my_copysign(float x, float y) {
    return (y >= 0.0f) ? fabsf(x) : -fabsf(x);
}
#define copysign my_copysign
#endif

// ==================== Madgwick算法函数 ==================== //
/**
  * @brief  Madgwick AHRS算法（6轴版本）
  */
static void MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az, float delta_t)
{
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2, _8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    // 加速度计归一化
    recipNorm = 1.0f / sqrtf(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;

    // 四元数微分方程
    qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
    qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
    qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

    // 计算雅可比矩阵
    _2q0 = 2.0f * q0;
    _2q1 = 2.0f * q1;
    _2q2 = 2.0f * q2;
    _2q3 = 2.0f * q3;
    _4q0 = 4.0f * q0;
    _4q1 = 4.0f * q1;
    _4q2 = 4.0f * q2;
    _8q1 = 8.0f * q1;
    _8q2 = 8.0f * q2;
    q0q0 = q0 * q0;
    q1q1 = q1 * q1;
    q2q2 = q2 * q2;
    q3q3 = q3 * q3;

    // 梯度下降算法校正
    s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
    s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
    s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
    s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;

    recipNorm = 1.0f / sqrtf(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;

    // 应用反馈校正
    qDot1 -= beta_madgwick * s0;
    qDot2 -= beta_madgwick * s1;
    qDot3 -= beta_madgwick * s2;
    qDot4 -= beta_madgwick * s3;

    // 积分
    q0 += qDot1 * delta_t;
    q1 += qDot2 * delta_t;
    q2 += qDot3 * delta_t;
    q3 += qDot4 * delta_t;

    // 归一化
    recipNorm = 1.0f / sqrtf(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;

    quat.q0 = q0;
    quat.q1 = q1;
    quat.q2 = q2;
    quat.q3 = q3;
}

/**
  * @brief  四元数转欧拉角
  */
static void Quaternion_To_Euler(void)
{
    float q0q0 = quat.q0 * quat.q0;
    float q0q1 = quat.q0 * quat.q1;
    float q0q2 = quat.q0 * quat.q2;
    float q0q3 = quat.q0 * quat.q3;
    float q1q1 = quat.q1 * quat.q1;
    float q1q2 = quat.q1 * quat.q2;
    float q1q3 = quat.q1 * quat.q3;
    float q2q2 = quat.q2 * quat.q2;
    float q2q3 = quat.q2 * quat.q3;
    float q3q3 = quat.q3 * quat.q3;

    // 横滚角
    float sinr_cosp = 2.0f * (q0q1 + q2q3);
    float cosr_cosp = 1.0f - 2.0f * (q1q1 + q2q2);
    angles.roll = atan2f(sinr_cosp, cosr_cosp) * 180.0f / M_PI;

    // 俯仰角
    float sinp = 2.0f * (q0q2 - q1q3);
    if (fabsf(sinp) >= 1.0f) {
        angles.pitch = copysign(M_PI / 2.0f, sinp) * 180.0f / M_PI;
    } else {
        angles.pitch = asinf(sinp) * 180.0f / M_PI;
    }

    // 偏航角
    float siny_cosp = 2.0f * (q0q3 + q1q2);
    float cosy_cosp = 1.0f - 2.0f * (q2q2 + q3q3);
    angles.yaw = atan2f(siny_cosp, cosy_cosp) * 180.0f / M_PI;

    // 限制角度范围在-180到180度之间
    if (angles.roll > 180.0f) angles.roll -= 360.0f;
    if (angles.roll < -180.0f) angles.roll += 360.0f;
    if (angles.pitch > 180.0f) angles.pitch -= 360.0f;
    if (angles.pitch < -180.0f) angles.pitch += 360.0f;
    if (angles.yaw > 180.0f) angles.yaw -= 360.0f;
    if (angles.yaw < -180.0f) angles.yaw += 360.0f;
}

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
        if(i % 10 == 0) {
            printf("\rCalibrating: %d/%d", i, samples);
        }
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
    printf("\n********************************\n");
    printf("*       IMU Test System        *\n");
    printf("********************************\n\n");

    // 初始化DWT计数器
    DWT_Init();

    printf("MPU6050 initializing...\n");
    MPU6050_Init();
    printf("MPU6050 init OK\n");

    printf("\nCalibrating MPU6050 (keep device stationary)...\n");
    OLED_ShowString(0, 1, "Calibrating...");
    MPU6050_Calibration();
    OLED_Clear();

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
    angles.yaw = 0.0f;
    roll_filtered = roll_angle;
    pitch_filtered = pitch_angle;

    // 初始化四元数
    q0 = 1.0f; q1 = 0; q2 = 0; q3 = 0;
    quat.q0 = 1.0f;
    quat.q1 = 0.0f;
    quat.q2 = 0.0f;
    quat.q3 = 0.0f;

    printf("\nIMU Ready\n");
}

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
            printf("[ZeroBias] Updated: %.4f, %.4f, %.4f °/s\n",
                   gyro_bias_x, gyro_bias_y, gyro_bias_z);
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
                printf("[Status] Angles frozen at Roll=%.2f, Pitch=%.2f\n",
                       roll_angle, pitch_angle);
            }
        }
    }
    else
    {
        stationary_count = 0;
        angles_frozen = 0;
    }

    // === 5. 互补滤波姿态解算（计算roll和pitch）===
    // 从加速度计计算角度
    float accel_roll = atan2f(ay, az) * 180.0f / M_PI;
    float accel_pitch = atan2f(-ax, sqrtf(ay*ay + az*az)) * 180.0f / M_PI;

    // 减小alpha，加快收敛
    float current_alpha = 0.90f;

    // 互补滤波公式
    roll_angle = current_alpha * (roll_angle + gx * dt) + (1.0f - current_alpha) * accel_roll;
    pitch_angle = current_alpha * (pitch_angle + gy * dt) + (1.0f - current_alpha) * accel_pitch;

    // === 6. Madgwick算法计算yaw（四元数）===
    // 转换为弧度
    float gx_rad = gx * DEG_TO_RAD;
    float gy_rad = gy * DEG_TO_RAD;
    float gz_rad = gz * DEG_TO_RAD;

    // Madgwick更新
    MadgwickAHRSupdateIMU(gx_rad, gy_rad, gz_rad, ax, ay, az, dt);

    // 四元数转欧拉角
    Quaternion_To_Euler();

    // 静止时缓慢回归冻结值（漂移抑制）
    if(angles_frozen && stationary_count > 300)
    {
        float beta = 0.001f;
        roll_angle = (1.0f - beta) * roll_angle + beta * frozen_angles.roll;
        pitch_angle = (1.0f - beta) * pitch_angle + beta * frozen_angles.pitch;
        angles.yaw = (1.0f - beta) * angles.yaw + beta * frozen_angles.yaw;
    }

    // 更新全局角度
    angles.roll = roll_angle;
    angles.pitch = pitch_angle;

    // 一阶低通滤波（可选，平滑输出）
    static float alpha_filter = 0.7f;
    roll_filtered = alpha_filter * roll_angle + (1.0f - alpha_filter) * roll_filtered;
    pitch_filtered = alpha_filter * pitch_angle + (1.0f - alpha_filter) * pitch_filtered;

    // === 7. 调试打印（约100ms一次）===
    print_counter++;
    if(print_counter >= 10)
    {
        print_counter = 0;

        // 第一次打印提示
        if(!angle_print_enabled)
        {
            printf("\n=== IMU Output Start ===\n");
            angle_print_enabled = 1;
        }

        // 打印角度 - FireWater协议格式
        printf("%6.2f, %6.2f, %6.2f\n", angles.roll, angles.pitch, angles.yaw);
    }
}
