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

PenServo penServo(12, 90, 60, true);
PenStepper penStepperX(X_STP, X_DIR, 27*1.5);
PenStepper penStepperY(Z_STP, Y_DIR, 27*1.5);
Axis axis(128);

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

// const char* moves = "Z0;"
// "X10 Y10;"
// ""
// "Z18.;"
// "X30;"
// "Y30;"
// "X10;"
// "Y10;"
// "Z0;"
// ""
// "X25;"
// "Z18.;"
// "Y25;"
// "X15;"
// "Y15;"
// "X25;"
// "Z0;"
// ""
// "X0 Y0;";

const char* moves = "U;X16 Y23;D;X16 Y24;X14. Y25;X12. Y27;X10 Y27;X7. Y27;X5. Y26;X3. Y25;X2 Y23;X1. Y20;X2 Y18;X2. Y16;X3. Y13;X5 Y11;X6. Y10;X8. Y8;X10. Y7;X12 Y5;X14 Y3;X15. Y2;X17. Y3;X19. Y5;X21 Y6;X23 Y8;X25 Y9;X26. Y11;X28 Y13;X29 Y15;X30 Y18;X30 Y20;X29. Y22;X28. Y25;X26. Y26;X24. Y27;X22 Y27;X19. Y27;X17. Y25;X16 Y24;U;X30Y30;";

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
		} else if (c == 'U') {
			position->setAxis('Z', 0);
		} else if (c == 'D') {
			position->setAxis('Z', 19.4);
		} else if (c >= 'A' && c <= 'Z') {
			position->setAxis(curAxis = c, 0);
		} else if (c == '.') {
			Milimeter value = position->getAxis(curAxis);
			position->setAxis(curAxis, value + 0.5);
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

	if (!axis.moving()) {
		penStepperX.disable();
		penStepperY.disable();
		penServo.disable();
	}
} 