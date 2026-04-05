#pragma once
#include <stdint.h>
#include <string>

struct StateEngineControl
{
    uint8_t Mode;
    float Position;
    float Velocity; 
    uint32_t Limits[3];
};

struct StateRotaryControl
{
  StateEngineControl Engine1;
  StateEngineControl Engine2;
};

