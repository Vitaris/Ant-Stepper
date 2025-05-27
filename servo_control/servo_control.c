#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "servo_control.h"
#include "../button/button.h"

#define CYCLE_TIME 0.001
#define FOLLOWING_ERROR 1.0 // Maximum permisible position deviation

void servo_control_calculate_next_position(servo_control_t* servo_control);

struct servo_control {
    
    
    enum positioning{
        IDLE,
        REQUESTED,
        ACCELERATING,
        BRAKING,
        POSITION_REACHED
    } positioning;

    bool* man_plus;             // Pointer to the button for manual movement in positive direction
    bool* man_minus;            // Pointer to the button for manual movement in negative direction
    float* current_position;    // Pointer to the current position of the servo motor
    float next_position;

    float next_stop;
    float servo_position;
	float servo_speed;
	uint32_t delay_start;
	bool *enable;
	bool enable_previous;
	float computed_speed;
	bool positive_direction;
	bool set_zero;
	bool nominal_speed_reached;
	float enc_offset;

	// Default movement
	float nominal_speed; 	// Desired motor speed
	float nominal_acc;		// Motor acceleration
	float current_speed; 	// Desired motor speed
	float current_acc;		// Motor acceleration
	float scale;			// Scale factor of the servo_control motor
};

servo_control_t* servo_control_init(float* current_position, bool* enable, bool* man_plus, bool* man_minus) {
    servo_control_t* servo_control = calloc(1, sizeof(struct servo_control));
    if (servo_control == NULL) {
        fprintf(stderr, "Failed to allocate memory for servo_control control\n");
        return NULL;
    }
	servo_control->enable = enable;
    servo_control->man_plus = man_plus;
    servo_control->man_minus = man_minus;

    servo_control->current_position = current_position;

    // Default values
    servo_control->nominal_speed = 100.0f; // Example default speed
    servo_control->nominal_acc = 50.0f;    // Example default acceleration
    servo_control->current_speed = servo_control->nominal_speed;
    servo_control->current_acc = servo_control->nominal_acc;
    servo_control->scale = 1.0f;           // Default scale factor

    return servo_control;
}

void servo_control_compute(servo_control_t* servo_control) {
    servo_control_calculate_next_position(servo_control);
}

float get_breaking_distance(const servo_control_t* const servo_control) {
	return 0.5 * (pow(servo_control->computed_speed, 2) / servo_control->current_acc);
}

void servo_control_calculate_next_position(servo_control_t* servo_control) {
	switch(servo_control->positioning) {
        case IDLE:
			servo_control->nominal_speed_reached = false;
			break;
		
		case REQUESTED:
			// First occurence of movement request, save the position of movement beginning
			if (servo_control->next_stop >= *servo_control->current_position) {
				// Positive direction
				servo_control->positive_direction = true;
				servo_control->current_acc = servo_control->nominal_acc;
				servo_control->current_speed = servo_control->nominal_speed;
			} else {
				// Negative direction
				servo_control->positive_direction = false;
				servo_control->current_acc = -servo_control->nominal_acc;
				servo_control->current_speed = -servo_control->nominal_speed;
			}
			if (servo_control->delay_start > 0) {
				servo_control->delay_start--;
				break;
			}
			servo_control->computed_speed = 0.0;
			servo_control->positioning = ACCELERATING;
			break;

		case ACCELERATING:
			servo_control->computed_speed += servo_control->current_acc * CYCLE_TIME;

			// check if nominal speed has been reached
			if (fabs(servo_control->computed_speed) > fabs(servo_control->current_speed)) {
				servo_control->nominal_speed_reached = true;
				servo_control->computed_speed = servo_control->current_speed;
			}

			// Compute position for next cycle time
			servo_control->next_position += servo_control->computed_speed * CYCLE_TIME;

			// Check if braking is needed
			if (servo_control->positive_direction) {
				if (servo_control->next_stop - servo_control->next_position < get_breaking_distance(servo_control)) {
					servo_control->positioning = BRAKING;
				}
			} else {
				if (servo_control->next_stop - servo_control->next_position > get_breaking_distance(servo_control)) {
					servo_control->positioning = BRAKING;
				}
			}
			break;

		case BRAKING:
			servo_control->computed_speed -= servo_control->current_acc * CYCLE_TIME;
			servo_control->next_position += servo_control->computed_speed * CYCLE_TIME;
			servo_control->nominal_speed_reached = false;
			
			// Check if desired position has been reached
			if (servo_control->positive_direction) {
				if (servo_control->computed_speed <= 0.0) {
					servo_control->positioning = POSITION_REACHED;
				}
			} else {
				if (servo_control->computed_speed >= 0.0) {
					servo_control->positioning = POSITION_REACHED;
				}
			}
			break;

		case POSITION_REACHED:
			servo_control->next_position = servo_control->next_stop;
			servo_control->positioning = IDLE;
			break;
	}
}

void _servo_goto(servo_control_t* const const servo_control, const float position, const float speed) {
	servo_control->next_stop = position / servo_control->scale;
	servo_control->nominal_speed = speed / servo_control->scale;
	if (servo_control->delay_start == 0) {
		servo_control->delay_start = 500;
	}
	else if (servo_control->delay_start == UINT32_MAX) { // TODO, add some sign to not add delay, e.g. maximum number
		servo_control->delay_start = 0;
	}
	servo_control->positioning = REQUESTED;
}

float servo_control_get_next_position(servo_control_t* servo_control) {
	return servo_control->next_position;
}

void servo_control_goto(servo_control_t* const servo_control, const float position, const float speed) {
    // Check if the servo control is already in a positioning state
	if (servo_control->positioning != IDLE) {
		// If it is, we can just update the next stop position and speed
		servo_control->next_stop = position / servo_control->scale;
		servo_control->nominal_speed = speed / servo_control->scale;
		return;
	}
    
    // If not, we can start a new positioning request
    _servo_goto(servo_control, position, speed);
}

void servo_control_change_acc(servo_control_t* const servo_control, const float acc) {
	servo_control->nominal_acc = acc;
}


// void servo_manual_handling(servo_control_t* const servo_control, const float min, const float max, const float speed, bool homed) {
// 	float limit_min;
// 	float limit_max;

// 	if (homed) {
// 		limit_min = min;
// 		limit_max = max; 
// 	}
// 	else {
// 		limit_min = -2000;
// 		limit_max = 2000;
// 	}
// 	if (button_raised(servo_control->man_plus)) {
// 		servo_control->delay_start = UINT32_MAX;
// 		servo_goto(servo_control, servo_control->next_stop = limit_max, speed);
// 	}
// 	else if (button_raised(servo_control->man_minus)) {
// 		servo_control->delay_start = UINT32_MAX;
// 		servo_goto(servo_control, servo_control->next_stop = limit_min, speed);
// 	}
// 	else if (button_dropped(servo_control->man_plus) || button_dropped(servo_control->man_minus)) {
// 		servo_stop_positioning(servo_control);
// 	}
// }
