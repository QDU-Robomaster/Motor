#pragma once
// clang-format on

#include <cstdint>

#include "app_framework.hpp"
#include "cycle_value.hpp"
#include "libxr_def.hpp"

class Motor {
 public:
  enum ControlMode : uint8_t {
    MODE_POSITION, /*位置控制模式*/
    MODE_VELOCITY, /*速度控制模式*/
    MODE_TORQUE,   /*力矩控制模式*/
    MODE_CURRENT,  /*电流控制模式*/
    MODE_MIT,      /*MIT控制模式*/
  };

  struct MotorCmd {
    ControlMode mode; /*电机控制模式*/
    float reduction_ratio = 1.0f; /*减速比*/
    float torque = 0;    /*力矩*/
    float position = 0;  /*目标位置*/
    float velocity = 0;  /*目标速度*/
    float kp = 0;        /*刚度*/
    float kd = 0;        /*阻尼*/
  };

  struct Feedback {
    uint8_t error_id;                      /*电机错误码*/
    uint8_t state;                      /*电机错误状态*/
    float position;                     /*电机原始角度*/
    LibXR::CycleValue<float> abs_angle; /*CycleValue的角度*/
    float velocity;                     /*转速*/
    float omega;                        /*角速度*/
    float torque;                       /*扭矩*/
    float temp;                         /*温度*/
  };

  virtual ~Motor();
  virtual void Enable() = 0;
  virtual void Disable() = 0;
  virtual void Relax() = 0;
  virtual ErrorCode Update() = 0;
  virtual const Feedback& GetFeedback() = 0;
  virtual void Control(const MotorCmd& cmd) = 0;
  virtual void ClearError() = 0;
  virtual void SaveZeroPoint() = 0;

 private:
};
