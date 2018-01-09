/*
 * Motor_VPID.cpp
 *
 *  Created on: Jun 22, 2015
 *      Author: patrick
 */


#include <Arduino.h>
#include "VPID.h"

/*
 *
 */
VPID::VPID(unsigned long pwmperiod, unsigned long PIDperiod) :
PID(&inputPID, &outputPID, &setpointPID, kp_v, ki_v, kd_v, DIRECT)
{

	unsigned long cc = 0;
	VPIDinterruptcounter = 0;
	mypwmperiod = pwmperiod;
	myPIDsubsample = PIDperiod/pwmperiod;

	//Ensure integer number interrupts, should warn of this...
	myPIDperiod = pwmperiod*myPIDsubsample;
	VPIDoverflow = 0;
	VPIDoverflowreset = 1; //Set this as success so it will run once.
	at_position_target = 0;

	//Set up timerone sub-classes
	//TimerOne::initialize(pwmperiod);

	cc = ((myPIDperiod/1000)-1);
	SetSampleTime(cc);
	SetOutputLimits(-1023,1023);
	SetMode(AUTOMATIC);

	//Write zero to all variables, as to not invoke some strange behavior
	kp_v = 0.0;
	ki_v = 0.0;
	kd_v = 0.0;
	kp_pos = 0.0;
	inputPID = 0.0;
	setpointPID = 0.0;
	outputPID = 0.0;

	//Zero velocity estimation variables
    t_1 = 0;
	t_2 = 0;
	t_3 = 0;
	t_4 = 0;

	//VPID loop variables
	position_target = 0; //The current target location
	position_setpoint = 0; //The current control loop setpoint
	position_current = 0; //The current encoder position
	position_last = 0;  //The last encoder position
    position_threshold = 10;  //How close to target to accept current position
    position_distance = 0;  //Current distance between target and current
    position_trajectory_target = 0;

	velocity_setpoint = 0.0;  //Current control loop set velocity
	velocity_current = 0.0;  //Current velocity measured in ISR
	velocity_direction = 0; //Current velocity direction;
	velocity_max = 0.0;
	velocity_atsetup = 0;
	velocity_finish = 0;
	accel_signmag = 0;
	accel_signmag = 0;

	tau2_slope = 0;
	tau4_slope = 0;
	tau4_constant = 0;


	accel= 0.0;
	decel = 0.0;

    movecounter = 0;
    movingactive = 0;

    //Non-essential variables useful for troubleshooting
    distanceerror = 0;

}

VPID::~VPID()
{
}


/*
 *
 */
void VPID::beginVPID()  //Why do I need to call this?
{
	//TimerOne::initialize(mypwmperiod);
	//TimerOne::start();
}

/*******************************************************************************\
 * Control and VPID calculation functions AKA: Meat and potatoes
 *
 *******************************************************************************/



/*
 *
 */
void VPID::VPID_ISR(void)
{
	VPIDinterruptcounter++;

	//We have interrupted enough times that enough time has elapsed to re-calc
	//  PID control values
	if(VPIDinterruptcounter >= myPIDsubsample)
	{
		//Set ISR flags
		VPIDinterruptcounter = 0;  //Reset counter
		VPIDoverflow = 1;

		//Save position variables
		velocity_current = position_current - position_last;
		//position_distance = abs(position_target - position_current);
		position_last = position_current;
		//Update PID loop settings.
		//If moving then update trajectory and increment counter.
		if(movingactive)
		{
			movecounter++;
			//Call compute function?
		}
		else
		{
			movecounter = 0;
		}

	}
}


/*
 *
 */
bool VPID::computeVPID()
{
	//long err = position_target - position_last;
	//position_distance = abs(position_target - position_current);
	bool movedone = ((abs(position_target - position_current)) <= position_threshold);
	//bool newtarget = (position_trajectory_target != position_target);


	/*We have not started a move, but the distance is greater than the threshold
	 * than we are beginning a new move. Calculate the expexted trajectory.
	 *
	 */
	//If a new position_target has been set(by gotoposition) then we can resetup trajectory
	if(position_trajectory_target != position_target)
	{
		//Attempt to calculate a new trajectroy if fail, than nothing.
		if(setupTrajectory())
		{
			movecounter = 0;  //Reset move counter
			movingactive = 1; //Set active move to 1
			SetMode(1); //Set mode to active.
		}

		//Serial.println(F("Upd"));
		return 0;
	}


	//Check for overlow, and not at setpoint as indicated above so update
	if(VPIDoverflow)
	{
	   VPIDoverflow = 0; //Clear overflow flag
	   if(movingactive)
	   {
			updateTrajectory();


			/*Position PID controller using velocity trajectory to update Position */
			inputPID = position_last;
			setpointPID = position_setpoint;
			Compute();

			/*Cascaded P-PI controller using position and velocity calculation. */
//			setpointPID = velocity_setpoint + kp_pos*(position_setpoint - position_current);
//			inputPID = velocity_current;
//			Compute();

			//Serial.println(velocity_current);
			//printAll();
	   }

		VPIDoverflowreset = 1;  //Must fqwelag this after updating vars
	}

	//If the target and the current position are equal, then stop move return success
	if(movedone && movingactive)
	{
		resetVPID();
		return movedone;
	}

	return movedone;
}

/*
 *
 */
void VPID::resetVPID()
{
	movingactive = 0;
    movecounter  = 0;
	position_target = position_current;
	position_trajectory_target = position_current;
	position_setpoint = position_current;
    inputPID = position_current;
    setpointPID = position_current;
    outputPID = 0.0;
    velocity_setpoint = 0;
    at_position_target = 1;
    SetMode(0);

}



/*
 *
 */
bool VPID::setupTrajectory()
{
	long estimateddistance = 0;
	position_distance = position_target - position_current;
	position_setpoint = position_current;
	position_trajectory_target = position_target;
	velocity_atsetup = velocity_current;

	//Determine if the target is forward or reverse w/r to current position
	if(position_distance < 0) //Reverse!
	{
		//Serial.print('n');
		velocity_max = -velocity_max;
		//accel = -accel;
		//decel = -decel;

	}

	//Determine what sign the accelerations and decelerations have to be.
	if (velocity_max > velocity_atsetup) accel_signmag = 1;
	else
	{
		accel_signmag = -1;
		//Serial.print('a');
	}

	if (velocity_max > velocity_finish) decel_signmag = -1;
	else
	{
		decel_signmag = 1;
		//Serial.print('d');
	}

	//Find the periods for the acceleration and deceleration
	tau2 = (double)accel_signmag*(velocity_max - velocity_atsetup)/accel;
	tau4 = (double)decel_signmag*(velocity_finish- velocity_max)/decel;

	//Estimate distance covered by acceleration and deceleration
	estimateddistance = ((velocity_max + velocity_atsetup)*tau2
			+ (velocity_max + velocity_finish))/2;

	//If its greater than the move distance, then we need to recalc a new max velocity
	if(abs(estimateddistance) > abs(position_distance))
	{
		//Serial.print('^');
		//tau3 = 1;  //One extra period to help it get closer
		tau3 = 0;

		double velmax_sq = (2*position_distance + ((accel_signmag*velocity_atsetup*velocity_atsetup)/accel)
						- ((decel_signmag*velocity_finish*velocity_finish)/decel))
						*((decel*accel)/(accel_signmag*decel - decel_signmag*accel));
		velocity_max = sqrt(velmax_sq);

		if(position_distance < 0) velocity_max = -velocity_max;

		tau2 = (double)accel_signmag*(velocity_max - velocity_atsetup)/accel;
		tau4 = (double)decel_signmag*(velocity_finish- velocity_max)/decel;

	}
	//If not greater, than calculate the value of Tau3
	else
	{
		//Serial.print('~');
		tau3 = (2*position_distance
				- (velocity_atsetup + velocity_max)*tau2
				- (velocity_finish + velocity_max)*tau4) /(2*velocity_max);

		//tau3++; //Add one extra sample period to help it get closer to target.

	}


	//Check estimated distance
	estimateddistance = ( ((velocity_atsetup + velocity_max)*tau2)
					+ (2*velocity_max*tau3)
					+ ((velocity_max + velocity_finish)*tau4))/2;
	//Find the error
	distanceerror = abs(estimateddistance) - abs(position_distance);

	t_1 = 0;
	t_2 = tau2;
	t_3 = tau2 + tau3;
	t_4 = tau2 + tau3 + tau4;

	//Calculate the tau4 constant to save time later
	tau2_slope = accel_signmag*accel;
	tau4_constant = (velocity_finish - decel_signmag*decel*t_4);
	tau4_slope = decel_signmag*decel;


	//Write PID variables to erase erata
	 inputPID = position_current;
	 setpointPID = position_current;
	 //outputPID = 0.0;


	//Test print all the trajectory estimation.
	if(0)
	{
		//Check once more to see what estimated distance is
		estimateddistance = ( ((velocity_atsetup + velocity_max)*tau2)
						+ (2*velocity_max*tau3)
						+ ((velocity_max + velocity_finish)*tau4))/2;
		//Find the error
		distanceerror = abs(estimateddistance) - abs(position_distance);



		Serial.print(F(" -- ESTIMATED TRAJECTORY -- "));
		Serial.print(F("  Dist: "));
		Serial.print(position_distance);
		Serial.print(F("  drem: "));
		Serial.print(distanceerror);
		Serial.print(F("  VelMax: "));
		Serial.print(velocity_max);
		Serial.print(F("  VelSetup: "));
		Serial.print(velocity_atsetup);
		Serial.print(F("  VelFinish: "));
		Serial.print(velocity_finish);
		Serial.print(F("  acc: "));
		Serial.print(accel);
		Serial.print(F("  dec: "));
		Serial.print(decel);
		Serial.print(F("  tau2: "));
		Serial.print(tau2);
		Serial.print(F("  tau3: "));
		Serial.print( tau3);
		Serial.print(F("  tau4: "));
		Serial.print(tau4);
		Serial.print(F("  t1: "));
		Serial.print(t_1);
		Serial.print(F("  t2: "));
		Serial.print(t_2);
		Serial.print(F("  t3: "));
		Serial.print(t_3);
		Serial.print(F("  t4: "));
		Serial.println(t_4);
	}

	return 1;

}


void VPID::updateTrajectory()
{

	if(movecounter == 0)
	{
		velocity_setpoint = velocity_current;
	}
	else if((movecounter > 0) && (movecounter <= t_2))
	{
		velocity_setpoint = tau2_slope*(double)movecounter + velocity_atsetup;
	}
	else if ((movecounter > t_2) && (movecounter < t_3))
	{
		velocity_setpoint = velocity_max; // *velocity_direction;
	}
	else if ((movecounter >= t_3 ) && (movecounter <= t_4))
	{
		velocity_setpoint = tau4_slope*(double)movecounter + tau4_constant;
	}
	else
	{
		velocity_setpoint = 0;
	}
	position_setpoint += velocity_setpoint;

}

/************************************************************************
 * Get and Set functions.  Function to get and set variables
 *
 *
 *********************************************************************/
void VPID::SetPIDTunings(double vkp,double vki,double vkd,
			double pkp,double pki,double pkd)
{
	kp_v = vkp;
	ki_v = vki;
	kd_v = vkd;
	kp_pos = pkp;
	//Write k values to PID class variables.
	SetTunings(kp_v,ki_v, kd_v);

}

/*
 *
 */
void VPID::setMotionProfile(double acc, double dec, double velm)
{
	accel = acc;
	decel = dec;
	velocity_max = velm;
}



/************************************************************************/
/* Print Function outputs all relevant variables in a concise-ish manner */
void VPID::printAll()
{

	//Current values
	Serial.print("MC-In-SP-OUT: ");
	Serial.print(movecounter);
	Serial.print(',');
	Serial.print(inputPID);
	Serial.print(',');
	Serial.print(setpointPID);
	Serial.print(',');
	Serial.print(outputPID);
	Serial.print(" || ");

	//Current values
	Serial.print("Pc-Pt-Ptt: ");
	Serial.print(position_current);
	Serial.print(',');
	Serial.print(position_target);
	Serial.print(',');
	Serial.print(position_trajectory_target);
	Serial.print(" || ");


	//Update trajectory values
	Serial.print(F("Vsp-Psp: "));
	Serial.print(velocity_setpoint);
	Serial.print(',');
	Serial.println(position_setpoint);



	//Serial.println("---------------");

}


