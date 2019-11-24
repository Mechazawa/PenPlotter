#include <Arduino.h>
#include "commandReader.h"
#include "motor/PenServo.hpp"
#include "motor/PenStepper.hpp"
#include "Position.hpp"
#include "Axis.hpp"
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
Axis axis(64);

void setupMotors(bool home = true) {
	penStepperX.enable();
	penStepperY.enable();
	penServo.enable();

	if (home) {
		penStepperX.home();
		penStepperY.home();
		penServo.home();
	}

	axis.setMotor('X', &penStepperX);
	axis.setMotor('Y', &penStepperY);
	axis.setMotor('Z', &penServo);
}

const char* moves = "Z0;"
"X10 Y10;"
""
"Z18;"
"X30;"
"Y30;"
"X10;"
"Y10;"
"Z0;"
""
"Z18;"
"X25;"
"Y25;"
"X15;"
"Y15;"
"Z0;"
""
"X0 Y0;";

void initMoves(Milimeter speed = 20) {
	Position* position = new Position;
	int i = 0;
	char curAxis = 'X';

	while (char c = moves[i++]) {
		if (c == ';') {
			axis.pushMove(position, speed);

			position = new Position;
		} else if (c >= '0' && c <= '9') {
			Milimeter value = position->getAxis(curAxis);
			
			value *= 10;
			value += c - '0';

			position->setAxis(curAxis, value);
		} else if (c >= 'A' && c <= 'Z') {
			position->setAxis(curAxis = c, 0);
		}
	}
}

void setup () { 
	// The stepper motor used in the IO pin is set to output
	delay(5000);
    setupMotors();
	initMoves();

	// Position* pos = new Position;
	// pos->setAxis('X', 20);
	// pos->setAxis('Y', 20);

	// axis.pushMove(pos, 20);

	// good speed is 20-30

	Serial.print("Moves: ");
	Serial.print(axis.getStackSize(), 10);
	Serial.println();

	if(axis.hasMotor('X')) {
		Serial.println("X motor registered");
		Serial.print(axis.getMotor('X')->getTargetPosition(), 10);
	} else {
		Serial.println("X motor not found!");
	}
}

void loop () {
	axis.tick();

	// if (!axis.moving()) {
	// 	penStepperX.disable();
	// 	penStepperY.disable();
	// 	penServo.disable();
	// }
} 