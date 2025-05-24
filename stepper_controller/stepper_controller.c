#include "pico/stdlib.h"
#include <stdio.h>
#include <stdlib.h>  
#include "stepper_controller.h"
#include "hardware/pio.h"
#include "stepdir_counter.pio.h"
#include "blink.pio.h"  

void update_blink(stepper_t* stepper, uint32_t freq, bool direction);

static bool pio_initialized = false;
static uint blink_offset = 0; 
static uint offset_encoder = 0;

static pio_hw_t* blink_pio = pio0;
static pio_hw_t* encoder_pio = pio1;

struct stepper {
    bool steppping_is_active;
    bool enable;
    uint8_t sm;
    uint8_t pin;
    uint32_t step;
    uint32_t position;
};

stepper_t* stepper_init(const uint8_t pin, const uint8_t sm) {
    stepper_t* stepper = calloc(1, sizeof(struct stepper));
    stepper->sm = sm;
    stepper->pin = pin;

    if (!pio_initialized) {
        pio_clear_instruction_memory(blink_pio);     // Clear pio0
        pio_clear_instruction_memory(encoder_pio);    // Clear pio1

        blink_offset = pio_add_program(blink_pio, &blink_program);
        offset_encoder = pio_add_program(encoder_pio, &stepdir_counter_program);
        pio_initialized = true;
    }

    // Step Generator
    blink_program_init(blink_pio, stepper->sm, blink_offset, stepper->pin);

    // Stepper Counter
    stepdir_counter_program_init(encoder_pio, stepper->sm, offset_encoder, stepper->pin, 0);

    // Set the Direction pin
    gpio_init(stepper->pin + 1);
    gpio_set_dir(stepper->pin + 1, GPIO_OUT);

    return stepper;
}

void stepper_compute(stepper_t* const stepper) {
    stepper->position = stepdir_counter_get_count(encoder_pio, stepper->sm);
}

bool stepper_is_active(stepper_t* stepper) {
    return stepper->enable;
}

void stepper_stop(stepper_t* stepper) {
    pio_sm_set_enabled(blink_pio, stepper->sm, false);
    pio_sm_restart(blink_pio, stepper->sm);
    stepper->enable = false;
}

void stepper_update_speed(stepper_t* stepper, int32_t speed) {
    // Use abs() for proper signed to unsigned conversion
    uint32_t abs_speed = (speed >= 0) ? (uint32_t)speed : (uint32_t)(-speed);
    bool direction = (speed >= 0);
    
    update_blink(stepper, abs_speed, direction);
}

void debug_tx_fifo(PIO pio, uint sm) {
    // Get number of entries in TX FIFO
    uint fifo_level = pio_sm_get_tx_fifo_level(pio, sm);
    printf("TX FIFO level: %d\n", fifo_level);
    
    // Read FIFO contents without removing values
    for (int i = 0; i < fifo_level; i++) {
        uint32_t value = pio->txf[sm];
        printf("FIFO[%d]: 0x%08x\n", i, value);
    }
}

void update_blink(stepper_t* stepper, uint32_t freq, bool direction) {
    if (freq <= 1.5) {
        stepper_stop(stepper);
        return;
    }
    
    uint32_t delay = (clock_get_hz(clk_sys) / (2 * freq)) - 3;

    if (stepper->enable) {
        
        // Load the new delay value into the FIFO
        pio_sm_clear_fifos(blink_pio, stepper->sm);
        pio_sm_exec(blink_pio, stepper->sm, pio_encode_out(pio_null, 32));  // Clear OSR
        pio_sm_exec(blink_pio, stepper->sm, pio_encode_in(pio_null, 32)); 
        pio_sm_put_blocking(blink_pio, stepper->sm, delay);
    }
    else {
        pio_sm_put_blocking(blink_pio, stepper->sm, delay);
        // pio_sm_exec(blink_pio, stepper->sm, pio_encode_pull(false, false));
        // pio_sm_exec(blink_pio, stepper->sm, pio_encode_out(pio_osr, 32));
        pio_sm_set_enabled(blink_pio, stepper->sm, true);
        stepper->enable = true;
    }

    // Set the direction pin
    gpio_put(stepper->pin + 1, direction);
}

uint32_t stepper_get_position(stepper_t* stepper) {
    return stepper->position;
}

uint stepper_get_pc(stepper_t* stepper) {
    return pio_sm_get_pc(blink_pio, stepper->sm) - blink_offset;
}

bool stepper_get_FIFO_empty(stepper_t* stepper) {
    return pio_sm_is_tx_fifo_empty(blink_pio, stepper->sm);
}
