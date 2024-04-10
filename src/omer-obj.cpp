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
 **************************************************
 * OmerCounter Class functions
 **************************************************/

#include "omer-obj.h"

OmerCounter sefirah;

OmerCounter::OmerCounter()
{

}

DayCount OmerCounter::count()
{
    DayCount returnData;
    returnData.total_count = hayomyom;
    returnData.weeks = hayomyom / 7;
    returnData.days = hayomyom % 7;
    return returnData;
}

void OmerCounter::stepCounter(int8_t step)
{
    hayomyom += step;
    if (hayomyom == Shavuot) hayomyom = 49;
    if (hayomyom < Rosh_Hodesh_Nisan) hayomyom = Rosh_Hodesh_Nisan;
}

HebrewDate OmerCounter::getHebrewDate()
{
    HebrewDate returnData;
    int8_t dateOffset = (hayomyom >= Rosh_Hodesh_Sivan) ? -44 : (hayomyom > Rosh_Hodesh_Iyyar_Day1) ? -15 : 15;
    returnData.date = hayomyom + dateOffset; 
   
    if ((hayomyom < Shavuot) && (hayomyom >= Rosh_Hodesh_Nisan)) {
        if (hayomyom > Rosh_Hodesh_Iyyar_Day1) {
            if (hayomyom >= Rosh_Hodesh_Sivan) {
                returnData.month = "SIVAN";
            } else returnData.month = "IYYAR";
        } else {
            returnData.month = "NISAN";
        }
    } else {
        returnData.month = "Error!"; //this shouldn't be possible
    }
    return returnData;
}

