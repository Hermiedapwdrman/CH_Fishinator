/*
 * CHMotorDriver.cpp
 *
 *  Created on: Jun 29, 2015
 *      Author: patrick
 */

#include "CHMotorDriver.h"
#include <Arduino.h>


CHMotorDriver::CHMotorDriver(unsigned char * cb, CHMotorChannel mchan, bool enableencoder,
		unsigned long pwmperiod, unsigned long PIDperiod):
		VPID(pwmperiod, PIDperiod)
{

	pcontrolbyte = cb;

	if(mchan == channel1)
	{
		//motorpwmpin = 9;
		bB = sr1A;
		bA = sr1B;
		ppA = M1Ap;
		ppB = M1Bp;
	}
	else if(mchan == channel2)
	{
		//motorpwmpin = 10;
		bB = sr2A;
		bA = sr2B;
		ppA = M2Ap;
		ppB = M2Bp;
	}
	else {}

	encoderinuse = enableencoder;
	encoder_enablelimit = 1;
	encoder_maxlimit = 1250;
	encoder_minlimit = -1250;

	disableVPIDpwm = 0;

	eepromcounter = 0;




}


CHMotorDriver::~CHMotorDriver()
{

}


bool CHMotorDriver::begin()
{


	if(encoderinuse)
	{
		pinMode(encApin, INPUT);
		pinMode(encBpin, INPUT);
		pinMode(encXpin, INPUT);

//		SetPIDTunings(6,22,0.5,0,0,0);
//		setMotionProfile(1, 0.3, 10);

		beginVPID();
	}
	else{}

	pinMode(SOLp, OUTPUT);
	pinMode(M1Ap, OUTPUT);
	pinMode(M1Bp, OUTPUT);
	pinMode(M2Ap, OUTPUT);
	pinMode(M2Bp, OUTPUT);

	//Enable pins for DRB8701 and set to enable
	pinMode(M1_sleep,OUTPUT);
	pinMode(M2_sleep,OUTPUT);
	digitalWrite(M1_sleep, 1); //Not sleep so enable
	digitalWrite(M2_sleep, 1); //Enable

	//Setup Pwm Registers
	analogWriteResolution(10); //Set PWM 10 bit (0-1023) resolution
	unsigned int freq = 1000000/mypwmperiod;
	Serial.println(freq);
	//analogWriteFrequency(SOLp,freq);  //Set PWM frequency to timer interrupt period
	analogWriteFrequency(M1Ap,freq);
	analogWriteFrequency(M1Bp,freq);
	analogWriteFrequency(M2Ap,freq);
	analogWriteFrequency(M2Bp,freq);



	//SPI.begin();
	//SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

//	if(encoderinuse)
//	{
//		//Read the eeprom to determine position.
//		eepromaddress_lsb = 0;
//		position_current = CHMD_initEEPROM();
//	}

	setMotorPWM(Brake,1023);

	return 1;
}



void CHMotorDriver::Encoder_ISR()
{
	//unsigned int B = ((PINC & 0x08) >> 4); //NOTE: CHANCE
	unsigned char B = digitalRead(encBpin);  //An effort to speed up digital read is below
	//B = ((PIND & 0b00001000) >> 3);

	if (B == 1)  //Encoder is moving forward
	{
		position_current++;
	}
	else if (B == 0)  //Encoder is moving backwards
	{
		position_current--;
	}

	if(encoder_enablelimit)
	{
		if((position_current > encoder_maxlimit) || (position_current < encoder_minlimit))
		{
#warning "IS THIS OKAY!, will this result in bad behavior?"
			if(movingactive)  //If a move is happening
			{
				//And the targett is outside the limits, then prevent
				if((position_target > encoder_maxlimit) || (position_target < encoder_minlimit))
				{
					setMotorPWM(Brake, 1023);
					resetVPID();
				}
			}
			else  //You are not in an active move and need to stop the rod
			{
				setMotorPWM(Brake, 1023);
				resetVPID();
			}

		}
	}

}



//void CHMotorDriver::writeSR()
//{
//	bitClear(PORTD,7);
//	SPI.transfer(*pcontrolbyte);
//	bitSet(PORTD,7);
//
//}

//void CHMotorDriver::updateCB(CHdirection dir)
//{
//	//Somepoint check for a change, if no change then do nothing
//
//	unsigned char cc = *pcontrolbyte;
//	//Not if using single motor class then bits need to be remapped
//
//	switch(dir)
//	{
//	case Forward:
//		bitClear(*pcontrolbyte, bA);
//		bitSet(*pcontrolbyte, bB);
//		break;
//	case Reverse:
//		bitSet(*pcontrolbyte, bA);
//		bitClear(*pcontrolbyte, bB);
//		break;
//	case Brake:
//		bitSet(*pcontrolbyte, bA);
//		bitSet(*pcontrolbyte, bB);
//		break;
//	case Neutral:
//		bitClear(*pcontrolbyte, bA);
//		bitClear(*pcontrolbyte, bB);
//		break;
//	default:
//		return;
//
//	}
//
//	//And write shift register if the control has changed
//	if(cc != *pcontrolbyte) writeSR();
//	//writeSR();
//
//}


void CHMotorDriver::enableSolenoid()
{
	digitalWrite(SOLp,0);
	//analogWrite(SOLp,512);
	return;
}
void CHMotorDriver::disableSolenoid()
{
	digitalWrite(SOLp,1);
	//analogWrite(SOLp,1);
	return;
}

bool CHMotorDriver::gotoPosition(long setpoint, double accel_cs2,
		double decel_cs2,  double velmax_cs, double vel_finish, bool blocking)
{
	//Set motion profile, then call setpoint function
	setMotionProfile(accel_cs2, decel_cs2, velmax_cs);
	velocity_finish = vel_finish;
	gotoPosition(setpoint);
	if(blocking)
	{
		while(movingactive) delayMicroseconds(10);
	}
	return 1;

}

bool CHMotorDriver::gotoPosition(long setpoint)
{
	//Set new target, disable at setpoint flag, and return 0;
	//Actual movement will be done by ISR.
	//If the trajectory target and target position are not equql, we have not setup trajectory
	if(abs(position_target - setpoint) > position_threshold )
	{
		setPositionTarget(setpoint);
		at_position_target = 0;
	}

	return at_position_target;


}

bool CHMotorDriver::setMotorPWM(CHdirection dir, unsigned int pwmval)
{

	unsigned char cc = *pcontrolbyte; //Save previous value

	switch(dir)
	{
	case Forward:
		bitClear(*pcontrolbyte, bA);
		bitSet(*pcontrolbyte, bB);
		digitalWrite(ppA, 0);
		analogWrite(ppB, pwmval);
		break;
	case Reverse:
		bitSet(*pcontrolbyte, bA);
		bitClear(*pcontrolbyte, bB);
			digitalWrite(ppA, 1);
		analogWrite(ppB, pwmval);
		break;
	case Brake:
		bitSet(*pcontrolbyte, bA);
		bitSet(*pcontrolbyte, bB);
			digitalWrite(ppA, 0);
		analogWrite(ppB, 0);
		break;
	case Neutral:
		bitClear(*pcontrolbyte, bA);
		bitClear(*pcontrolbyte, bB);
			digitalWrite(ppA, 0);
		analogWrite(ppB, 0);
		break;
	default:
		break;

	}

//	//And write shift register if the control has changed
//	if(cc != *pcontrolbyte ){}
//
//	switch(dir)
//	{
//	case Forward:
//		pwm(bA, pwmval);
//		pwm(bB, 0);
//		break;
//	case Reverse:
//		pwm(bA, 0);
//		pwm(bB, pwmval);
//		break;
//	case Brake:
//		pwm(bA, 1024);
//		pwm(bB, 1024);
//		break;
//	case Neutral:
//		pwm(bA, 0);
//		pwm(bB, 0);
//		break;
//	default:
//		return 0;
//
//	}

	return 1;
}

//bool CHMotorDriver::setMotorPWM(CHdirection dir, unsigned int pwmval)
//{
//	//disablePwm(motorpwmpin);
//	updateCB(Brake);
//	pwm(motorpwmpin,1023);
//	//delayMicroseconds(1);  //Wait for transients with FETS switching
//	updateCB(Neutral);
//	//delayMicroseconds(1);  //Wait for transients with FETS switching
//	updateCB(dir);
//	pwm(motorpwmpin,pwmval);
//	return 1;
//
//}


bool CHMotorDriver::findEncoderZero()
{
	unsigned int cc = 0;
//	int dest = 0;

//	if(position_current < 0)  // Rod is currently behind index, move forward to find it
//	{
//		//char adir = Reverse;
//		dest = 1;
//	}
//	else if (position_current > 0) //Rod is ahead of index
//	{
//		//char adir = Forward;
//		dest = -1;
//	}
  	encoder_enablelimit = 0;
	//printAll();
	setPositionCurrent(0);
	//printAll();
	resetVPID();
//
	//printAll();
//	gotoPosition(dest*500, accel, decel, velocity_max, 0);
	setMotionProfile(0.3,0.2,2.5);
	Compute();
	delay(2*mypwmperiod);
	Compute();
	delay(2*mypwmperiod);


	//gotoPosition(-1000, accel, decel, velocity_max, 0);
	gotoPosition(-1000, 0.3, 0.2, 2.5, 0);
	//printAll();

	while(!cc)
	{
		cc = pulseIn(encXpin, HIGH);
		delayMicroseconds(10);
	}
	setPositionCurrent(0);
	//gotoPosition(position_current);

	//gotoPosition(dest*35,accel, decel, velocity_max, 0);
	resetVPID();

	encoder_enablelimit = 1;
	return 1;

}


void CHMotorDriver::CHMD_Timer_ISR()
{
	//Run VPID interrupt.
	VPID_ISR();

	//Run compute, update PWMs if needed
	if(VPIDoverflow)
	{
		if(!disableVPIDpwm)
		{
			at_position_target = computeVPID();
			double pwmval = getPWM();
			if(pwmval > 0) setMotorPWM(Forward,pwmval);
			else if (pwmval < 0) setMotorPWM(Reverse,abs(pwmval));
			else{
				//setMotorPWM(Brake, 1023);
				setMotorPWM(Neutral, 0);

			}
		}


		//setMotorPWM(Brake, 1020);
	}

//	//If you have been at the same position for 5 seconds write eeprom.
//	if(at_position_target)
//	{
//		if (eepromcounter > 350)
//		{
//			CHMD_writeEEPROM();
//			eepromcounter = 0;
//		}
//		else eepromcounter++;
//
//	}

}


/*********************************************************************
 * EEPROM write and init functions, the purpose of these is to help with
 * index localization/
 *
 * Where for a EEPROM address 0xmmnn; we will store the encoder value in
 * each nn = 1:255 and the lsB of the address (nn) is stored at 0xmm00.
 * mm = is a constanst defined in the .h file and can be changed at reproggram.
 *
 *
 *******************************************************************/

//int CHMotorDriver::CHMD_initEEPROM(char upperbyte)
int8_t CHMotorDriver::CHMD_initEEPROM()
{
	uint16_t ii = 0;
	uint8_t cc = 0;
	//First read the lsb from the address storage register
	ii = (eepromaddress_msb << 8);
	cc = EEPROM.read(ii);
	delay(5);

	Serial.print(F("storadd: "));
	Serial.print(ii, HEX);
	Serial.print(F(" lsb: "));
	Serial.print(cc, HEX);

	//Increment and rewrite the address storage register
	if(cc == 0xFF) eepromaddress_lsb = 1;  //NOTE: CANNOT BE 0, thats the address storage byte!
	else eepromaddress_lsb = cc+1;
	EEPROM.write(ii,eepromaddress_lsb);
	delay(5);

	Serial.print(F(" storadd: "));
	Serial.print(ii, HEX);
	Serial.print(F(" newlsb: "));
	Serial.print(eepromaddress_lsb, HEX);

	//Read the encoder value register and return the value
	ii = (eepromaddress_msb << 8) + cc;
	cc = EEPROM.read(ii);
	delay(5);

	Serial.print(F(" encadd: "));
	Serial.print(ii, HEX);
	Serial.print(F(" cp: "));
	Serial.print(cc);

	//Finally write back the encoder value to the new storage address.
	ii = (eepromaddress_msb << 8) + eepromaddress_lsb;
	EEPROM.write(ii,cc);
	delay(5);

	Serial.print(F(" newencadd: "));
	Serial.println(ii, HEX);

	return cc;  //Return the address value

}


//bool CHMotorDriver::CHMD_writeEEPROM()
//{
//
//	char cc = 0;
//	if(position_current < 0) cc = -1;
//	else if (position_current >= 0 ) cc = 1;
//	else cc = 0;
//	EEPROM.update(((eepromaddress_msb << 8)+ eepromaddress_lsb), cc);
//	delay(3);
//	return 1;
//
//}


