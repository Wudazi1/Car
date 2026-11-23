# Car - 智能车控制项目

基于 STM32F103VG 的智能车控制程序，实现速度闭环与方向（舵机）控制。

## 硬件平台

* **芯片：** STM32F103VG（Cortex-M3）
* **主频：** 72MHz
* **开发板：** 正点原子 MiniSTM32（或其他兼容板）

## 功能模块

|模块|文件|说明|
|-|-|-|
|电机控制|`APP/motor.c/.h`|电机驱动与 PWM 输出|
|编码器|`APP/encoder.c/.h`|轮式编码器读取，实现速度反馈|
|PID 控制|`APP/pid.c/.h`|速度闭环 PID 算法|
|舵机控制|`APP/servo.c/.h`|前后舵机方向控制|
|按键|`APP/key.c/.h`|按键输入检测|
|手柄遥控|`APP/ps2.c/.h`|PS2 手柄按键 / 摇杆解析，遥控输入|
|串口|`Core/Src/usart.c`|串口通信（调试/数据上报）|
|定时器|`Core/Src/tim.c`|PWM 输出、编码器计数|

## 遥控手柄

小车支持 **PS2 手柄遥控**：左摇杆控制速度（前进/后退），右摇杆控制转向，各按键功能见下图。

!\[PS2 手柄键位功能图](PS2手柄键位功能图.png)

> 高清大图：\\\[`PS2手柄键位功能图.png`](PS2手柄键位功能图.png) · 网页版（含完整键位对照表）：\\\[`PS2手柄键位功能图.html`](PS2手柄键位功能图.html) · 键位设计稿：`ps2手柄键位功能.xlsx`

* ✅ **已实现**：左摇杆、右摇杆、▲ 加档、▼ 减档、× 四轮转向、R1 刹车
* ⏳ **规划中**：L1 刹车、L2/R2 转圈、△ 自动大灯、□ 自动巡线、○ 灯光秀、←/→ OLED 切页、SELECT/START/MODE 预留
* ⚠️ 手柄需处于**绿灯（模拟）模式**，否则摇杆无响应

## 项目结构

```
Car/
├── .gitignore              # Git 忽略规则
├── README.md               # 本文件
├── PS2手柄键位功能图.png     # 手柄键位功能图（高清大图）
├── PS2手柄键位功能图.html    # 手柄键位功能图（网页版，含键位对照表）
├── ps2手柄底图.webp          # 键位图底图素材
└── SmartCar/
    └── smart\\\_car/          # STM32CubeMX 生成的标准结构
        ├── APP/            # 业务代码（电机、舵机、PID、编码器、按键、手柄）
        ├── Core/           # HAL 层代码
        │   ├── Inc/        # 头文件
        │   └── Src/        # 源文件（gpio, tim, usart, it...）
        ├── Drivers/        # STM32F1xx HAL 驱动库
        ├── MDK-ARM/        # Keil MDK-ARM 工程文件
        └── smart\\\_car.ioc    # STM32CubeMX 配置文件
```

## 编译方式

使用 **Keil MDK-ARM** 打开 `MDK-ARM/smart\\\_car.uvprojx` 进行编译和调试。

## 提交历史规范

* `feat:` 新功能
* `fix:` Bug 修复
* `chore:` 杂项（构建、依赖、配置）
* `docs:` 文档更新

示例：

```
feat: 实现速度闭环 PID 控制
fix: 修正舵机中值偏移
chore: 添加 .gitignore
```

## 作者

炟子

