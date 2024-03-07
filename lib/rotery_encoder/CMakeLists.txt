set(LIB_NAME encoder)

add_library(${LIB_NAME} encoder.cpp)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(${LIB_NAME} pico_stdlib hardware_gpio pico_time)
