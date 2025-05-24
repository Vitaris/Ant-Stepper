#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "src/stepper_controller.h"
#include "lcd/lcd.h"
#include "button/button.h"

#define __BSD_VISIBLE 1 // Enable BSD visible features
#include <math.h>
#include <string.h>

#define TABLE_HEIGHT 6

// Timers
uint64_t old_cycle_time = 0;
struct repeating_timer servo_timer;
struct repeating_timer LCD_refresh_timer;

// LCD
lcd_t lcd;

// Buttons
button_t F1;
button_t F2;
button_t F3;
button_t F4;

// Steppers
stepper_t *stepper_1;
stepper_t *stepper_2;

bool F1_pressed;
bool F2_pressed;
bool F3_pressed;
bool F4_pressed;
bool some_released;

bool blink_500ms;
bool lcd_refresh;

uint offset;
uint offset_encoder;

uint32_t i = 0;
int32_t speed2 = 0;

void core1_entry() {

    // LCD
    // lcd = lcd_create(2, 3, 4, 5, 6, 7, 8, 16, 2);

    while (1)
    {
        if (lcd_refresh)
        {
            

            // enc_old = enc;
            lcd_refresh = false;
        }
    }
}

bool servo_timer_callback(struct repeating_timer *t) {
    button_compute(F1);
    button_compute(F2);
    button_compute(F3); 
    button_compute(F4);

    if (F1->state_dropped){

    } 
    else if (F2->state_dropped){
        F2_pressed = true;
    }
    else if (F3->state_dropped){
        F3_pressed = true;
    }
    else if (F4->state_dropped){

    } 
    else if (F1->state_raised || F2->state_raised || F3->state_raised || F4->state_raised) {
        some_released = true;
    }
    return true;
}

int32_t generate_sine_wave(uint32_t step, uint32_t period) {
    // Calculate sine wave from -1000 to 1000
    double angle = (2.0 * M_PI * step) / period;
    return (int32_t)(500000.0 * sin(angle));
}

bool LCD_refresh_timer_callback(struct repeating_timer *t) {
    lcd_refresh = true;
    speed2 = generate_sine_wave(i, 5000);
    stepper_update_speed(stepper_1, speed2);
    i++;
    return true;
}

int main() {
    stdio_init_all();

    stepper_1 = stepper_init(18, 0);
    stepper_2 = stepper_init(3, 1);
    
    // stepper_update_speed(stepper_1, 1000);
    // stepper_update_speed(stepper_2, 5000);
    int32_t speed = 1000;

    F1 = create_button(12);
    F2 = create_button(13);
    F3 = create_button(15);
    F4 = create_button(14);
    
    // Timer for servo control
    // add_repeating_timer_ms(-1, servo_timer_callback, NULL, &servo_timer);

    // 100ms LCD refresh timer
    add_repeating_timer_ms(-1, LCD_refresh_timer_callback, NULL, &LCD_refresh_timer);
    
    // Launch core1
    // multicore_launch_core1(core1_entry);

    // Initial wait 
    // busy_wait_ms(500);

    int button = 0;

    while (1)
    {
        button_compute(F1);
        button_compute(F2);
        button_compute(F3); 
        button_compute(F4);

        stepper_compute(stepper_1);
        stepper_compute(stepper_2);

        if (F1->state_dropped) {
            button = 1;
            speed = 1000;
            stepper_update_speed(stepper_1, speed);
        } else if (F2->state_dropped) {
            button = 2;
            stepper_stop(stepper_1);
        } else if (F3->state_dropped) {
            button = 3;
            speed += 1000;
            stepper_update_speed(stepper_1, speed);
        } else if (F4->state_dropped) {
            button = 4;
            speed -= 1000;
            stepper_update_speed(stepper_1, speed);
        } else {
            button = 0;
        }

        // Print your table Bold: \x1b[1m Abc \x1b[0m
        printf("┌──────────────────────────┐\n");
        printf("│ Stepper Motor    0       │\n");
        printf("├────────────┬─────────────┤\n");
        printf("│ Position 1 │ Position 2  │\n");
        printf("│     %3d    │    %3d      │\n", stepper_get_position(stepper_1), stepper_get_position(stepper_2));
        printf("└────────────┴─────────────┘\n");

        
        sleep_ms(500);

        // Move cursor up to overwrite previous table (skip on first run if needed)
        printf("\033[%dA", TABLE_HEIGHT);
        // tight_loop_contents();
    }
}
