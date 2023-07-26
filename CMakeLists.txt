cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(pio_blink)

# by default the header is generated into the build dir
pico_generate_pio_header(pio_blink ${CMAKE_CURRENT_LIST_DIR}/ant_stepper.pio)
# however, alternatively you can choose to generate it somewhere else (in this case in the source tree for check in)
#pico_generate_pio_header(pio_blink ${CMAKE_CURRENT_LIST_DIR}/blink.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})

target_sources(pio_blink PRIVATE ant_stepper.c)

target_link_libraries(pio_blink PRIVATE pico_stdlib hardware_pio)
pico_add_extra_outputs(pio_blink)

# add url via pico_set_program_url
# example_auto_set_url(pio_blink)