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
struct repeating_timer servo_timer;
struct repeating_timer LCD_refresh_timer;

// LCD
lcd_t lcd;
bool lcd_refresh = false;

// Buttons
button_t F1;
button_t F2;
button_t F3;
button_t F4;

// Steppers
stepper_t *stepper_1;
stepper_t *stepper_2;

void core1_entry() {

    // LCD
    // lcd = lcd_create(2, 3, 4, 5, 6, 7, 8, 16, 2);

    while (1)
    {
        // Wait for LCD refresh signal
        if (lcd_refresh) {
            lcd_refresh = false;

            // Print your table Bold: \x1b[1m Abc \x1b[0m
            printf("┌──────────────────────────┐\n");
            printf("│ Stepper Motor    0       │\n");
            printf("├────────────┬─────────────┤\n");
            printf("│ Position 1 │ Position 2  │\n");
            printf("│     %.2f    │    %.2f      │\n", stepper_get_position(stepper_1), stepper_get_position(stepper_2));
            printf("└────────────┴─────────────┘\n");

            // Move cursor up to overwrite previous table (skip on first run if needed)
            printf("\033[%dA", TABLE_HEIGHT);
        }
    }
}

bool servo_timer_callback(struct repeating_timer *t) {
    button_compute(F1);
    button_compute(F2);
    button_compute(F3); 
    button_compute(F4);

    stepper_compute(stepper_1);
    stepper_compute(stepper_2);

    if (F1->state_dropped){
        stepper_change_acc(stepper_1, 10.0);
        stepper_goto(stepper_1, stepper_get_position(stepper_1) + 1.0, 1.0);
    } 
    else if (F2->state_dropped){
        stepper_change_acc(stepper_1, 50.0);
        stepper_goto(stepper_1, stepper_get_position(stepper_1) - 1.0, 1.0);
    }
    else if (F3->state_dropped){
        stepper_change_acc(stepper_1, 800.0);
        stepper_goto(stepper_1, stepper_get_position(stepper_1) + 10.0, 20.0);
    }
    else if (F4->state_dropped){
        stepper_change_acc(stepper_1, 1.0);
        stepper_goto(stepper_1, stepper_get_position(stepper_1) + 100.0, 10.0);
    } 
    
    return true;
}

bool LCD_refresh_timer_callback(struct repeating_timer *t) {
    lcd_refresh = true;
    return true;
}

int main() {
    stdio_init_all();

    stepper_1 = stepper_init(18, 0, 200.0, 250.0, 100.0, 50.0);
    stepper_2 = stepper_init(3, 1, 200.0, 250.0, 100.0, 50.0);

    F1 = create_button(12);
    F2 = create_button(13);
    F3 = create_button(15);
    F4 = create_button(14);
    
    // Timer for servo control
    add_repeating_timer_ms(-1, servo_timer_callback, NULL, &servo_timer);

    // 100ms LCD refresh timer
    add_repeating_timer_ms(-200, LCD_refresh_timer_callback, NULL, &LCD_refresh_timer);
    
    // Launch core1
    multicore_launch_core1(core1_entry);

    // Initial wait 
    busy_wait_ms(500);


    while (1)
    {
        tight_loop_contents();
    }
}
