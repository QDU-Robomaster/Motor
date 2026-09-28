# Motor

统一电机控制接口的抽象基类。业务模块只持有 `Motor&` / `Motor*`，通过它屏蔽具体电机驱动
（如 `RMMotor`、`DMMotor`）的差异。

它统一了三件事：

- 控制命令结构（位置 / 速度 / 力矩 / 电流 / MIT）；
- 反馈结构（角度、转速、角速度、扭矩、温度、错误码）；
- 上层模块只依赖抽象接口，不耦合具体驱动实现。

## 依赖

无其他模块依赖，仅使用 LibXR。

本模块是库（manifest `standalone: false`）：它不会被实例化，没有构造参数；
驱动模块和业务模块包含 `Motor.hpp`，并在各自 manifest 的 `depends` 中列出
`QDU-Robomaster/Motor`，由此被拉入工程。

## 公共接口

`Motor` 是纯虚接口：

- 生命周期：`Enable()` / `Disable()` / `Relax()`；
- 反馈刷新：`LibXR::ErrorCode Update()`；
- 反馈读取：`const Feedback& GetFeedback()`；
- 控制下发：`Control(const MotorCmd&)`；
- 维护：`ClearError()` / `SaveZeroPoint()`。

数据结构：

- `Motor::ControlMode`：`MODE_POSITION`、`MODE_VELOCITY`、`MODE_TORQUE`、`MODE_CURRENT`、`MODE_MIT`。
- `Motor::MotorCmd`：`mode`、`reduction_ratio`（默认 1.0）、`torque`、`position`、`velocity`、`kp`、`kd`。
  各驱动只实现其中一部分模式，字段的解释也由驱动决定（例如 `RMMotor` 的 `MODE_CURRENT`
  从 `velocity` 字段读取归一化电流），见各驱动的 README。
- `Motor::Feedback`：`error_id`、`state`、`position`（原始角度）、`abs_angle`、`velocity`（转速）、
  `omega`（角速度）、`torque`、`temp`。`abs_angle` 是 `LibXR::CycleValue<float>`，即归一化到
  [0, 2π) 的单圈角；需要多圈角度时，上层用相邻两次 `abs_angle` 的差值（`CycleValue` 相减得到
  [-π, π) 的最短差）自行累加。

典型用法：

```cpp
void DriveMotor(Motor& motor)
{
  motor.Update();
  const Motor::Feedback& fb = motor.GetFeedback();

  Motor::MotorCmd cmd{};
  cmd.mode = Motor::MODE_TORQUE;
  cmd.torque = 0.5f;
  cmd.reduction_ratio = 1.0f;
  motor.Control(cmd);
}
```

接入新驱动 `MyMotor`：`class MyMotor : public Motor`，实现全部纯虚函数；在 `Control()` 中按
`ControlMode` 分发到底层协议，在 `Update()` 中刷新 `Feedback`。上层模块继续使用 `Motor&`，
无需修改。`Update()` 与 `Control()` 应在固定周期调用。

## 使用

本库由依赖它的模块自动拉入。单独添加源请求：

```sh
xrobot module add QDU-Robomaster/Motor
xrobot setup
```

库没有实例，不需要 `xrobot instance add`，也不在 `User/xrobot.yaml` 的 `modules:` 中出现。

`xrobot module show .`（在本仓库中）或 `xrobot module show Modules/QDU-Robomaster/Motor`
（在 BSP 中）打印 manifest。
