#include "PID.h"
#include <stdlib.h>

static const float PID_OUT_MIN = -1024.0f;
static const float PID_OUT_MAX = 1024.0f;
static const float PID_ITERM_MIN = -1024.0f;
static const float PID_ITERM_MAX = 1024.0f;

struct pid_data {
	// Input, output and setpoint
	float * input; 		// Current Process Value
	float * output; 	// Corrective Output from PID Controller
	float * setpoint; 	// Controller Setpoint
	
	// Tuning parameters
	float Kp; 			// Stores the gain for the Proportional term
	float Ki; 			// Stores the gain for the Integral term
	float Kd;			// Stores the gain for the Derivative term

	float iterm; 		// Accumulator for integral term
	float lastin; 		// Last input value for differential term
	
	bool error; 		// Error flag
};

pid_data_t* pid_create(float* in, float* out, float* set, float kp, float ki, float kd)
{
	pid_data_t* pid = calloc(1, sizeof(struct pid_data));

	// Initialize PID data structure by given parameters
	pid->input = in;
	pid->output = out;
	pid->setpoint = set;

	pid->Kp = kp;
	pid->Ki = ki;
	pid->Kd = kd;
		
	return pid;
}

void pid_compute(pid_data_t* const pid)
{
	float in = *(pid->input);
	// Compute error
	float error = (*(pid->setpoint)) - in;

	// Compute integral
	pid->iterm += (pid->Ki * error);

	// Apply limit to integral value
	if (pid->iterm > PID_ITERM_MAX) {
		pid->iterm = PID_ITERM_MAX;
		pid->error = true;
	} else if (pid->iterm < PID_ITERM_MIN) {
		pid->iterm = PID_ITERM_MIN;
		pid->error = true;
	}

	// Compute differential on input
	float dinput = in - pid->lastin;

	// Compute PID output
	float out = pid->Kp * error + pid->iterm - pid->Kd * dinput;

	// Apply limit to output value
	if (out > PID_OUT_MAX) {
		out = PID_OUT_MAX;
	} else if (out < PID_OUT_MIN) {
		out = PID_OUT_MIN;
	}
	
	// Output to pointed variable
	(*pid->output) = out;

	// Keep track of some variables for next execution
	pid->lastin = in;
}

void pid_reset_all(pid_data_t* const pid) {
	pid->iterm = 0.0;
	*pid->output = 0.0;
	pid->lastin = *(pid->input);
	pid->error = false;
}

bool pid_get_error(const pid_data_t* const pid) {
	return pid->error;
}