/******************************************************************************0
 * mymain.cpp
 *
 * Main function for the arduino based projects and environment.  The real
 * main.cpp lives int the arduino library and source listed by arduion version.
 * Found rolled into the AA_Arduino_core Lib.
 *
 * This file will always have two funcitons, setup() and loop(). These are the
 * functions defined in the arduino build environment, and as an effort to not
 * deviate will keep that format.
 *
 *
 *
 *  Created on: Jan 12, 2015
 *  Author: Patrick
 *  Last Edited: Jan 26, 2015
 *****************************************************************************/




#include <Arduino.h>
//#include <EEPROM.h>
//#include "VPID.h"
#include "CHMotorDriver.h"


//Function wrappers needed for use
void ISRwrapper();
void blinkLED(void);
void encISRwrapper();
void ISRkillswitch();
volatile char killvar = 0;

//Fishing control Control variables  new due to rotation in Encoder
int limitmax = 550;
int limitmin = -15;
//Cast locations
//int castrelease = 70;
//int caststart = -165;
//int castfinish = 420;
//int reelfinish  = 320;
//int hooklocation = 190;  // was 90
//int castrelease = -370;
//int caststart = -580;
//int castfinish = -10;
//int reelfinish  = -110;
//int hooklocation = -240;  // was 90

int castrelease = 220;
int caststart = 10;
int castfinish = 530;
int reelfinish  = 430;
int hooklocation = 330;  // was 90


//Test functions
void CAST(double acc, double dec, double speed);
void PreCAST(long location);
void testTraj(long dis, double vc, double acc, double dec, double vm, double vf);

//General Motion Control (precast and go to position)
double accel = 0.3;
double decel = 0.2;
double velmax = 2.5;
unsigned int ledState = LOW;

//Cast motion control
double castspeed = 21;  //1023 if using move only command;//Note 60 is max
double castspeedmax = 21;
double castspeedmin = 10;

unsigned long pwmper = 50;
double castacc = castspeed/2;
double castdec = castspeed/2;

//Fish state machine variables
int fishstate = 0;
int fishStateMachine(char key, int state);


int led = 13;
long cursp = 0;
unsigned char controlbyte = 0;

//int ccc  = 0; //Temporary counter for testing

CHMotorDriver myReel(&controlbyte,channel2, 0, pwmper,300*pwmper);
CHMotorDriver myRod(&controlbyte,channel1, 1, pwmper,300*pwmper);
IntervalTimer myTimer;

void setup()
{


	initArduino();

	// initialize the digital pin as an output.
	  pinMode(led, OUTPUT);
	  Serial.begin(115200);
	delay(6000);
	Serial.println(F("____________________________\nMD60 Test software\n-------------------------\n"));


	  //NEEDED for Leonardo only
	#if defined(USBCON)
		while(!Serial)delay(2);
	#endif

//	//myRod.initialize(1000);
//	Serial.println(F("GO!"));
//	Serial.println(myRod.myPIDsubsample);
//	Serial.println((myRod.myPIDperiod-1000)/1000);

	myRod.begin();
	myRod.setMotionProfile(accel, decel, velmax);
	myRod.SetPIDTunings(10,20,0.5,0,0,0); //PID using velocity as Position setpoint
	myRod.setEncoderLimits(limitmin, limitmax);
	myRod.disableEncoderLimits();

	myRod.disableSolenoid();
	myRod.setMotorPWM(Brake,1023);
	myReel.setMotorPWM(Brake,1023);


    myTimer.begin(ISRwrapper, pwmper);
    attachInterrupt(myRod.encApin,encISRwrapper, RISING);


//    Serial.print(F("EncoderAddress: "));
//    Serial.println(((myRod.eepromaddress_msb<<8)+ myRod.eepromaddress_lsb), HEX);
//    Serial.print(F("CurrentPos: "));
//    Serial.println(myRod.getPositionCurrent());
//    Serial.println(F("Init GO: Starting loop!"));

    #warning "PWM values are 0-1023 for some reason"

	Serial.println(F("Hello Peter, Please Zero device before fishing."));



//    delay(5000);
//    Serial.println(F("Hello Peter, Please enter '1' if the mark is in front of the alignment mark, else enter '2'"));
//
//    char exit_from_zero = 0;
//	int cc = -1;
//
//	myRod.setMotionProfile(accel, decel, velmax);
//	myRod.resetVPID();

//	while(!exit_from_zero) {
////        if(Serial.available())
////        {
//		Serial.setTimeout(50000);
//
//		cc = Serial.parseInt();
//		Serial.print(cc);
//		if (cc == 1){
//			Serial.println(F(" - Rod is in front of mark, finding zero"));
//
//
////			//Setup search for zero going backwards, DOES NOT WORK AND I DONT WHY 05/17
////			myRod.setPositionCurrent(40);
////			cursp = myRod.getPositionCurrent();
////			//myRod.gotoPosition(10,accel,decel,velmax,0,0);
////			myRod.resetVPID();
////			myRod.printAll();
//
//
//			//Kludge to make the device move a bit
//			cursp = myRod.getPositionCurrent();
//			cursp+=50;
//			myRod.gotoPosition(cursp, accel,decel,velmax, 0,0);
//
//
//			myRod.findEncoderZero();
//			cursp = myRod.getPositionCurrent();
//			exit_from_zero = 1;
//		}
//		else if (cc == 2){
//			Serial.println(F(" - Rod is behind the mark, finding zero"));
//
//			//Setup search for zero going forwards.  Does not work, no idea why
////			myRod.setPositionCurrent(-40);
////			cursp = myRod.getPositionCurrent();
//////			myRod.gotoPosition(-10,accel,decel,velmax,0,0);
////			myRod.resetVPID();
////			myRod.printAll();
//
//			//Kludge to make the device move a bit
//			cursp = myRod.getPositionCurrent();
//			cursp-=50;
//			myRod.gotoPosition(cursp, accel,decel,velmax, 0,0);
//
//
//			myRod.findEncoderZero();
//			cursp = myRod.getPositionCurrent();
//			exit_from_zero = 1;
//		}
//		else Serial.println(F("You didn't hit 1 or 2!  Try again"));
//		//Zero rod
//
////        }
////		Serial.print('.');
////		delay(100);
//
//    }
//    Serial.println(F("Rod zeroed!  Time to fish"));

}


void loop()
{

	char cc = 0;
	double dd = 0;
	int ii = 0;
	long v1 = 0;
	double v2 = 0;
	double v3 = 0;
	double v4 = 0;
	double v5 = 0;
	double v6 = 0;

	//unsigned long ii = 0;

	if(Serial.available())
	{
		cc = Serial.read();
		Serial.print(cc);
		switch (cc)
		{

			case 't':  //Find encoder Index
				Serial.setTimeout(5000);
				Serial.print(F("Type 123 to zero: "));
				ii = Serial.parseInt();
				Serial.println(ii);
				if(ii == 123)
				{
					myRod.findEncoderZero();
					cursp = myRod.getPositionCurrent();
					fishstate = 2;
					delay(2000);
					myRod.gotoPosition(reelfinish, accel, decel, 2.5,0,1);
				}
				break;

			case 'y':  //Find encoder Index
				Serial.setTimeout(5000);
				Serial.print(F("Type 123 to zero: "));
				ii = Serial.parseInt();
				Serial.println(ii);
				if(ii == 123)
				{
					myRod.findEncoderZero();
					cursp = myRod.getPositionCurrent();
					fishstate = 2;
					delay(2000);
					myRod.gotoPosition(reelfinish, accel, decel, 2.5,0,1);
				}
				break;

			case 'x':
				cursp = myRod.getPositionCurrent();
				cursp-=50;
				myRod.gotoPosition(cursp, accel,decel,velmax, 0,0);
				break;

			case 'z':
				cursp = myRod.getPositionCurrent();
				cursp+=50;
				myRod.gotoPosition(cursp, accel,decel,velmax, 0,0);
				break;

            case 'w':
                myRod.printAll();
                break;

            case '=':
                myRod.enableSolenoid();
                delay(1000);
                myReel.disableSolenoid();  //Either should work!
                break;

		/// Center SNP keymap
			case 'a':  //Center Puff Hard
				//Advance one state
				fishstate = fishStateMachine(cc, fishstate);

				break;

			case 'b':  //Center Puff Soft

				break;

			case 'c':  //Center Sip Hard
				//Go back one state
				fishstate = fishStateMachine(cc, fishstate);

				break;

			case 'e':  //Center Sip Soft

				break;

		/// Right SNP keymap
			case 'f':  //Right Puff Hard
				//Cast faster/farther +5
				//Cast slower shorter +5
				castspeed += 3;
				castspeed = constrain(castspeed, castspeedmin, castspeedmax);
				Serial.print(F("Cast Speed: "));
				Serial.println(castspeed);
				break;

			case 'g':  //Right Puff Soft

				break;

			case 'h':  //Right Sip Hard
				//Advance cast release +5
				castrelease += 5;
				Serial.print(F("Cast Release: "));
				Serial.println(castrelease);
				break;

			case 'i':  //Right Sip Soft

				break;

		/// Left SNP keymap
			case 'j':  //Left Puff Hard
				//Cast slower shorter +5
				castspeed -= 3;
				castspeed = constrain(castspeed, castspeedmin, castspeedmax);
				Serial.print(F("Cast Speed: "));
				Serial.println(castspeed);
				break;

			case 'k':  //Left Puff Soft

				break;

			case 'm':  //Left Sip Hard
				//Retard cast release -5
				castrelease -= 5;
				Serial.print(F("Cast Release: "));
				Serial.println(castrelease);
				break;

			case 'n':  //Left Sip Soft

				break;

		/// Joystick keymap
			case 'u':  //Joystick up, north
				myReel.setMotorPWM(Forward,1023);
				delay(30);
				break;

			case 'd':  //Joystick down, south
				myReel.setMotorPWM(Forward,1023);
				delay(30);
				break;

			case 'l':  //Joystick left, west, mapped to move rod forward
                cursp = myRod.getPositionCurrent();
                cursp-=50;
                myRod.gotoPosition(cursp, accel,decel,velmax, 0,0);
				break;

			case 'r':  //Joystick right, east
                cursp = myRod.getPositionCurrent();
                cursp+=50;
                myRod.gotoPosition(cursp, accel,decel,velmax, 0,0);
				break;

		/// Lip Switch
			case 'p':  //Lip switch
				Serial.println("Lip");
				break;







//		case 'o':
//			Serial.print(F("Vel_KP: "));
//			Serial.print(myRod.GetKp());
//			Serial.print(F("  Vel_KI: "));
//			Serial.print(myRod.GetKi());
//			Serial.print(F("  Vel_KD: "));
//			Serial.print(myRod.GetKd());
//			Serial.print(F("  POS_KP: "));
//			Serial.println(myRod.getKpp());
//			break;
//		case 'p':
//			Serial.setTimeout(5000);
//
//			Serial.print(F("Vel_KP: "));
//			dd = Serial.parseFloat();
//			Serial.println(dd);
//			myRod.kp_v = dd;
//
//			Serial.print(F("  Vel_KI: "));
//			dd = Serial.parseFloat();
//			Serial.print(dd);
//			myRod.ki_v = dd;
//
//			Serial.print(F("  Vel_KD: "));
//			dd = Serial.parseFloat();
//			Serial.print(dd);
//			myRod.kd_v = dd;
//
//			Serial.print(F("  Pos_KP: "));
//			dd = Serial.parseFloat();
//			Serial.println(dd);
//			myRod.kp_pos = dd;
//
//			myRod.SetPIDTunings(myRod.kp_v,myRod.ki_v, myRod.kd_v, myRod.kp_pos,0,0);
//
//
//			break;
//
//		case 'l':
//			Serial.print(F("Accel: "));
//			Serial.print(myRod.getAccel());
//			Serial.print(F("  Decel: "));
//			Serial.print(myRod.getDecel());
//			Serial.print(F("  VelMax: "));
//			Serial.println(myRod.getVelocityMax());
//			break;
//		case ';':
//			Serial.setTimeout(5000);
//
//			Serial.print(F("Accel: "));
//			dd = Serial.parseFloat();
//			Serial.print(dd);
//			if(dd) accel = dd;
//
//			Serial.print(F("  Decel: "));
//			dd = Serial.parseFloat();
//			Serial.print(dd);
//			if(dd)  decel = dd;
//
//			Serial.print(F("  VelMax: "));
//			dd = Serial.parseFloat();
//			Serial.println(dd);
//			if(dd)  velmax = dd;
//
//			//And write to motion
//			myRod.setMotionProfile(accel, decel, velmax);
//
//			break;
//		case '0':
//		case '/':
//			//myReel.setMotorPWM(Reverse,1024);
//			myReel.setMotorPWM(Forward,1024);
//			delay(30);
//			Serial.println("Reel!");
//			break;
//		//HOOK!
//		case '.':
//			myRod.gotoPosition(castrelease,1,decel, 8,0,0);
//			Serial.println("HOOK");
//			break;
//		case '=':
//			myRod.enableSolenoid();
//			delay(1000);
//			myReel.disableSolenoid();  //Either should work!
//			break;
//
//		//CAST FUNCTIONS
//		case 'z':  //Go to start location
//			//PreCAST(caststart);
//			myRod.gotoPosition(caststart, accel, decel, velmax,0,1);
//			cursp = myRod.getPositionCurrent();
//			break;
//		case 'c':  //Go to release location
//			myRod.gotoPosition(castrelease, accel, decel, velmax,0,1);
//			cursp = myRod.getPositionCurrent();
//			break;
//		case 'v':  //Go to finish location
//			myRod.gotoPosition(castfinish, accel, decel, velmax,0,1);
//			cursp = myRod.getPositionCurrent();
//			break;
//		case 'x': //Cast!!!
//			Serial.setTimeout(5000);
//			Serial.print(F("Type 123 to zero: "));
//			ii = Serial.parseInt();
//			Serial.println(ii);
//			if(ii == 123 && myRod.isEncoderSafe())	CAST(castacc, castdec,castspeed);
//			else Serial.println(F("Encoder not safe! NO CAST"));
//			cursp = myRod.getPositionCurrent();
//			break;
//		case 'b':  //Change cast params
//			Serial.print(F("Enter new cast speed: "));
//			dd = Serial.parseFloat();
//			Serial.println(dd);
//			if(dd != 0)	castspeed = dd;
//			break;
//		case '[':
//			castspeed--;
//			if(castspeed < 3) castspeed = 3;
//			Serial.print("CastSpeed: ");
//			Serial.println(castspeed);
//			break;
//		case ']':
//			castspeed++;
//			if(castspeed> 25) castspeed = 25;
//			Serial.print("CastSpeed: ");
//			Serial.println(castspeed);
//			break;
//
//
//		case 'n':  //Change cast locations
//			Serial.setTimeout(5000);
//			Serial.print(F("Enter new cast start: "));
//			ii = Serial.parseInt();
//			Serial.println(ii);
//			if(ii != 0)	caststart = ii;
//			Serial.print(F("Enter new cast release: "));
//			ii = Serial.parseInt();
//			Serial.println(ii);
//			if(ii != 0)	castrelease = ii;
//			Serial.print(F("Enter new cast finish: "));
//			ii = Serial.parseInt();
//			Serial.println(ii);
//			if(ii != 0)	castfinish = ii;
//			break;
//
//		/** Trajectory testing command **/
//		case 'u':  //Change cast params
//			Serial.setTimeout(5000);
//			myRod.disableVPIDpwm = 1;
//			Serial.print(F("Distance: "));
//			v1 = Serial.parseInt();
//			Serial.println(v1);
//			Serial.print(F("Vcur: "));
//			v2 = Serial.parseFloat();
//			Serial.println(v2);
//			Serial.print(F("ACC: "));
//			v3 = Serial.parseFloat();
//			Serial.println(v3);
//			Serial.print(F("DEC: "));
//			v4 = Serial.parseFloat();
//			Serial.println(v4);
//			Serial.print(F("Vmax: "));
//			v5 = Serial.parseFloat();
//			Serial.println(v5);
//			Serial.print(F("Vfinish: "));
//			v6 = Serial.parseFloat();
//			Serial.println(v6);
//
//			testTraj(v1,v2,v3,v4,v5, v6);
//
//			break;
//
//

		default:
			break;
		}

	}
	else{
		myReel.setMotorPWM(Brake,1024);


//		ccc++;
//		if(ccc > 1000){
//			myRod.printAll();
//			ccc = 0;
//
//		};



	}


}

void ISRwrapper()
{
	myRod.CHMD_Timer_ISR();

	//Put go to here?

}



void encISRwrapper()
{
	myRod.Encoder_ISR();

}

void ISRkillswitch()
{
	killvar = 1;
	//Serial.println("KILL!!!!");
	myRod.resetVPID();
	myRod.gotoPosition(myRod.getPositionCurrent());
	delay(1000000);

}


void blinkLED(void)
{
  if (ledState == LOW) {
    ledState = HIGH;
   // enableSolenoid();
  } else {
    ledState = LOW;
    //disableSolenoid();
  }
  digitalWrite(led, ledState);
}

//void CAST(double acc, double dec, double speed)
//{
//	//Save old acel profile
//	double oacc = myRod.accel;
//	double odec = myRod.decel;
//	double ovel = myRod.velocity_max;
//
//	//Prepare to cast
//	while(!myRod.gotoPosition(caststart));
//	delay(4000);
//	myRod.enableSolenoid();
//
//	// Set motion profile for forward cast.
//	myRod.setMotionProfile(acc, dec, speed);
//	delay(1000);
//	myRod.gotoPosition(castfinish);
//	while(myRod.getPositionCurrent() <= castrelease) delayMicroseconds(10);
//	myRod.disableSolenoid();
//
//	while(!myRod.at_position_target) delay(15);
//
//	//Set new accel profile:
//	myRod.setMotionProfile(acc, decel, speed);
//
//	delay(2000);
//
//
//	//Restore old motion profile
//	myRod.setMotionProfile(oacc,odec,ovel);
//
//
//}

void CAST(double acc, double dec, double speed)
{
	//Save old acel profile
	double oacc = myRod.accel;
	double odec = myRod.decel;
	double ovel = myRod.velocity_max;

	//Prepare to cast
//	Serial.println('B');
	//PreCAST(caststart);
//	myRod.gotoPosition(caststart, accel, decel, velmax,0,1);
//	delay(1000);
//	Serial.println('C');
	myRod.enableSolenoid();
	delay(1000);


	/*** VPID Control ***/
	double ca = speed/2;
	double cd = speed/2;

	//Move with setposition, make sure to comment out other method
	//myRod.setMotionProfile(acc, dec, speed);
	//myRod.setMotionProfile(accel, decel, speed);
    myRod.encoder_enablelimit = 0; //KLUDGE to try to speed up detection.
	myRod.gotoPosition(castrelease+160, ca, cd, speed, speed);
//	Serial.println('X');
	while(myRod.getPositionCurrent() <= castrelease) delayMicroseconds(10);
	myRod.disableSolenoid();


//	Serial.println('Y');

//	while(!	myRod.gotoPosition(castrelease, castacc, decel, velmax, 0));
	myRod.gotoPosition(castfinish, ca/2, decel, velmax, 0,1);

	myRod.setMotionProfile(oacc, odec, ovel);
//	Serial.println('Z');

	//Wait and finish
    myRod.encoder_enablelimit = 1; //KLUDGE re-enable encoder.
    delay(2000);
	myRod.resetVPID();
	cursp = myRod.getPositionCurrent();


}

void PreCAST(long location)
{
	//while(!myRod.gotoPosition(caststart, accel, decel, velmax,0));
	myRod.gotoPosition(caststart, accel, decel, velmax,0);
	while(myRod.movingactive) delayMicroseconds(10);

}

void testTraj(long dis, double vc, double acc, double dec, double vm, double vf)
{
	unsigned int m1 = 0;
	myRod.disableVPIDpwm = 0;

	myRod.position_target = dis;
	myRod.position_current = 0;
	myRod.accel = acc;
	myRod.decel = dec;
	myRod.velocity_max = vm;
	myRod.velocity_current = vc;
	myRod.velocity_finish = vf;


	/** Note: takes about 400us to run setup trajectory **/
//	m1 = millis();
//	for (unsigned int ii = 0; ii <= 1000; ii++)
//	{
//		myRod.setupTrajectory();
//	}
//	Serial.print("1000xsetup: ");
//	Serial.print((millis()-m1));

	myRod.setupTrajectory();


	/**Takes about 30us to update trajectory next update value **/
//	m1 = millis();
//	for (unsigned int jj = 0; jj <= 10000; jj++)
//	{
//		for (unsigned int ii = 0; ii <= myRod.t_4; ii++)
//		{
//			myRod.movecounter = ii;
//			myRod.updateTrajectory();
//			//myRod.printAll();
//			//Serial.println();
//
//		}
//	}
//	Serial.print((millis()-m1));

	for (unsigned int ii = 0; ii <= myRod.t_4; ii++)
	{
		myRod.movecounter = ii;
		myRod.updateTrajectory();
		myRod.printAll();
		//Serial.println();

	}



	myRod.resetVPID();
	myRod.disableVPIDpwm = 0;

}

int fishStateMachine(char key, int state){
	int nextstate = 0;
	int exitwait = 0;
	char cc;

	if(key == 'a'){
		Serial.print("Puff Advance");
		switch(state){
			case 2: //Precast, retrieve complete
				nextstate = 8;
				Serial.println("Puff, 2");
				myRod.gotoPosition(caststart, accel, decel, velmax,0,1);
				break;
			case 8: //Cast start
				Serial.println("Puff, 8");
				//Cast here;
//				myRod.gotoPosition(castfinish, accel, decel, velmax,0,1);
//
				Serial.print(F("Hit lip switch to cast"));
				while (!exitwait) {
					if (Serial.available()) {
						cc = Serial.read();
						if (cc == 'p' && myRod.isEncoderSafe()) {
							Serial.println("CASTING!!!!!");
							nextstate = 4;
							CAST(castacc, castdec, castspeed);
							cursp = myRod.getPositionCurrent();


						} else {
							nextstate = 8;
							Serial.println(F("Encoder not safe! NO CAST"));
							Serial.print(F("EnVal = "));
							Serial.println(myRod.isEncoderSafe());
							Serial.println(cc);
						}
						exitwait = 1;
					}
				}

				break;
			case 4:  //Cast complete, retreive position
                nextstate = 2;
                Serial.println("Sip, 4");
                myRod.gotoPosition(reelfinish, accel, decel, velmax,0,1);

				break;
			case 11: //Hook position
				nextstate = 2;
				Serial.println("Puff, 11");
				myRod.gotoPosition(reelfinish, accel, decel, velmax,0,1);

				break;
			default:
				break;
		}
	}
	else if (key == 'c'){
		Serial.println("Sip Return");
		switch(state){
			case 2: //Precast, retrieve complete
				nextstate = 4;
				Serial.println("Sip, 2");
                myRod.gotoPosition(castfinish, accel, decel, velmax,0,1);
                break;
			case 8: //Cast start
				nextstate = 2;
				Serial.println("Sip, 8");
				myRod.gotoPosition(reelfinish, accel, decel, velmax,0,1);
				break;
            case 4:  //Hook!
                Serial.println("Sip, 4");
                nextstate = 11;
                //Hook here more violently
                myRod.gotoPosition(hooklocation,10,10, 15,0,0);
//				myRod.gotoPosition(hooklocation, accel, decel, velmax,0,1);
                myRod.setMotorPWM(Brake,1023);

				break;
			case 11: //Hook position
				nextstate = 4;
				Serial.println("Sip, 11");
				myRod.gotoPosition(castfinish, accel, decel, velmax,0,1);

				break;
			default:
				break;
		}

	}
	else {} //Do nothing, however shouldnt reach here.


	return nextstate;

}