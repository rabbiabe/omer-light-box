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

#include "omer-counter.h"

int main()
{
    stdio_init_all();
    sleep_ms(1000);

    setup();

    while (true) {

        if (encoderTurn != 0) {
            printf("<< encoder turned %+d >>\n", encoderTurn);
            switch (setMode)
            {
            case SET_DATE:
                sefirah.stepCounter(encoderTurn);
                break;
            
            case SET_TIME_H:
                timeNow.hour += encoderTurn;
                if (timeNow.hour < 0) timeNow.hour = 23;
                if (timeNow.hour > 23) timeNow.hour = 0;
                break;
            
            case SET_TIME_M:
                timeNow.min += encoderTurn;
                if (timeNow.min < 0) timeNow.min = 59;
                if (timeNow.min > 59) timeNow.min = 0;
                break;
            
            case SET_ALARM_H:
                timeAlarm.hour += encoderTurn;
                if (timeAlarm.hour < 0) timeAlarm.hour = 23;
                if (timeAlarm.hour > 23) timeAlarm.hour = 0;
                break;
            
            case SET_ALARM_M:
                timeAlarm.min += encoderTurn;
                if (timeAlarm.min < 0) timeAlarm.min = 59;
                if (timeAlarm.min > 59) timeAlarm.min = 0;
                break;
            
            case SHOW_TODAY_COUNT:
            default:
                break;
            }
        encoderTurn = 0;
        }

        if (longPress) {
            longPress = false;
            printf("( long press )\n");
            if (setMode == SHOW_TODAY_COUNT) {
                setMode = SET_DATE;
                displayResetTimeout(TIMEOUT_CANCEL_NORESET);
                rtc_get_datetime(&timeNow);
                if (!displayActive) {
                    displayWake(false);
                }
            } else {
                updateTimeAlarm();
                setMode = SHOW_TODAY_COUNT;
                displayResetTimeout(OLED_TIMEOUT_MS);
            }
        }

        if (shortAction) {
            shortAction = false;
            printf("[ short press ]\n");
            if (setMode == SHOW_TODAY_COUNT) {
                if (!displayActive) {
                    displayWake(true);
                } else {
                    displayResetTimeout(OLED_TIMEOUT_MS);
                }
            } else {
                setMode++;
                if (setMode > SET_ALARM_M) setMode = SET_DATE;
            }
        }
    }

    return 0;
}

void setup()
{
    gpio_init_mask(maskInit);
    gpio_set_dir_out_masked(maskOutput);
    gpio_set_dir_in_masked(maskInput);

    gpio_pull_up(pinEncAcw);
    gpio_pull_up(pinEncBccw);
    gpio_set_pulls(pinEncSW, false, false);

    adc_init();
    adc_gpio_init(pinBrightness);
    adc_gpio_init(pinVSysRef);

    gpio_set_irq_enabled(pinEncAcw, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(pinEncBccw, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled_with_callback(pinEncSW, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &callbackEncoder);

    add_repeating_timer_ms(REFRESH_TIME_MS, &callbackRefresh, NULL, &timerRefresh);
    add_repeating_timer_ms(BRIGHTNESS_TIME_MS, &callbackBright, NULL, &timerBrightness);
    add_repeating_timer_ms(BATT_CHK_TIME_MS, &callbackBatt, NULL, &timerCheckBatt);

    rtc_init();
    updateTimeAlarm();

    displayStartup();
}

void callbackEncoder(uint gpio, uint32_t event) 
{
    printf("{ %d Encoder gpio = %d, event = %d }\n", time_us_32(), gpio, event);
    switch (gpio)
    {
    case pinEncSW:
        if (event == GPIO_IRQ_EDGE_RISE) {
            uint32_t timeCall = time_us_32();
            uint32_t timeDiff = timeCall - encoderPress;
            if ((timeDiff > LONG_HOLD_TIME_US) && (encoderPress > 0)) {
                printf("LONG:\ntimeCall %u\nPress  %u\n\n====> timeDiff %u\n\n\n", timeCall, encoderPress, timeDiff);
                longPress = true;
            } else {                                                  
                printf("SHORT:\ntimeCall %u\nPress  %u\n\n====> timeDiff %u\n\n\n", timeCall, encoderPress, timeDiff);
                shortAction = true;                                   
            }
            encoderPress = 0;                                          
        } else {
            encoderPress = time_us_32();   
            //if (setMode == SHOW_TODAY_COUNT) displayResetTimeout(OLED_TIMEOUT_MS);                             
        }
        break;
    
    case pinEncAcw:
    case pinEncBccw:
        encoderTurn = readEncoder();
        break;

    default:
        break;
    }
}

void inline updateTimeAlarm() 
{
    rtc_set_datetime(&timeNow);
    busy_wait_us_32(RTC_SETUP_TIME_US);
    rtc_set_alarm(&timeAlarm, &callbackAlarm);
    busy_wait_us_32(RTC_SETUP_TIME_US);
}

void callbackAlarm()
{
    setMode = SHOW_TODAY_COUNT;
    sefirah.stepCounter(1);
    DayCount nowCount = sefirah.count();
    if (nowCount.total_count < 1) {
        displayWake(true);
    } else {
        displayWake(false);
        displayResetTimeout(OMER_COUNTER_SLEEP_MS);
    }
}

bool callbackRefresh(repeating_timer_t *rt)
{
    if (displayActive) displayCountText(sefirah.count());
    gpio_put(PICO_DEFAULT_LED_PIN, displayActive);
    return true;
}

bool callbackBatt(repeating_timer_t *rt)
{
    adc_select_input(ADC_VSYS_REF);
    onBattery = (adc_read() < BATT_BACKUP);
    printf("\n\n\n===> Check Battery Level\n  adc_read() = %d\n  onBattery = %d\n\n", adc_read(), onBattery);
    return true;
}

bool callbackBright(repeating_timer_t *rt)
{
    ledStrip.setBrightness(getBrightness());
    ledStrip.show();
    return true;
}

int64_t callbackScreenTimeout(alarm_id_t id, void *user_data)
{
    printf("\n\n[[ callbackScreenTimeout() ]]\n\n");
    screenTimeout = 0;
    displaySleep();    
    return 0;
}

int8_t readEncoder()
{
    static uint8_t lrmem = 3;
    static int lrsum = 0;
    static int8_t TRANS[] = {0, -1, 1, 14, 1, 0, 14, -1, -1, 14, 0, 1, 14, 1, -1, 0};

    // Read BOTH pin states to deterimine validity of rotation (ie not just switch bounce)
    int8_t l = gpio_get(pinEncAcw);
    int8_t r = gpio_get(pinEncBccw);

    // Move previous value 2 bits to the left and add in our new values
    lrmem = ((lrmem & 0x03) << 2) + (2 * l) + r;

    // Convert the bit pattern to a movement indicator (14 = impossible, ie switch bounce)
    lrsum += TRANS[lrmem];

    /* encoder not in the neutral (detent) state */
    if (lrsum % 4 != 0)
    {
        return 0;
    }

    /* encoder in the neutral state - clockwise rotation*/
    if (lrsum == 4)
    {
        lrsum = 0;
        return 1;
    }

    /* encoder in the neutral state - anti-clockwise rotation*/
    if (lrsum == -4)
    {
        lrsum = 0;
        return -1;
    }

    // An impossible rotation has been detected - ignore the movement
    lrsum = 0;
    return 0;
}

uint32_t scale(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}