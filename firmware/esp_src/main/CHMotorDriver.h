/*
 * CHMotorDriver.h
 *
 *  Created on: Jun 29, 2015
 *      Author: patrick
 */

#ifndef CHMOTORDRIVER_H_
#define CHMOTORDRIVER_H_

#include "VPID.h"
#include <EEPROM.h>
#include <SPI.h>

enum CHdirection {
	Forward,
	Reverse,
	Brake,
	Neutral
};

enum CHMotorChannel{
	channel1,
	channel2
};


class CHMotorDriver: public VPID
{
public:
	CHMotorDriver(unsigned char * cb, CHMotorChannel mchan, bool enableencoder,
			unsigned long pwmperiod, unsigned long PIDperiod);
	//CHMotorDriver(unsigned char *cb, CHMotorChannel mchan, unsigned long pwmperiod, unsigned long PIDperiod);
	~CHMotorDriver();
	//Function to call to init and set up everything
	bool begin();
//	bool findEncoderZero(CHdirection dir, int speed);
	bool findEncoderZero();


	//ControlFunctions
	void enableSolenoid();
	void disableSolenoid();
//	bool gotoPosition(long setpoint, double accel_cs2,
//			double decel_cs2,  double velmax_cs,  double vel_finish);
	bool gotoPosition(long setpoint, double accel_cs2,
			double decel_cs2,  double velmax_cs, double vel_finish, bool blocking = 0);

	bool gotoPosition(long setpoint);
	bool setMotorPWM(CHdirection dir, unsigned int pwmval);
	void setEncoderLimits(int min, int max){encoder_maxlimit = max; encoder_minlimit = min;}

	void enableEncoderLimits() {encoder_enablelimit = 1;}
	void disableEncoderLimits() {encoder_enablelimit = 0;}
	bool isEncoderSafe() { return encoder_enablelimit;}
	//Public subroutines not meant to be called normally
	void Encoder_ISR();
	void CHMD_Timer_ISR();

	volatile bool disableVPIDpwm;  //Used to disable PID loop control/used in casting

	//Save Value to EEPROM -
	// Return function which can increment the address being written to, it can be
	// a byte so you can increment which address to write, so 256 rotating addressess. TO save eeprom.
//	int  CHMD_initEEPROM(char upperbyte);
	int8_t  CHMD_initEEPROM();
//	bool CHMD_writeEEPROM(int idx, uint8_t val);
	bool CHMD_writeEEPROM();
	//int8_t CHMD_readEEPROM() {return EEPROM.read(((eepromaddress_msb << 8)+ eepromaddress_lsb));}
	unsigned int eepromcounter;

	//Might be converted to a changing value
	const static uint8_t eepromaddress_msb = 0xa5;
	uint8_t eepromaddress_lsb;

//private:
	volatile unsigned char *pcontrolbyte;//Pointer to control byte, so mulitple things can share
	bool encoderinuse;



	void writeSR();
	void updateCB(CHdirection dir);

	//Bit offfsets for control byte
	const static char srSOL = 1;
	const static char sr2B = 2;
	const static char sr2A = 3;
	const static char sr1B = 4;
	const static char sr1A = 5;

	//Defines for output pins
	const static char SOLp = 2;
	const static char M1Ap = 6;
	const static char M1Bp = 5;
	const static char M2Ap = 4;
	const static char M2Bp = 3;
	const static char encApin = 8;
	const static char encBpin = 9;
	const static char encXpin = 7;
	const static char M1_sleep = 15;
	const static char M2_sleep = 14;


	char bA,bB;
	char ppA, ppB;

	bool encoder_enablelimit;
	int encoder_maxlimit;
	int encoder_minlimit;

	//Might be converted to a changing value
//	const static uint8_t eepromaddress_msb = 0xa5;
//	uint8_t eepromaddress_lsb;


};




#endif /* CHMOTORDRIVER_H_ */
