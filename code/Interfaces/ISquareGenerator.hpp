#ifndef ISQUAREGENERATOR_HPP
#define ISQUAREGENERATOR_HPP

#include <span>
#include <cstdint>

struct Packet {
    uint8_t n;     
    uint16_t freq;   
};

class ISquareGenerator {
public:
    virtual ~ISquareGenerator() = default;

    virtual bool add(std::span<Packet> signal) = 0; 
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;  
    virtual size_t maxSignalSize() const = 0;  
};

#endif