#include <Arduino.h>
#include "commandReader.h"
#include "motor/PenServo.hpp"
#include "motor/PenStepper.hpp"

// The following is a simple stepper motor control procedures
# define EN 8 // stepper motor enable , active low
# define X_DIR 5 // X -axis stepper motor direction control
# define Y_DIR 6 // y -axis stepper motor direction control
# define Z_DIR 7 // z axis stepper motor direction control
# define X_STP 2 // x -axis stepper control
# define Y_STP 3 // y -axis stepper control
# define Z_STP 4 // z -axis stepper control

#define STEP_DELAY_MS 200

PenServo penServo(Z_STP, 90, 0.6, true);
PenStepper penStepperX(X_STP, X_DIR, 27);

void setupMotors() {
	penStepperX.enable();
	penServo.enable();

	penStepperX.home();
	penServo.home();
}

void setup () {   // The stepper motor used in the IO pin is set to output
    setupMotors();

	penStepperX.setTargetPosition(30);

	penServo.setTargetPosition(22);
}

char line[100];

void loop () {
	penStepperX.tick(micros());
	
	Serial.print("Position X: ");
	Serial.print(penStepperX.getPosition(), 10);
	Serial.println();

	if (penStepperX.getPosition() == penStepperX.getTargetPosition() && penStepperX.getPosition() >= 20) {
		penServo.setTargetPosition(0);

		penStepperX.setSpeed(60);
		penStepperX.setTargetPosition(10);
	}

	// printEmoticons();

	// penServo.move(20, 0);
	// delay(1000);
	// penServo.move(0, 0);
	// delay(1000);

	// Serial.println("Step");
	// servo.write(40); 
    // step (false, X_DIR, X_STP, 400, 1); // X axis motor reverse 1 ring, the 200 step is a circle.
    // step (false, Y_DIR, Y_STP, 400, 1); // X axis motor reverse 1 ring, the 200 step is a circle.
	// servo.write(0); 
	// step (true, X_DIR, X_STP, 400, 1); // X axis motor forward 1 laps, the 200 step is a circle.
	// step (true, Y_DIR, Y_STP, 400, 1); // X axis motor forward 1 laps, the 200 step is a circle.
} 