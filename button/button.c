#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <stdlib.h>

#include "button.h"

button_t create_button(uint8_t gpio_pin_num)
{
    // Create button data structure
    button_t button = (button_t)malloc(sizeof(struct button));

    // Set button GPIO to be an input
    gpio_init(gpio_pin_num);
    gpio_set_dir(gpio_pin_num, GPIO_IN);
    gpio_pull_up(gpio_pin_num);

    button->gpio_pin_num = gpio_pin_num;

    return button;
}

void button_compute(button_t button)
{
    // Read the button state
    bool current_state = gpio_get(button->gpio_pin_num);
    int currenttime = time_us_64();

    // Positive change detection (off -> on)
    if (current_state != button->state && button->state == false)
    {
        // Debounce
        if (time_us_64() - button->time_pressed > BUTTON_DEBOUNCE_TIME)
        {
            button->state_raised = true;
            button->time_pressed = time_us_64();
        }
    }
    else 
    {
        button->state_raised = false;
    }

    // Negative change detection (on -> off)
    if (current_state != button->state && button->state == true)
    {
        // Debounce
        if (time_us_64() - button->time_released > BUTTON_DEBOUNCE_TIME)
        {
            button->state_dropped = true;
            button->time_released = time_us_64();
        }
    }
    else
    {
        button->state_dropped = false;
    }

    button->state = current_state;
}

// Change detection shold be somehow generalized
bool button_change_detection(button_t button, bool current_button_state, int current_time)
{
    if (current_button_state != button->state && button->state == true)
    {
        // Debounce
        if (current_time - button->time_released > BUTTON_DEBOUNCE_TIME)
        {
            button->state_dropped = true;
            button->time_released = current_time;
        }
    }
    else
    {
        return false;
    }
}
