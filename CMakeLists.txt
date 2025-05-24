cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ant_stepper)

pico_sdk_init()

add_executable(ant_stepper
    ant_stepper.c
    lcd/lcd.c
    button/button.c
    src/stepper_controller.c
)

pico_generate_pio_header(ant_stepper ${CMAKE_CURRENT_LIST_DIR}/src/step_generator.pio)
pico_generate_pio_header(ant_stepper ${CMAKE_CURRENT_LIST_DIR}/src/stepdir_counter.pio)

# enable usb output, disable uart output
pico_enable_stdio_usb(ant_stepper 1)
pico_enable_stdio_uart(ant_stepper 0)

pico_add_extra_outputs(ant_stepper)

target_link_libraries(ant_stepper pico_stdlib
        pico_multicore
        hardware_pio
        hardware_pwm
        hardware_adc
        )
        