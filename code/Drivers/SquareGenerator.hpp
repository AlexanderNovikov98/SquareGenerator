#ifndef SQUAREGENERATOR_HPP
#define SQUAREGENERATOR_HPP

#include "ISquareGenerator.hpp"
#include <array>
#include <algorithm> 

#include "tim.h"

class SquareGenerator : public ISquareGenerator {
public:

    // Singleton Accessor
    static SquareGenerator& getInstance() {
        static SquareGenerator instance;
        return instance;
    }

    // Delete copy constructor and assignment operator (Singleton pattern)
    SquareGenerator(const SquareGenerator&) = delete;
    SquareGenerator& operator=(const SquareGenerator&) = delete;

    // Implement ISquareGenerator Interface
    bool add(std::span<Packet> signal) override {
        if (signal.size() > MAX_SIGNAL_SIZE) return false;
        std::copy(signal.begin(), signal.end(), buffer_.begin());
        bufferSize_ = signal.size();
        return true;
    }

    void start() override {
        if (bufferSize_ == 0 || running_) return;
        currentIndex_ = 0;
        running_ = true;
        startNextPacket();
    }

    void stop() override {
        running_ = false;
        HAL_TIM_OC_Stop_IT(&htim1, TIM_CHANNEL_1);
    }

    bool isRunning() const override {
        return running_;
    }

    size_t maxSignalSize() const override {
        return MAX_SIGNAL_SIZE;
    }

private:
    static const size_t MAX_SIGNAL_SIZE = 10;
    std::array<Packet, MAX_SIGNAL_SIZE> buffer_;
    size_t bufferSize_ = 0;
    size_t currentIndex_ = 0;
    bool running_ = false;

    SquareGenerator() = default; // Private constructor (Singleton)

    void startNextPacket() {
        if (currentIndex_ >= bufferSize_) {
            stop();
            return;
        }

        const auto& packet = buffer_[currentIndex_];

        // Clamp frequency within valid range (100Hz - 20kHz)
        uint16_t freq = std::clamp(packet.freq, 100u, 20000u);

        // Compute ARR for the frequency
        uint16_t ARR = (1000000 / (2 * freq)) - 1;
        uint16_t RCR = packet.n - 1; // RCR = pulses - 1

        __HAL_TIM_SET_AUTORELOAD(&htim1, ARR);
        __HAL_TIM_SET_REPETITIONCOUNTER(&htim1, RCR);

        // Force update to apply changes
        TIM1->EGR |= TIM_EGR_UG;

        HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);
    }

public:
    static void TimerCallback() {
        SquareGenerator& instance = getInstance();
        instance.currentIndex_++;
        instance.startNextPacket();
    }
};

// Redirect TIM1 IRQ to the Singleton instance
extern "C" void TIM1_UP_TIM10_IRQHandler(void) {
    if (__HAL_TIM_GET_IT_SOURCE(&htim1, TIM_IT_UPDATE)) {
        __HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
        SquareGenerator::TimerCallback();
    }
}

#endif