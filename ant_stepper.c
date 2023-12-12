/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ant_stepper.pio.h"

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, int freq);

int main() {
    setup_default_uart();

    // todo get free sm
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &blink_program);
    printf("Loaded program at %d\n", offset);

    blink_pin_forever(pio, 0, offset, 25, 2);
    // blink_pin_forever(pio, 1, offset, 2, 10);
    // blink_pin_forever(pio, 2, offset, 11, 1);
    // blink_pin_forever(pio, 3, offset, 12, 8);

    while (true)
        tight_loop_contents();
}

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, int freq) {
    blink_program_init(pio, sm, offset, pin);
    pio_sm_set_enabled(pio, sm, true);

    printf("Blinking pin %d at %d Hz\n", pin, freq);

    // PIO counter program takes 3 more cycles in total than we pass as
    // input (wait for n + 1; mov; jmp)
    int8_t minus_3 = -3;
    // pio->txf[sm] = (clock_get_hz(clk_sys) / (2 * freq)) - 3;
    uint32_t sys_clk = clock_get_hz(clk_sys);
    // a = 0xFE2329B3;
    int a = (sys_clk / (2 * freq)) +  minus_3;
    a = 0x00;
    // pio->txf[sm] = (sys_clk / (2 * freq)) +  minus_3;
    int b = a | ((int)1 << 31); // toto je dore, realne to prida 1ku na msb
    pio->txf[sm] = b;
}

