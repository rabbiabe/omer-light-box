file(GLOB_RECURSE FILES src/*.c* src/*.h* lib/*.c* *.h*)

cmake_minimum_required(VERSION 3.22)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
project(omer-counter VERSION 1.0.0)
add_executable(${PROJECT_NAME}
                ${FILES})

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()

include(lib/PicoLED/PicoLed.cmake)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_add_extra_outputs(${PROJECT_NAME})

add_library(pico_ssd1306 INTERFACE)

target_sources(pico_ssd1306 INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/lib/SSD1306/src/ssd1306/SSD1306_OLED.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lib/SSD1306/src/ssd1306/SSD1306_OLED_graphics.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lib/SSD1306/src/ssd1306/SSD1306_OLED_Print.cpp
  ${CMAKE_CURRENT_LIST_DIR}/lib/SSD1306/src/ssd1306/SSD1306_OLED_font.cpp
)

target_include_directories(pico_ssd1306 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(${PROJECT_NAME}
                        pico_stdlib
                        hardware_gpio
                        hardware_i2c
                        hardware_spi
                        hardware_adc
                        hardware_uart
			hardware_rtc
			hardware_pio
			pico_ssd1306
			pico_time
			PicoLed)
