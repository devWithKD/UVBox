add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(
  ${PROJECT_NAME}
  pico_stdlib
  pico_binary_info
  hardware_pwm
  hardware_i2c
  encoder
  picoOled)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
