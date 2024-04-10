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

/*
 * PROJECT LIBRARIES
 */
#include "display.h"
#include "omer-obj.h"

/*
 * PINS
 */
const uint8_t   pinSDA = 4,
                pinSCL = 5,
                pinLED = 10,
                pinEncAcw = 11,
                pinEncBccw = 12,
                pinEncSW = 13,
                pinBrightness = 26,
                pinVSysRef = 29;

const uint32_t maskInit =   (1 << PICO_DEFAULT_LED_PIN) |
                            (1 << pinEncAcw)  |
                            (1 << pinEncBccw) |
                            (1 << pinEncSW);

const uint32_t maskInput =  (1 << pinEncAcw)  |
                            (1 << pinEncBccw) |
                            (1 << pinEncSW);

const uint32_t maskOutput = (1 << PICO_DEFAULT_LED_PIN);

/*
 * MACROS AND VARIABLES
 */

#define ADC_VSYS_REF 3
#define BATT_BACKUP 1655                            // ADC channel 3 reads VSYS/3, with VREF still at 3.3V; 1655 = 1.333V which indicates VSYS = 4V, battery backup instead of 5VDC input
#define BATT_CHK_TIME_MS (-60 * 1000)               // 1 minute, negative value to have evenly-spaced callbacks
#define REFRESH_TIME_MS -333                        // 333ms, negative value to have evenly-spaced callbacks
#define BRIGHTNESS_TIME_MS -100                     // 100ms, negative value to have evenly-spaced callbacks

#define RTC_SETUP_TIME_US 70                        // 70µs

#define LONG_HOLD_TIME_US (3 * 1000 * 1000)         // 3 seconds

#define OMER_COUNTER_SLEEP_MS (5 * 60 * 60 * 1000)  // 5 hours

#define COUNT_NEW_DAY 1

extern OmerCounter sefirah;
extern PicoLed::PicoLedController ledStrip;
extern PicoLed::Color omerColors[];
extern uint8_t setMode;
extern alarm_id_t screenTimeout;

repeating_timer_t timerRefresh, timerCheckBatt, timerBrightness;

datetime_t  
    timeNow = {
        .year = 2024,   //don't care
        .month = 1,     //don't care
        .day = 1,       //don't care
        .dotw = 0,      //don't care
        .hour = 12,
        .min = 00,
        .sec = 00       //don't care
    }, 

    timeAlarm = {
        .year = -1,     //don't care
        .month = -1,    //don't care
        .day = -1,      //don't care
        .dotw = -1,     //don't care
        .hour = 19,
        .min = 00,
        .sec = 00       //this value stays fixed, no user input
    };

volatile bool   refreshState = false,
                longPress = false,
                displayActive = true,
                onBattery = false,
                shortAction = false;

volatile uint32_t encoderPress = 0;
volatile int8_t encoderTurn = 0;

/*
 * CALLBACKS AND INTERRUPTS
 */
void callbackAlarm();
bool callbackBatt(repeating_timer_t *rt);
bool callbackBright(repeating_timer_t *rt);
void callbackEncoder(uint gpio, uint32_t event) ;
bool callbackRefresh(repeating_timer_t *rt);
int64_t callbackScreenTimeout(alarm_id_t id, void *user_data);

/*
 * OTHER FUNCTIONS
 */
int8_t readEncoder();
uint32_t scale(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);
void setup();
void inline updateTimeAlarm(); 
