#ifndef PID_H
#define PID_H
 
#include <stdbool.h>
#include <stdint.h>

typedef struct pid_data pid_data_t;

/**
 * @brief Creates a new PID controller
 *
 * Creates a new pid controller and initializes it�s input, output and internal
 * variables. Also we set the tuning parameters
 *
 * @param pid A pointer to a pid_controller structure
 * @param in Pointer to float value for the process input
 * @param out Poiter to put the controller output value
 * @param set Pointer float with the process setpoint value
 * @param kp Proportional gain
 * @param ki Integral gain
 * @param kd Diferential gain
 * @param error_signal Pointer to error handler
 *
 * @return returns a pid_data_t* controller handle
 */
pid_data_t* pid_create(float* in, float* out, float* set, float kp, float ki, float kd);


/**
 * @brief Computes the output of the PID control
 *
 * This function computes the PID output based on the parameters, setpoint and
 * current system input.
 *
 * @param pid The PID controller instance which will be used for computation
 */
void pid_compute(pid_data_t* const pid);

void pid_reset_all(pid_data_t* const pid);

bool pid_get_error(const pid_data_t* const pid);

#endif
