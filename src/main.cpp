#include <Arduino.h>
#include "commandReader.h"
#include "motor/PenServo.hpp"
#include "motor/PenStepper.hpp"
// #include "Wire.h"
// #include <hd44780.h>                       // main hd44780 header
// #include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

// hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

//https://en.wikipedia.org/wiki/Plotutils

// The following is a simple stepper motor control procedures
# define EN 8 // stepper motor enable , active low
# define X_DIR 5 // X -axis stepper motor direction control
# define Y_DIR 6 // y -axis stepper motor direction control
# define Z_DIR 7 // z axis stepper motor direction control
# define X_STP 2 // x -axis stepper control
# define Y_STP 3 // y -axis stepper control
# define Z_STP 4 // z -axis stepper control

PenServo penServo(12, 90, 0.6, true);
PenStepper penStepperX(X_STP, X_DIR, 27);
PenStepper penStepperY(Z_STP, Y_DIR, 27);

void setupMotors() {
	penStepperX.enable();
	penStepperY.enable();
	penServo.enable();

	penStepperX.home();
	penStepperY.home();
	penServo.home();
}

void setup () { 
	// The stepper motor used in the IO pin is set to output
    setupMotors();

	penStepperY.setSpeed(20);
	penServo.setTargetPosition(19);
}

void loop () {
	unsigned long ms = micros();

	penStepperX.tick(ms);
	penStepperY.tick(ms);

	if (penServo.getTargetPosition() == penServo.getPosition() && penServo.getPosition() > 10 && penStepperX.homed()) {
		penStepperX.setTargetPosition(20);
		penStepperY.setTargetPosition(60);
	}

	if (penStepperY.getPosition() == penStepperY.getTargetPosition() && penStepperY.getPosition() >= 20) {
		penStepperX.setTargetPosition(40);
	}

	if	(penStepperX.getPosition() > 39) {
		penServo.setTargetPosition(0);

		penStepperX.disable();
		penStepperY.disable();
	}
} 