#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "encoder.hpp"
extern "C"
{
#include "picoOled.h"
}

#define encoderBtn 15
#define encoderClk 13
#define encoderDt 14
#define oled_scl 1
#define oled_sda 0
#define LED_PIN 22

t_OledParams oLED;

uint led_slice = 0;
uint led_chan = 0;
uint8_t selected_menu = 0;
uint8_t intensity = 20; // default intentsity 20%
uint16_t duration = 30; // default duration 30 second
const uint16_t max_duration = 1200;
int pwm_duty_cycle = 0;
uint8_t duration_minute = duration / 60;
uint8_t duration_second = duration % 60; 
bool timerRanOut = false;

typedef enum
{
  START_OP,
  STOP_OP
} operation;

void uv_box_init();
void state_machine();
void set_intensity();
void set_duration();
void start_exposure();
void print_menu(int operation);
float map_range(float value, float in_min, float in_max, float out_min, float out_max);

int main()
{
  stdio_init_all();
  uv_box_init();
  oledDraw_string(&oLED, 15, 10, "UV Exposure Unit", NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 24, 22, "DEPARTMENT OF", NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 21, 33, "MECHANICAL ENG", NORMAL_SIZE, WHITE);
  oledDisplay(&oLED);
  sleep_ms(3000);
  oledClear(&oLED, BLACK);
  oledDraw_string(&oLED, 27, 22, "Developed By", NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 27, 34, "Mandar Joshi", NORMAL_SIZE, WHITE);
  oledDisplay(&oLED);
  sleep_ms(3000);
  oledClear(&oLED, BLACK);
  state_machine();
}

void uv_box_init()
{
  gpio_set_function(LED_PIN, GPIO_FUNC_PWM);
  led_slice = pwm_gpio_to_slice_num(LED_PIN);
  led_chan = pwm_gpio_to_channel(LED_PIN);
  pwm_set_wrap(led_slice, 1024);
  pwm_set_chan_level(led_slice, led_chan, 0);
  pwm_set_enabled(led_slice, true);

  encoder.init(encoderBtn, encoderClk, encoderDt);
  encoder.knobUI_en();

  configOled(&oLED, i2c0, 0x3c, oled_sda, oled_scl, CTRL_SH1106, H_64, W_132);
}

void state_machine()
{
  encoder.long_pressed = false;
  encoder.val = 0;
  encoder.btn_pressed = false;
  while (true)
  {
    oledClear(&oLED, BLACK);
    selected_menu = encoder.val;
    print_menu(START_OP);
    switch (selected_menu)
    {
    case 0: // Set Brightness
      /* code */
      oledDraw_string(&oLED, 108, 16, "(X)", NORMAL_SIZE, WHITE);
      if (encoder.btn_pressed)
        set_intensity();
      break;
    case 1: // Set Duration
      /* code */
      oledDraw_string(&oLED, 108, 28, "(X)", NORMAL_SIZE, WHITE);
      if (encoder.btn_pressed)
        set_duration();
      break;
    case 2: // Start UV exposure
      /* code */
      oledDraw_string(&oLED, 108, 40, "(X)", NORMAL_SIZE, WHITE);
      if (encoder.btn_pressed)
        start_exposure();
      break;
    default:
      break;
    }
    oledDisplay(&oLED);
    if (encoder.val > 2)
      encoder.val = 2;
    if (encoder.val < 0)
      encoder.val = 0;
  }
}

void set_intensity()
{
  encoder.btn_pressed = false;
  encoder.val = intensity;
  while (!encoder.btn_pressed)
  {
    intensity = encoder.val;
    oledClear(&oLED, BLACK);
    print_menu(START_OP);
    oledDraw_string(&oLED, 108, 16, "(X)", NORMAL_SIZE, WHITE);
    oledDisplay(&oLED);
    if (encoder.val < 0)
      encoder.val = 0;
    if (encoder.val > 100)
      encoder.val = 100;
  }
  encoder.val = 0;
  encoder.btn_pressed = false;
}

void set_duration()
{
  encoder.btn_pressed = false;
  encoder.val = duration / 5;
  while (!encoder.btn_pressed)
  {
    if (encoder.long_pressed)
    {
      encoder.long_pressed = false;
      encoder.val = 6;
    }
    duration = encoder.val * 5;
    oledClear(&oLED, BLACK);
    print_menu(START_OP);
    oledDraw_string(&oLED, 108, 28, "(X)", NORMAL_SIZE, WHITE);
    oledDisplay(&oLED);
    if (encoder.val < 0)
      encoder.val = 0;
    if (encoder.val > 240)
      encoder.val = 240;
  }
  encoder.val = 1;
  encoder.btn_pressed = false;
}

void start_exposure()
{
  encoder.btn_pressed = false;
  timerRanOut = false;
  encoder.knobUI_dis();
  absolute_time_t start_time = get_absolute_time();
  absolute_time_t end_time = delayed_by_ms(start_time, duration * 1000);
  absolute_time_t current_time;
  pwm_duty_cycle = map_range(intensity, 0, 100, 0, 1023);
  while (!encoder.btn_pressed && !timerRanOut)
  {
    current_time = get_absolute_time();
    int diff = absolute_time_diff_us(current_time, end_time) / 1000;
    int initial_diff = absolute_time_diff_us(start_time, end_time) / 1000;
    if (diff <= 1)
    {
      timerRanOut = true;
    }
    int width = map_range(diff, initial_diff, 0, 3, 96);
    int completion = map_range(diff, initial_diff, 0, 0, 100);
    char msg[10];
    sprintf(msg, "%d", completion);
    strcat(msg, "%");
    // printf("width: %d, diff: %d, pwm: %d\n", width, diff, pwm_duty_cycle);

    oledClear(&oLED, BLACK);
    print_menu(STOP_OP);
    oledDraw_string(&oLED, 108, 40, "(X)", NORMAL_SIZE, WHITE);

    oledDraw_rectangle(&oLED, 0, 52, 98, 62, HOLLOW, WHITE);
    oledDraw_rectangle(&oLED, 2, 54, width, 60, SOLID, WHITE);

    oledDraw_string(&oLED, 104, 53, msg, NORMAL_SIZE, WHITE);
    oledDisplay(&oLED);
    pwm_set_gpio_level(LED_PIN, pwm_duty_cycle);
  }
  pwm_set_gpio_level(LED_PIN, 0);
  encoder.btn_pressed = false;
  timerRanOut = false;
  while (!encoder.btn_pressed)
  {
    oledClear(&oLED, BLACK);
    print_menu(STOP_OP);
    oledDraw_string(&oLED, 108, 40, "(X)", NORMAL_SIZE, WHITE);

    oledDraw_rectangle(&oLED, 0, 52, 98, 62, HOLLOW, WHITE);
    oledDraw_rectangle(&oLED, 2, 54, 96, 60, SOLID, WHITE);

    oledDraw_string(&oLED, 104, 53, "100%", NORMAL_SIZE, WHITE);
    oledDisplay(&oLED);
  }
  encoder.btn_pressed = false;
  encoder.knobUI_en();
  encoder.val = 2;
}

void print_menu(int operation)
{
  char msg[10];
  duration_minute = duration / 60;
  duration_second = duration % 60;
  oledClear(&oLED, BLACK);
  oledDraw_string(&oLED, 39, 5, "Welcome!", NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 0, 16, "Intensity:", NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 108, 16, "( )", NORMAL_SIZE, WHITE);
  sprintf(msg, "%d", intensity);
  strcat(msg, "%");
  oledDraw_string(&oLED, 72, 16, msg, NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 0, 28, "Duration:", NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 108, 28, "( )", NORMAL_SIZE, WHITE);
  sprintf(msg, "%0.2d:%0.2d", duration_minute, duration_second);
  oledDraw_string(&oLED, 64, 28, msg, NORMAL_SIZE, WHITE);
  if (operation == START_OP)
    oledDraw_string(&oLED, 0, 40, "Start Exposure", NORMAL_SIZE, WHITE);
  else
    oledDraw_string(&oLED, 0, 40, "Stop Exposure", NORMAL_SIZE, WHITE);
  oledDraw_string(&oLED, 108, 40, "( )", NORMAL_SIZE, WHITE);
}

float map_range(float value, float in_min, float in_max, float out_min, float out_max)
{
  // Check for zero range in input or output
  if (in_max - in_min == 0 || out_max - out_min == 0)
  {
    return -1.0; // Indicate error (e.g., division by zero)
  }

  // Normalize the input value to a 0-1 range
  float in_range = in_max - in_min;
  float normalized_value = (value - in_min) / in_range;

  // Scale and translate the normalized value to the output range
  float out_range = out_max - out_min;
  float mapped_value = out_min + (normalized_value * out_range);

  return mapped_value;
}