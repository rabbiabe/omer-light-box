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
 * Display functions
 **************************************************/

#include "display.h"

const uint8_t led_strip_length = 24;
uint8_t setMode, display_buffer[OLED_W * (OLED_H / 8)];
alarm_id_t screenTimeout = 0;

SSD1306 display(OLED_W, OLED_H);

PicoLed::PicoLedController ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(pio0, 0, pinLED, led_strip_length, PicoLed::FORMAT_GRB);

led_color omerColors[7] = {
    PicoLed::HSV(124,255,255),  // Blue     week 1 / day 1 of each week
    PicoLed::HSV(0,255,255),    // Red      week 2 / day 2 of each week
    PicoLed::HSV(85,255,255),   // Green    week 3 / day 3 of each week
    PicoLed::HSV(198,255,255),  // Purple   week 4 / day 4 of each week
    PicoLed::HSV(21,255,255),   // Orange   week 5 / day 5 of each week
    PicoLed::HSV(43,255,255),   // Yellow   week 6 / day 6 of each week
    PicoLed::HSV(0,0,255),      // White    week 7 / day 7 of each week
};

void displayStartup()
{
    display.OLEDbegin(OLED_ADDR, i2c0, OLED_I2C_CLOCK_KHZ, pinSDA, pinSCL);
    display.OLEDSetBufferPtr(OLED_W, OLED_H, display_buffer, sizeof(display_buffer)/sizeof(uint8_t));
    displayWake(true);
    ledStrip.setBrightness(getBrightness());
    displayRainbow();
}

void displayWake(bool set_timeout)
{
    displayActive = true;
    display.OLEDinit();
    display.OLEDclearBuffer();
    display.fillScreen(0);
    display.OLEDupdate();
    if (set_timeout) {
        displayResetTimeout(OLED_TIMEOUT_MS);
    } else {
        displayResetTimeout(TIMEOUT_CANCEL_NORESET);
    }   
}

void displaySleep()
{
    displayActive = false;
    ledStrip.clear();
    ledStrip.show();
    display.OLEDclearBuffer();
    display.fillScreen(0);
    display.OLEDupdate();
    display.OLEDPowerDown();
}

void displayResetTimeout(uint32_t timeout_ms)
{
    printf("\n\n} displayResetTimeout() -- timeout_ms == %d {\n", timeout_ms);
    if (screenTimeout != 0) {
        printf("====> screenTimeout = %d, cancelling alarm\n", screenTimeout);
        cancel_alarm(screenTimeout);
        screenTimeout = 0;
    }
    if (timeout_ms > 0) {
        printf("====> timeout_ms = %d, setting alarm\n", timeout_ms);
        screenTimeout = add_alarm_in_ms(timeout_ms, callbackScreenTimeout, NULL, true);    
    }
    printf("\n\n\n");
}

uint8_t adjustHour12(uint8_t hour24, char* ampm)
{
    if (hour24 > 23) return 0xFF;

    uint8_t hour12;

    if (hour24 > 11) {
        strcpy(ampm, "PM");
    } else {
        strcpy(ampm, "AM");
    }

    if (hour24 > 12) {
        hour12 = hour24 - 12;
    } else {
        hour12 = hour24;
        if (hour12 == 0) hour12 = 12;
    }

    return hour12;
}

void displayCountColor(DayCount today)
{
    if (onBattery) {
        ledStrip.clear();
        ledStrip.show();
        return;
    }

    uint8_t colorWeek, colorDay;
    if (today.days == 0) {
        colorWeek = today.weeks - 1;
        colorDay = 6;
    } else {
        colorWeek = today.weeks;
        colorDay = today.days - 1;
    }
    ledStrip.fill(omerColors[colorWeek]);
    ledStrip.fill(omerColors[colorDay], 0, 1);
    ledStrip.fill(omerColors[colorDay], 23, 1);
    ledStrip.fillGradient(omerColors[colorDay], omerColors[colorWeek], 19, 4);
    ledStrip.fillGradient(omerColors[colorWeek], omerColors[colorDay], 1,4);
    ledStrip.show();
}

uint8_t getBrightness()
{
    adc_select_input(ADC_BRIGHT_REF);
    uint16_t reading = BRIGHTNESS_INVERT ? (4095 - adc_read()) : adc_read();
    return scale(reading, 0, 4095, 40, 255);
}

void displayCountText(DayCount today)
{
    char heading[6], month[6], tagLeft[4], tagRight[5], time_ampm[5], alarm_ampm[5], timeTemp[3];
    HebrewDate thisDate = sefirah.getHebrewDate();
    uint8_t time12, alarm12;

    bool setHighlight[6], countingOmer = ((today.total_count > 0) && (today.total_count < 50)) ? true : false;
    
    for (uint8_t h = 0; h < 6; h++) setHighlight[h] = (setMode == h);

    display.OLEDclearBuffer();

    if (setMode == SHOW_TODAY_COUNT) {
        /**************************************************
         * These display instructions apply when the screen
         * is awakened, either by user (pressing encoder)
         * or by the RTC alarm.
         **************************************************/
        if (countingOmer) {

            sprintf(heading, "COUNT");
            if (today.weeks > 0) {
                sprintf(tagLeft, "%dWk", today.weeks);
                if (today.days > 0) sprintf(tagRight, "%dDay", today.days);
            } else {
                sprintf(tagLeft, "   ");
                sprintf(tagRight, "    ");
            }
            
            /*  PRINT HEADING  */
            display.fillRect(0, 0, 128, 16, true);
            display.setCursor(29,0);
            display.setFont(fontAlteHaasGrotsek16pxB);
            display.setInvertFont(true);
            display.print(heading);

            /*  PRINT TODAY'S COUNT  */
            display.setCursor(36,16);
            display.setFont(fontTaiganja32px);
            display.setInvertFont(false);
            sprintf(timeTemp, "%2d", today.total_count);
            display.print(timeTemp);

            display.setFont(fontTaiganja16px);

            /*  PRINT WEEK TALLY  */
            if (today.weeks > 0) {
                display.setCursor(1,48);
                display.print(tagLeft);
            }

            /*  PRINT DAY TALLY  */
            display.setCursor(71,48);
            display.print(tagRight);
        
        } else {
            displayHoldingPattern();
        }
    } else {
        /**************************************************
         * These display instructions apply when setting
         * the clock.
         **************************************************/
        time12 = adjustHour12(timeNow.hour, time_ampm);
        alarm12 = adjustHour12(timeAlarm.hour, alarm_ampm);

        sprintf(heading, "SET");
        
        /*  PRINT HEADING  */
        display.fillRect(0, 0, 128, 16, true);
        display.setCursor(43,0);
        display.setFont(fontAlteHaasGrotsek16pxB);
        display.setInvertFont(true);
        display.print(heading);

        /*  PRINT DATA LABELS  */
        display.setInvertFont(false);
        display.setCursor(0,16);
        display.print("D");
        display.setCursor(0,32);
        display.print("T");
        display.setCursor(0,48);
        display.print("N");

        display.setFont(fontAlteHaasGrotsek16px);

        /*  PRINT DATE  */
        if (setHighlight[SET_DATE]) {
            display.setInvertFont(true);
            display.fillRect(22, 16, 106, 16, 1);
        }
        display.setCursor(22,16);
        sprintf(timeTemp, "%2d", thisDate.date);
        display.print(timeTemp);
        display.setCursor(58,16);
        display.print(thisDate.month);

        /*  PRINT TIME  */
        display.setInvertFont(setHighlight[SET_TIME_H]);
        display.setCursor(30,32);
        sprintf(timeTemp, "%2d", time12);
        display.print(timeTemp);
        display.setCursor(58,32);
        display.setInvertFont(false);
        display.print(":");
        display.setInvertFont(setHighlight[SET_TIME_M]);
        display.setCursor(72,32);
        sprintf(timeTemp, "%02d", timeNow.min);
        display.print(timeTemp);
        display.setInvertFont(false);
        display.setCursor(100,32);
        display.print(time_ampm);

        /*  PRINT NIGHTFALL ALARM  */
        display.setInvertFont(setHighlight[SET_ALARM_H]);
        display.setCursor(30,48);
        sprintf(timeTemp, "%2d", alarm12);
        display.print(timeTemp);
        display.setCursor(58,48);
        display.setInvertFont(false);
        display.print(":");
        display.setInvertFont(setHighlight[SET_ALARM_M]);
        display.setCursor(72,48);
        sprintf(timeTemp, "%02d", timeAlarm.min);
        display.print(timeTemp);
        display.setInvertFont(false);
        display.setCursor(100,48);
        display.print(alarm_ampm);
    }   

    if (countingOmer) displayCountColor(today);
    display.OLEDupdate();
}

void displayHoldingPattern()
{
    static uint8_t filledSquare = 1;
    for (uint8_t row = 0; row < 7; row++) {
        for (uint8_t col = 0; col < 7; col++) {
            if (((col+1) + (row * 7)) == filledSquare) {
                display.fillRect((44 + (col * 6)), (20 + (row * 6)), 5, 5, 1);
            } else {
                display.drawRect((44 + (col * 6)), (20 + (row * 6)), 5, 5, 1);
            }
        }
    }
    filledSquare++;
    if (filledSquare > 49) filledSquare = 1;
    displayRainbow();
}

void inline displayRainbow()
{
    ledStrip.fillRainbow(0, 255/24);
    ledStrip.show();
}