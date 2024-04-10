/**************************************************
 * Omer Counter v2.1 for Raspberry Pi Pico
 * Written by Abe Friedman 2024-04-08
 * github.com/rabbiabe
 * 
 * readEncoder() adapted from code by Ralph S. Bacon 
 * https://github.com/RalphBacon/226-Better-Rotary-Encoder---no-switch-bounce
 * 
 * Header and MakeFile based on examples from 
 * Peter Zimon https://github.com/peterzimon/pico-toolbox
 *
 **************************************************/

#pragma once


/*
 * C++ HEADERS
 */
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cstdio>

/*
 * PICO HEADERS
 */
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "hardware/rtc.h"
#include "pico/time.h"
#include "pico/types.h"
#include "hardware/pio.h"


/*
 * OTHER LIBRARIES
 */
#include "../lib/PicoLED/PicoLed.hpp"
#include "../lib/SSD1306/include/ssd1306/SSD1306_OLED.hpp"
#include "../lib/SSD1306/fonts/fontAlteHaasGrotsek.h"
#include "../lib/SSD1306/fonts/fontTaiganja.h"
#include "../lib/SSD1306/fonts/fontAndaleMono.h"

#include "omer-obj.h"


/*
 * VARIABLES & DEFINITIONS
 */
#define OLED_W 128
#define OLED_H 64
#define OLED_ADDR 0x3C
#define OLED_I2C_CLOCK_KHZ 400
#define OLED_TIMEOUT_MS (13 * 1000)
#define TIMEOUT_CANCEL_NORESET 0

#define ADC_BRIGHT_REF 0
#define BRIGHTNESS_INVERT true // reverses the brightness reading

#define SHOW_TODAY_COUNT 0 

#define SET_DATE 1
#define SET_TIME_H 2
#define SET_TIME_M 3
#define SET_ALARM_H 4
#define SET_ALARM_M 5

typedef struct PicoLed::Color led_color;

extern const uint8_t   
                pinLED,
                pinSDA,
                pinSCL,
                pinEncAcw,
                pinEncBccw,
                pinEncSW,
                pinBrightness,
                pinVSysRef;

extern volatile bool refreshState, displayActive, onBattery;
extern datetime_t timeNow, timeAlarm;
extern OmerCounter sefirah; 




/*
 * SCREEN TURN-OFF CALLBACK
 */
int64_t callbackScreenTimeout(alarm_id_t id, void *user_data);

/*
 * DISPLAY FUNCTIONS
 */
uint8_t adjustHour12(uint8_t hour24, char* ampm);
void displayCountText(DayCount today);
void displayCountColor(DayCount today);
void displayResetTimeout(uint32_t timeout_ms);
void displaySleep();
void displayStartup();
void displayWake(bool set_timeout);
uint8_t getBrightness();
uint32_t scale(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);
void displayHoldingPattern();
void inline displayRainbow();