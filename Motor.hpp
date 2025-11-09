#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: No description provided
constructor_args:
   - can_bus_name: "can1"
   - motor0:
         model: Motor<RMMotorContainer>::Model::MOTOR_NONE
         reverse: false
template_args:
 - MotorType: RMMotorContainer
required_hardware:
  - can
depends: []
=== END MANIFEST === */
// clang-format on

#include <cstddef>

#include "RMMotorContainer.hpp"
#include "app_framework.hpp"
#include "libxr_def.hpp"

/**
 * @brief 电机容器模板类
 * @tparam MotorType 电机类型（如RMMotorContainer）
 */
template <typename MotorType>
class Motor : public LibXR::Application {
 public:
  using Model = typename MotorType::Model;
  using Param = typename MotorType::Param;

  /**
   * @brief 构造函数
   * @param hw 硬件容器
   * @param app 应用管理器
   * @param can_bus_name CAN总线名称
   * @param param_0~param_10 11个电机的参数配置
   */
  Motor(LibXR::HardwareContainer& hw, LibXR::ApplicationManager& app,
        const char* can_bus_name, Param param_0 = {Model::MOTOR_NONE, false},
        Param param_1 = {Model::MOTOR_NONE, false},
        Param param_2 = {Model::MOTOR_NONE, false},
        Param param_3 = {Model::MOTOR_NONE, false},
        Param param_4 = {Model::MOTOR_NONE, false},
        Param param_5 = {Model::MOTOR_NONE, false},
        Param param_6 = {Model::MOTOR_NONE, false},
        Param param_7 = {Model::MOTOR_NONE, false},
        Param param_8 = {Model::MOTOR_NONE, false},
        Param param_9 = {Model::MOTOR_NONE, false},
        Param param_10 = {Model::MOTOR_NONE, false})
      : motor_(hw, app, can_bus_name, param_0, param_1, param_2, param_3,
               param_4, param_5, param_6, param_7, param_8, param_9, param_10) {
    UNUSED(hw);
    UNUSED(app);
    UNUSED(can_bus_name);
    app.Register(motor_);
  }

  void OnMonitor() override {}

  /**
   * @brief 获取指定索引的电机指针
   * @param index 电机索引 (0-10)
   * @return 电机指针，如果索引无效则返回 nullptr
   */
  typename MotorType::RMMotor* GetMotor(size_t index) {
    return motor_.GetMotor(index);
  }

  /**
   * @brief 设置指定电机的电流控制
   * @param index 电机索引
   * @param current 归一化电流值 [-1.0, 1.0]
   */
  void SetCurrent(size_t index, float current) {
    auto* motor = GetMotor(index);
    if (motor != nullptr) {
      motor->CurrentControl(current);
    }
  }

  void Update() { motor_.Update(); }

  /**
   * @brief 获取指定电机的角度
   * @param index 电机索引
   * @return 电机角度（弧度）
   */
  float GetAngle(size_t index) {
    auto* motor = GetMotor(index);
    return (motor != nullptr) ? motor->GetAngle() : 0.0f;
  }

  /**
   * @brief 获取指定电机的速度
   * @param index 电机索引
   * @return 电机速度
   */
  float GetRPM(size_t index) {
    auto* motor = GetMotor(index);
    return (motor != nullptr) ? motor->GetSpeed() : 0.0f;
  }

  float GetOmega(size_t index) {
    auto* motor = GetMotor(index);
    return (motor != nullptr) ? motor->GetSpeed() / 184.6153 : 0.0f;
  }

  /**
   * @brief 获取指定电机的电流
   * @param index 电机索引
   * @return 电机电流
   */
  float GetCurrent(size_t index) {
    auto* motor = GetMotor(index);
    return (motor != nullptr) ? motor->GetCurrent() : 0.0f;
  }

  /**
   * @brief 获取指定电机的温度
   * @param index 电机索引
   * @return 电机温度
   */
  float GetTemp(size_t index) {
    auto* motor = GetMotor(index);
    return (motor != nullptr) ? motor->GetTemp() : 0.0f;
  }

 private:
  MotorType motor_;  // 电机容器实例
};
