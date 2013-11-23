#pragma config(Hubs,  S1, HTMotor,  HTServo,  HTMotor,  HTMotor)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     driveLeft,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     driveRight,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_1,     elevatorElevator, tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C3_2,     motorG,        tmotorTetrix, openLoop) // Unused
#pragma config(Motor,  mtr_S1_C4_1,     motorH,        tmotorTetrix, openLoop) // Unused
#pragma config(Motor,  mtr_S1_C4_2,     motorI,        tmotorTetrix, openLoop) // Unused
#pragma config(Servo,  srvo_S1_C2_1,    flipper,              tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C2_2,    leftGrabber,          tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C2_3,    rightGrabber,         tServoContinuousRotation)
#pragma config(Servo,  srvo_S1_C2_4,    servo4,               tServoNone) // Unused
#pragma config(Servo,  srvo_S1_C2_5,    servo5,               tServoNone) // Unused
#pragma config(Servo,  srvo_S1_C2_6,    servo6,               tServoNone) // Unused
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
 * teleop.c
 * This file is part of Teleop
 *
 * Copyright (C) 2012 - EMC Squirrled
 *
 * Teleop is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of th e License, or
 * (at your option) any later version.
 *
 * Teleop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Teleop; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */


/* ===== INCLUDES ===== */

#include <JoystickDriver.c>

/* ===== DEFINES ===== */

/* Milliseconds between updates */
#define JOYSTICK_UPDATE_TIME 10
#define MOTOR_UPDATE_TIME 10

/* Joystick range to ignore */
#define DEADZONE 10

/* DPAD directional values */
#define DPAD_NONE -1
#define DPAD_UP 0
#define DPAD_DOWN 4
#define DPAD_LEFT 6
#define DPAD_RIGHT 2

/* IR seeker mast servo values */
#define RAISE_THE_TOPSAIL 255
#define LOWER_THE_TOPSAIL 10

/* Ring Divider servo values */
#define REMOVE_THE_DIVIDER 255
#define PLACE_THE_DIVIDER 195

/* This is not a #define, but we treat it like one
   Used for file I/O */
word FILE_SIZE = 255;

/* ===== TASKS ===== */

task ReadJoystick1();
task ReadJoystick2();
task UpdateDriveMotors();
task UpdateManipulators();
task Debug();

/* ===== STRUCTURES ===== */

/* Defines the different drive modes. The specific numbers are also codes for the run control file */
typedef enum {
	TNK_LIN = 1,
	TNK_EXP = 0,
	TNK_LOW = 2,
	ARC_LIN = 4,
	ARC_EXP = 3,
	ARC_LOW = 5,
} mode;

/* Defines which side of the robot we are talking about */
typedef enum {
	LEFT,
	RIGHT,
} side;

/* Defines a motor */
typedef struct {
	signed byte iPower;
} my_motor;

/* Defines a servo */
typedef struct {
	unsigned byte iPosition;
} my_servo;

/* Defines our robot */
typedef struct {
	my_motor oLeftMotor;
	my_motor oRightMotor;
	int grabberPower;
	int flipperPower;
	int elevatorPower;
	int elevatorElevatorPower;
	int dispenserPower;
	mode oMode;
} robot;

/* ==== FUNCTIONS ===== */

void vInitializeRobot(void);
int iJoyToPower(side theSide);

/* ===== GLOBALS === */

/* Create a robot from the above template */
robot oRobot;

/* ===== CODE ===== */

/* Task Main -- Entry point of the code */
task main() {
	/* Initialize the robot and wait for the start of the match */
	vInitializeRobot();
	waitForStart();

	/* Start all tasks */
	StartTask(ReadJoystick1);
	StartTask(ReadJoystick2);
	StartTask(UpdateDriveMotors);
	StartTask(UpdateManipulators);
	StartTask(Debug);

	/* Do nothing, for the tasks do it all */
	while(1) wait10Msec(1000);
}

/* Set driving data from the first joystick */
task ReadJoystick1() {
	wait1Msec(3);
	/* Run forever */
	while(1) {
		/* Update the joystick structure */
	    getJoystickSettings(joystick);

		/* Overide the joystick values if the DPad is used */
	    if (joystick.joy1_TopHat == DPAD_UP) {
	    	oRobot.oLeftMotor.iPower = 35;
	    	oRobot.oRightMotor.iPower = 35;
	  	} else if (joystick.joy1_TopHat == DPAD_DOWN) {
	    	oRobot.oLeftMotor.iPower = -35;
	    	oRobot.oRightMotor.iPower = -35;
	  	} else  if (joystick.joy1_TopHat == DPAD_LEFT) {
	    	oRobot.oLeftMotor.iPower = -70;
	    	oRobot.oRightMotor.iPower = 70;
	  	} else  if (joystick.joy1_TopHat == DPAD_RIGHT) {
	    	oRobot.oLeftMotor.iPower = 70;
	    	oRobot.oRightMotor.iPower = -70;
	  	} else  if(joy1Btn(4)) {
	    	oRobot.oLeftMotor.iPower = 40;
	    	oRobot.oRightMotor.iPower = 40;
        } else  if(joy1Btn(2)) {
	    	oRobot.oLeftMotor.iPower = -40;
	    	oRobot.oRightMotor.iPower = -40;
        } else {
			/* Set the drive power from the joysticks */
		    oRobot.oLeftMotor.iPower = iJoyToPower(LEFT);
		    oRobot.oRightMotor.iPower = iJoyToPower(RIGHT);
    	}

		/* Give processor time to the other tasks */
	    wait1Msec(JOYSTICK_UPDATE_TIME);
	}
}

/* Set manipulator data from the second joystick */
task ReadJoystick2() {
	wait1Msec(4);
	/* Run forever */
	while(1) {
		/* Update the joystick structure */
		getJoystickSettings(joystick);

		/* Set the arm's power from the DPad */
		oRobot.elevatorElevatorPower = 0;
		if(joystick.joy2_TopHat == DPAD_UP) {
			oRobot.elevatorElevatorPower = 60;
		}
		if(joystick.joy2_TopHat == DPAD_DOWN) {
			oRobot.elevatorElevatorPower = -30;
		}

		/* Set the turntable's power from the DPad */
		oRobot.dispenserPower = 0;
		if(joystick.joy2_TopHat == DPAD_LEFT) {
			oRobot.dispenserPower = -50;
		}
		if(joystick.joy2_TopHat == DPAD_RIGHT) {
			oRobot.dispenserPower = 50;
		}
		/* Set the ring divider's position from the left triggers */
		oRobot.grabberPower = 127;
		if(joy2Btn(6)) {
			oRobot.grabberPower = 255;
		}
		if(joy2Btn(8)) {
			oRobot.grabberPower = 0;
		}
		oRobot.flipperPower = 127;
		if(joy2Btn(5)) {
			oRobot.elevatorPower = 50;
		}
		if(joy2Btn(7)) {
			oRobot.elevatorPower = -50;
		}
		/* Give processor time to other tasks */
		wait1Msec(JOYSTICK_UPDATE_TIME);
	}
}

/* Update the driving hardware */
task UpdateDriveMotors() {
	/* Run forever */
	while(1) {
		/* Set the motors with the requested values */
	    motor[driveLeft] = -1 * oRobot.oLeftMotor.iPower;
	    motor[driveRight] = oRobot.oRightMotor.iPower;

		/* Give processor time to other tasks */
	    wait10Msec(MOTOR_UPDATE_TIME);
	}
}

/* Update the manipulator hardware */
task UpdateManipulators() {
	wait1Msec(1);
	/* Run forever */
	while(1) {
		/* Set the manipulators with the requested values */
		servo[flipper] = oRobot.flipperPower;
		servo[leftGrabber] = oRobot.grabberPower;
		servo[rightGrabber] =  oRobot.grabberPower;
		motor[elevatorElevator] = oRobot.elevatorElevatorPower;
		motor[motorB] = oRobot.elevatorPower;
		motor[motorC] = oRobot.dispenserPower;
		//motor[] = oRobot.elevatorElevatorPower  /////////// Need to finish this part
		/* Give processor time to other tasks */
		wait1Msec(MOTOR_UPDATE_TIME);
	}
}

/* Print debugging information */
task Debug() {
	/* Clear the screen so we can use it */
	disableDiagnosticsDisplay();

	/* Run forever */
	while(1) {
		/* Output the interal state of the robot structure */
		eraseDisplay();
		nxtDisplayCenteredTextLine(1, "LEFT: %d", oRobot.oLeftMotor.iPower);
		nxtDisplayCenteredTextLine(2, "RIGHT: %d", oRobot.oRightMotor.iPower);
		wait10Msec(1);
	}
}

/* Initialize the entire robot */
void vInitializeRobot() {
	/* Initialize file I/O variables */
	TFileHandle file;
	TFileIOResult result;
	byte value;

	/* Set all motors and servos to their initial positions */
	oRobot.oLeftMotor.iPower = 0;
	oRobot.oRightMotor.iPower = 0;

	/* Read the driving mode from a run control file */
	OpenRead(file, result, "teleoprc.txt", FILE_SIZE);
	ReadShort(file, result, value);
	Close(file,result);
	oRobot.oMode = (mode) (value - 48);

	return;
}

/* Calculates the motor power for a specific side based off the joystick value and drive mode */
int iJoyToPower(side theSide) {
	/* Initialize variables */
	float fLeftPower;
	float fRightPower;

	/* Choose the algorithm based off of the requested mode */
	switch(oRobot.oMode) {
		/* Linearly scaled tank mode */
		case TNK_LIN:
			fLeftPower = (float) (((float) joystick.joy1_y1) / 127);
			fRightPower = (float) (((float) joystick.joy1_y2) / 127);
			fLeftPower *= 100;
			fRightPower *= 100;
			fLeftPower = abs(fLeftPower) >= DEADZONE ? fLeftPower : 0;
			fRightPower = abs(fRightPower) >= DEADZONE ? fRightPower : 0;
			break;

		/* Exponentially scaled tank mode */
		case TNK_EXP:
			fLeftPower = (float) (((float) joystick.joy1_y1) / 127);
			fRightPower = (float) (((float) joystick.joy1_y2) / 127);
			pow(fLeftPower, 3);
			pow(fRightPower, 3);
			fLeftPower *= 100;
			fRightPower *= 100;
			break;

		/* Linearly scaled lowspeed tank mode */
		case TNK_LOW:
			fLeftPower = (float) (((float) joystick.joy1_y1) / 127);
			fRightPower = (float) (((float) joystick.joy1_y2) / 127);
			fLeftPower *= 50;
			fRightPower *= 50;
			fLeftPower = abs(fLeftPower) >= DEADZONE ? fLeftPower : 0;
			fRightPower = abs(fRightPower) >= DEADZONE ? fRightPower : 0;
			break;

		/* Linearly scaled arcade mode */
		case ARC_LIN:
			if(abs(joystick.joy1_y1) > DEADZONE) {
				fLeftPower = 100 * ((float) (((float) joystick.joy1_y1) / 127));
				fRightPower = 100 * ((float) (((float) joystick.joy1_y1) / 127));
			} else if(abs(joystick.joy1_x2) > DEADZONE) {
				fLeftPower = 100 * ((float) (((float) joystick.joy1_x2) / 127));
				fRightPower = -100 * ((float) (((float) joystick.joy1_x2) / 127));
			} else {
				fLeftPower = 0;
				fRightPower = 0;
			}
			break;

		/* Exponentially scaled arcade mode */
		case ARC_EXP:
			if(abs(joystick.joy1_y1) > DEADZONE) {
				fLeftPower = (float) (((float) joystick.joy1_y1) / 127);
				fRightPower = (float) (((float) joystick.joy1_y1) / 127);
				pow(fLeftPower, 3);
				pow(fRightPower, 3);
				fLeftPower *= 100;
				fRightPower *= 100;
			} else if(abs(joystick.joy1_x2) > DEADZONE) {
				fLeftPower = (float) (((float) joystick.joy1_x2) / 127);
				fRightPower = (float) (((float) joystick.joy1_x2) / 127);
				pow(fLeftPower, 3);
				pow(fRightPower, 3);
				fLeftPower *= 100;
				fRightPower *= -100;
			} else {
				fLeftPower = 0;
				fRightPower = 0;
			}
			break;

		/* Linearly scaled lowspeed arcade mode */
		case ARC_LOW:
			if(abs(joystick.joy1_y1) > DEADZONE) {
				fLeftPower = 50 * ((float) (((float) joystick.joy1_y1) / 127));
				fRightPower = 50 * ((float) (((float) joystick.joy1_y1) / 127));
			} else if(abs(joystick.joy1_x2) > DEADZONE) {
				fLeftPower = 50 * ((float) (((float) joystick.joy1_x2) / 127));
				fRightPower = -50 * ((float) (((float) joystick.joy1_x2) / 127));
			} else {
				fLeftPower = 0;
				fRightPower = 0;
			}
			break;

		/* Somehow we have a garbled drive mode */
		default:
			oRobot.oMode = TNK_LIN;
			PlaySound(soundBeepBeep);
	}

	/* Return the requested value */
	switch(theSide) {
		case LEFT:
			return (int) fLeftPower;
			break;

		case RIGHT:
			return (int) fRightPower;
			break;

		/* Somehow we have a garbled drive mode; don't move! */
		default:
			PlaySound(soundBeepBeep);
			return 0;
	}
}
