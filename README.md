# Motor 模块

## 模块描述
`Motor` 模块提供了一个通用的、可扩展的电机控制框架。它采用模板元编程技术，定义了一个顶层包装类 `Motor<MotorType>`，该类将具体的电机驱动实现（如 `RMMotorContainer`）作为模板参数。

这种设计实现了上层业务逻辑与底层硬件驱动的完全解耦。`Motor` 类本身不包含任何具体的控制逻辑，而是将所有功能调用转发给其内部的 `MotorType` 实例，从而实现了零成本的编译时抽象。

## 依赖硬件
依赖于作为模板参数传入的 `MotorType` 的具体要求。
*   当 `MotorType` 为 `RMMotorContainer` 时，依赖：
    *   CAN 总线
    *   大疆系列电机（M3508, M2006, GM6020）
    *   相应电调

## 构造参数
`Motor<MotorType>` 类的构造函数参数如下：

*   `hw`: `LibXR::HardwareContainer&`，硬件容器的引用。
*   `app`: `LibXR::ApplicationManager&`，应用管理器的引用。
*   `can_bus_name`: `const char*`，要使用的硬件总线名称（例如 "can1"）。
*   `param_0` 到 `param_10`: 可选参数，用于配置 0 到 10 号电机。每个参数都是一个 `Param` 结构体。
    *   `Param` 结构体的具体定义由 `MotorType` 提供。对于 `RMMotorContainer`，它包含：
        *   `model`: 电机型号，来自 `RMMotorContainer::Model` 枚举。
        *   `reverse`: `bool` 类型，设置 `true` 将反转电机方向。

## 主要功能
*   **编译时多态**: 通过C++模板实现，允许在不修改代码的情况下切换不同的电机驱动方案。
*   **统一接口**: 为所有不同类型的电机驱动提供了一致的API。
*   **类型安全**: `Motor` 类通过 `using` 关键字直接使用 `MotorType` 中定义的 `Model` 和 `Param` 类型，确保了类型一致性。
*   **易于扩展**: 支持新电机类型只需创建一个符合接口规范的新电机容器类，无需修改 `Motor` 包装类。

## 核心类与结构体
*   `Motor<MotorType>`: 顶层模板包装类。它包含一个 `MotorType` 的实例，并将所有操作转发给它。
*   `MotorType`: 模板参数，代表具体的电机驱动实现，例如 `RMMotorContainer`。
*   `Motor::Model`: 类型别名，实际类型为 `MotorType::Model`。
*   `Motor::Param`: 类型别名，实际类型为 `MotorType::Param`。

## 设计原则与工作方式
`Motor` 模块遵循经典的**包装器模式 (Wrapper Pattern)**，但在编译时完成。

1.  **实例化**: 当你写下 `Motor<RMMotorContainer> motor_can1(...)` 时，`Motor` 类内部会创建一个 `RMMotorContainer` 类型的成员变量 `motor_`。
2.  **构造转发**: `Motor` 的构造函数会将其接收到的所有参数原封不动地转发给 `motor_` 的构造函数。
3.  **方法转发**: 当你调用 `motor_can1.GetMotor(0)` 时，`Motor` 类内部的实现是 `return motor_.GetMotor(0);`。它只是一个简单的调用转发。

这个设计的核心优势是，你的上层代码（例如底盘控制算法）只依赖于 `Motor` 类的接口，而完全不知道底层是 `RMMotorContainer` 还是其他电机驱动。

## 如何使用
```cmd
 admin@LAPTOP-5QH7DV3G D:\....\bsp-dev-c  main  xrobot_add_mod.exe Motor
[SUCCESS] Appended module instance 'Motor' as id 'Motor_1' to User\xrobot.yaml
 admin@LAPTOP-5QH7DV3G D:\....\bsp-dev-c  main  xrobot_gen_main.exe
Discovered modules: BlinkLED, BMI088, BuzzerAlarm, CMD, DR16, IST8310, MadgwickAHRS, Motor, TestMotorControl
[SUCCESS] Generated entry file: User/xrobot_main.hpp
```

## 主要方法
`Motor<MotorType>` 类提供了以下方法，这些方法会直接调用 `MotorType` 实例的同名方法。

*   `typename MotorType::RMMotor* GetMotor(size_t index)`: 获取指定索引的底层电机对象指针。
*   `void SetCurrent(size_t index, float current)`: 设置指定电机的归一化电流 `[-1.0, 1.0]`。
*   `void SetTorque(size_t index, float torque)`: 设置指定电机的扭矩 (N·m)。
*   `float GetAngle(size_t index)`: 获取电机角度 (弧度)。
*   `float GetSpeed(size_t index)`: 获取电机速度 (RPM)。
*   `float GetCurrent(size_t index)`: 获取电机扭矩电流 (安培)。
*   `float GetTemp(size_t index)`: 获取电机温度 (摄氏度)。
