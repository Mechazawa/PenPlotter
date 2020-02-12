#include <Arduino.h>
// #include "commandReader.h"
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

PenServo penServo(12, 90, 60);
PenStepper penStepperX(X_STP, X_DIR, 30);
PenStepper penStepperY(Z_STP, Y_DIR, 30);
Axis axis(20); // 42 works

void shutDownMotors() {
	penStepperX.disable();
	penStepperY.disable();
	penServo.disable();

	axis.deleteMotor('X');
	axis.deleteMotor('Y');
	axis.deleteMotor('Z');
}

void setupMotors(bool home = true) {
	penStepperX.enable();
	penStepperY.enable();
	penServo.enable();

	if (home) {
		penServo.home();
		penStepperX.home();
		penStepperY.home();
	}

	axis.setMotor('X', &penStepperX);
	axis.setMotor('Y', &penStepperY);
	axis.setMotor('Z', &penServo);
}

// const char* moves = "U;X16 Y23;D;X16 Y24;X14. Y25;X12. Y27;X10 Y27;X7. Y27;X5. Y26;X3. Y25;X2 Y23;X1. Y20;X2 Y18;X2. Y16;X3. Y13;X5 Y11;X6. Y10;X8. Y8;X10. Y7;X12 Y5;X14 Y3;X15. Y2;X17. Y3;X19. Y5;X21 Y6;X23 Y8;X25 Y9;X26. Y11;X28 Y13;X29 Y15;X30 Y18;X30 Y20;X29. Y22;X28. Y25;X26. Y26;X24. Y27;X22 Y27;X19. Y27;X17. Y25;X16 Y24;U;X30Y30;";
// const char* moves = "U;X10Y10;D;X30Y30;Y10;X10;U;Y30X0;";

void parseCommand(String command, Milimeter speed = 13) {
	Serial.print("INPUT \"");
	Serial.print(command);
	Serial.println("\"");

	Position* position = new Position;
	char curAxis = 'X';

	for(unsigned int i = 0; i < command.length(); i++) {
		char c = command[i];

		if (c >= '0' && c <= '9') {
			Milimeter value = position->getAxis(curAxis);
			
			value *= 10;
			value += c - '0';

			position->setAxis(curAxis, value);
		} else if (c == 'U') {
			position->setAxis('Z', 0);
		} else if (c == 'D') {
			position->setAxis('Z', 14.8);
		} else if (c >= 'A' && c <= 'Z') {
			position->setAxis(curAxis = c, 0);
		} else if (c == '.') {
			Milimeter value = position->getAxis(curAxis);
			position->setAxis(curAxis, value + 0.5);
		} else if (c == '!') {
			setupMotors(false);
		} else if (c == '#') {
			shutDownMotors();
		} else if (c == '?') {
			Serial.print("X: ");
			Serial.print(axis.getMotor('X')->getPosition(), 10);
			Serial.print(" (?");
			Serial.print(penStepperX.getMissedSteps(), 10);
			Serial.println(")");

			Serial.print("Y: ");
			Serial.print(axis.getMotor('Y')->getPosition(), 10);
			Serial.print(" (?");
			Serial.print(penStepperY.getMissedSteps(), 10);
			Serial.println(")");


			Serial.print("Z: ");
			Serial.println(axis.getMotor('Z')->getPosition(), 10);
		}
	}

	if (axis.pushMove(position, speed)) {
		Serial.print("OK ");
	} else {
		delete position;

		Serial.print("FULL ");
	}


	Serial.print(axis.getMaxStackSize() - axis.getStackSize(), 10);
	Serial.print("/");
	Serial.println(axis.getMaxStackSize(), 10);
}

String commandLoopInput;
void commandLoop() {;
  if (Serial.available()) {
	char c = Serial.read();

	if (c == ';') {
		parseCommand(commandLoopInput);

		commandLoopInput = "";
	} else {
		commandLoopInput += c;
	}
  }
}

void setup () { 
  	Serial.begin(9600);
	// The stepper motor used in the IO pin is set to output
	delay(2000);
    setupMotors();

	// Position* pos = new Position;
	// pos->setAxis('X', 20);
	// pos->setAxis('Y', 20);

	// axis.pushMove(pos, 20);

	// good speed is 20-30
}

unsigned long lastCmdLoopMs = 0;
unsigned long ms;
void loop () {
	axis.tick(ms = micros());

	if (ms - lastCmdLoopMs >= 10000) {
		commandLoop();

		lastCmdLoopMs = ms;
	}
} 