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


#include <string>
#include <cstdint>



/*
 * OMER COUNTING STRUCTS / CLASSES
 */
typedef struct HebrewDate {
    std::string month;
    uint8_t date;
} HebrewDate;

typedef struct DayCount {
    int8_t total_count;
    int8_t weeks;
    int8_t days;
} DayCount;

class OmerCounter {
    public:
        OmerCounter();
        DayCount count();
        void stepCounter(int8_t step);
        HebrewDate getHebrewDate();

    private:
        int8_t hayomyom = -2;
        const int8_t Rosh_Hodesh_Nisan = -14, Rosh_Hodesh_Iyyar_Day1 = 15, Rosh_Hodesh_Sivan = 45, Shavuot = 50;
};