#ifndef STEPPER_CONTROLLER_H
#define STEPPER_CONTROLLER_H

// Forward declaration of the stepper structure
typedef struct stepper stepper_t;

/**
 * @brief Initialize a stepper motor controller.
 * 
 * @param pin GPIO pin used for the step signal.
 * @param sm  PIO state machine number to use.
 * @return Pointer to the initialized stepper_t structure.
 */
stepper_t* stepper_init(const uint8_t pin, const uint8_t sm);

/**
 * @brief Perform stepper control computations (should be called periodically).
 * 
 * @param stepper Pointer to the stepper_t structure.
 */
void stepper_compute(stepper_t* const stepper);

/**
 * @brief Check if the stepper is currently active (moving).
 * 
 * @param stepper Pointer to the stepper_t structure.
 * @return true if active, false otherwise.
 */
bool stepper_is_active(stepper_t* stepper);

/**
 * @brief Stop the stepper motor.
 * 
 * @param stepper Pointer to the stepper_t structure.
 */
void stepper_stop(stepper_t* stepper);

/**
 * @brief Update the speed of the stepper motor.
 * 
 * @param stepper Pointer to the stepper_t structure.
 * @param speed   New speed value (steps per second or similar unit).
 */
void stepper_update_speed(stepper_t* stepper, int32_t speed);

/**
 * @brief Get the current position of the stepper motor.
 * 
 * @param stepper Pointer to the stepper_t structure.
 * @return Current position (step count).
 */
uint32_t stepper_get_position(stepper_t* stepper);

#endif // STEPPER_CONTROLLER_H