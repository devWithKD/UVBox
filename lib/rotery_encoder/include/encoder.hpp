#ifndef ENCODER
#define ENCODER

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"
using namespace std;
class Encoder
{
public:
    int _btn;
    int _clk;
    int _dt;
    bool btn_pressed;
    bool long_pressed;
    bool change = false;              // flag for change in encoder val
    bool state, dt_state, last_state; // encoder parameters
    bool enable;
    int val = 0; // encoder value
    // int lastval;
    absolute_time_t time1_ms, time2_ms;
    Encoder();
    ~Encoder();
    void init(int btn, int clk, int dt);
    void knobUI_en();
    void knobUI_dis();

private:
};

extern Encoder encoder;
void encoder_callback(uint pin, uint32_t events);

#endif