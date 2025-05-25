#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H
#include <stdbool.h>
#include <stdint.h>

typedef struct servo_control servo_control_t;

servo_control_t* servo_control_init(float* current_position, bool* man_plus, bool* man_minus);

void servo_control_compute(servo_control_t* servo_control);

void servo_control_calculate_next_position(servo_control_t* servo_control);

float servo_control_get_next_position(servo_control_t* servo_control);
void servo_control_goto(servo_control_t* const servo_control, const float position, const float speed);

#endif // SERVO_CONTROL_H