#ifndef STEPPER_CONTROLLER_H
#define STEPPER_CONTROLLER_H

typedef struct stepper stepper_t;

stepper_t* stepper_init(const uint8_t pin, const uint8_t sm);

void stepper_compute(stepper_t* const stepper);

bool stepper_is_active(stepper_t* stepper);

void stepper_stop(stepper_t* stepper);

void stepper_update_speed(stepper_t* stepper, int32_t speed);

uint32_t stepper_get_position(stepper_t* stepper);

uint stepper_get_pc(stepper_t* stepper);

bool stepper_get_FIFO_empty(stepper_t* stepper);

#endif // STEPPER_CONTROLLER_H