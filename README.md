# Car - 智能车控制项目

基于 STM32F103VG 的智能车控制程序，实现速度闭环与方向（舵机）控制。

## 硬件平台

- **芯片：** STM32F103VG（Cortex-M3）
- **主频：** 72MHz
- **开发板：** 正点原子 MiniSTM32（或其他兼容板）

## 功能模块

| 模块 | 文件 | 说明 |
|------|------|------|
| 电机控制 | `APP/motor.c/.h` | 电机驱动与 PWM 输出 |
| 编码器 | `APP/encoder.c/.h` | 轮式编码器读取，实现速度反馈 |
| PID 控制 | `APP/pid.c/.h` | 速度闭环 PID 算法 |
| 舵机控制 | `APP/servo.c/.h` | 前后舵机方向控制 |
| 按键 | `APP/key.c/.h` | 按键输入检测 |
| 串口 | `Core/Src/usart.c` | 串口通信（调试/数据上报） |
| 定时器 | `Core/Src/tim.c` | PWM 输出、编码器计数 |

## 项目结构

```
Car/
├── .gitignore              # Git 忽略规则
├── README.md               # 本文件
└── SmartCar/
    └── smart_car/          # STM32CubeMX 生成的标准结构
        ├── APP/            # 业务代码（电机、舵机、PID、编码器、按键）
        ├── Core/           # HAL 层代码
        │   ├── Inc/        # 头文件
        │   └── Src/        # 源文件（gpio, tim, usart, it...）
        ├── Drivers/        # STM32F1xx HAL 驱动库
        ├── MDK-ARM/        # Keil MDK-ARM 工程文件
        └── smart_car.ioc    # STM32CubeMX 配置文件
```

## 编译方式

使用 **Keil MDK-ARM** 打开 `MDK-ARM/smart_car.uvprojx` 进行编译和调试。

## 提交历史规范

- `feat:` 新功能
- `fix:` Bug 修复
- `chore:` 杂项（构建、依赖、配置）
- `docs:` 文档更新

示例：
```
feat: 实现速度闭环 PID 控制
fix: 修正舵机中值偏移
chore: 添加 .gitignore
```

## 作者

吴运龙
