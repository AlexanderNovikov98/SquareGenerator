#pragma once

#include <Fsm.h>
#include <Timer.h>

#include "Drivers.h"
#include "Settings.h"

class Error_S final : public m::IState {
 public:
  Error_S(std::span<m::IState*> transitions, Drivers& drivers,
          Settings& settings)
      : IState(transitions), drivers_(drivers), settings_(settings) {}

  void entry() override {
    // TODO: send error reason to remote panel
  }

  void run() override {}

  enum class Event : uint32_t {
    Ok = 0,

    Size
  };
  std::optional<uint32_t> checkEvents() override { return std::nullopt; }

  void exit() override {}

 private:
  Drivers& drivers_;
  Settings& settings_;
};