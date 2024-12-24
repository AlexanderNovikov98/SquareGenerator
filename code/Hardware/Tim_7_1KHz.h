#pragma once
#include <IIt.h>

#include "tim.h"

inline const std::function<void()>* Tim_7_1kHz_cb_ = nullptr;

class Tim_7_1kHz final : public m::ifc::mcu::IIt {
 public:
  Tim_7_1kHz(const std::function<void()>& cb) : IIt(cb) {
    Tim_7_1kHz_cb_ = &cb;
    htim7.PeriodElapsedCallback = [](TIM_HandleTypeDef* htim) {
      (*Tim_7_1kHz_cb_)();
    };
  }

  bool start() override {
    if (running_) return false;
    if (HAL_TIM_Base_Start_IT(&htim7) == HAL_OK) {
      running_ = true;
      return true;
    } else {
      return false;
    }
  }

  bool running() override { return running_; }

  bool stop() override {
    if (!running_) return false;
    if (HAL_TIM_Base_Stop_IT(&htim7) == HAL_OK) {
      running_ = false;
      return true;
    } else {
      return false;
    }
  }

 private:
  bool running_ = false;
};
