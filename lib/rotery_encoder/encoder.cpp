#include "encoder.hpp"

Encoder encoder;
void encoder_callback(uint pin, uint32_t events)
{

    if (pin == encoder._clk)
    {
        encoder.state = gpio_get(encoder._clk);
        encoder.dt_state = gpio_get(encoder._dt);
        if (encoder.enable)
        {
            encoder.change = true;
            if (encoder.state != encoder.last_state)
            {
                if (encoder.dt_state != encoder.state)
                {
                    // encoder.lastval = encoder.val;
                    encoder.val++;
                }
                else
                {
                    // encoder.lastval = encoder.val;
                    encoder.val--;
                }
                encoder.last_state = encoder.state;
            }
        }
    }
    else if (pin == encoder._btn)
    {
        if (events == 0x04)
        {
            encoder.time1_ms = get_absolute_time();
        }
        else if (events == 0x08)
        {
            encoder.time2_ms = get_absolute_time();
            if (absolute_time_diff_us(encoder.time1_ms, encoder.time2_ms) > 1000 * 1000)
            {
                encoder.long_pressed = true;
                encoder.btn_pressed = false;
            }
            else
            {
                encoder.long_pressed = false;
                encoder.btn_pressed = true;
            }
        }
    }
}

Encoder::Encoder()
{
}

Encoder::~Encoder()
{
    this->val = 0;
    this->change = false;
    gpio_set_irq_enabled_with_callback(this->_clk, 0x04 | 0x08, 0, encoder_callback);
    gpio_set_irq_enabled_with_callback(this->_btn, 0x04 | 0x08, 0, encoder_callback);

    // delete(this);
}

void Encoder::knobUI_en()
{
    enable = true;
}

void Encoder::knobUI_dis()
{
    enable = false;
}

void Encoder::init(int btn, int clk, int dt)
{
    this->_btn = btn;
    this->_clk = clk;
    this->_dt = dt;
    //////////////////////////////
    ////// initiate pins /////////
    //////////////////////////////
    gpio_init(this->_btn);
    gpio_set_dir(this->_btn, GPIO_IN);
    gpio_pull_up(this->_btn);

    gpio_init(this->_clk);
    gpio_set_dir(this->_clk, GPIO_IN);
    gpio_pull_up(this->_clk);

    gpio_init(this->_dt);
    gpio_set_dir(this->_dt, GPIO_IN);
    gpio_pull_up(this->_dt);
    ///////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////// initiate ////////////////////////////////////////
    gpio_set_irq_enabled_with_callback(this->_clk, 0x04 | 0x08, 1, encoder_callback);
    gpio_set_irq_enabled_with_callback(this->_btn, 0x04 | 0x08, 1, encoder_callback);
    ///////////////////////////////////////////////////////////////////////////

    change = false;
    btn_pressed = false;
    long_pressed = false;
}
