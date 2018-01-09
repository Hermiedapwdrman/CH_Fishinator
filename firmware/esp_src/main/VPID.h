/*
 * Motor_VPID.h
 *
 *  Created on: Jun 22, 2015
 *      Author: patrick
 */

#ifndef VPID_H_
#define VPID_H_


#include <PID_v1.h>
//#include "PID_v1.h"
//#include "../components/Arduino-PID-Library/PID_v1.h"
//#include <TimerOne.h>


class VPID : public PID
//class VPID : public TimerOne
{
public:
	VPID(unsigned long pwmperiod, unsigned long PIDperiod);
	~VPID();

	void beginVPID();  //Must call to ensure timer is init'd correctly
	/*Update all PID variables, and trajectory variables. Can be called as often
	 * as wanted but will only generate new values after a PIDoverflow */
    bool computeVPID();  //Update all PID loops, and trajectories
	/* ISR which increments interrupt counter, after PID subsample occurences,
	 * it will record new position and velocity data */
	void VPID_ISR(void);

    //Get-Set functions
    //Get kp_v, kp_i,kp_d are called through the PID calls
	void SetPIDTunings(double vkp,double vki,double vkd,  //Write pkd variables
			double pkp,double pki,double pkd);
	void setMotionProfile(double acc, double dec, double velm);
	double getKpp(){return kp_pos;}
	inline double getAccel(){return accel;}
	double getDecel(){return decel;}
	double getVelocityMax(){return velocity_max;}
	void setPositionCurrent(long vv){position_current = vv;}
	double getPositionCurrent(){return position_current;}
	void setPositionTarget(long vv){position_target = vv;}
	double getPositionTarget(){return position_target;}
	double getPWM(){return outputPID;}
	void setPositionThreshold(unsigned char cc){position_threshold = cc;}

	//Temp kludge to reset VPID
	void resetVPID();


    //Print Function for debug
    void printAll();

    /*Public variables for VPID control loop */
    //TimerVariables
	unsigned long mypwmperiod; //PWM period in usec
	unsigned long myPIDperiod; //How often to compute new PID values
	unsigned int  myPIDsubsample; //Ratio of PWM period to PID period

	/*PID gain variables*/
	double kp_v, ki_v, kd_v;
	double kp_pos;

	//VPID loop variables
	double accel, decel;
	double velocity_max;

	//ISR flags, can use to check if something has happened.
	volatile bool VPIDoverflow; //Will be set after PISsubsample timer 1 interupt, cleared by VPIDcompute
	volatile bool VPIDoverflowreset;  //is set by VPID compute, not cleared ever...
	volatile bool at_position_target;

//protected:
	/********************************************
	 * Computation functions called in VPID control loop
	 *
	 ********************************************/

	/* Will generate all the timing variables and points for a trapazoid motion
	 * profile.*/
	bool setupTrajectory();
	/* Updates the setpoints for a the trapazoidal trajectory. Needs to be called
	 * at least once per PID overflow/PIDsubsample. */
	void updateTrajectory();


	/*****************************************************
	 * Private Variables for VPID loop control
	 ******************************************************/
	//VPID loop position variables
	volatile long position_target; //The current target location
	volatile double position_setpoint; //The current control loop setpoint
	volatile long position_current; //The current encoder position
	volatile long position_last;  //The last encoder position
    unsigned char position_threshold;  //How close to target to accept current position
    volatile long position_distance;  //Current distance between target and current
    volatile long position_trajectory_target;  //Current trajectory profile target destination

    //VPID velocity variables.
	double velocity_setpoint;  //Current control loop set velocity
	volatile double velocity_current;  //Current velocity measured in ISR
	volatile double velocity_atsetup;  //Holder var for the measured vel when a new move was initiated
	double velocity_finish;
	signed char velocity_direction; //Current velocity direction;
	signed char accel_signmag; //To determine if the acceleration is actually a decel
	signed char decel_signmag; //To determine if the acceleration is actually a decel

	//Precalculated variables for the move function.
	double tau2_slope;
	double tau4_slope;
	double tau4_constant;

	//Loop counter and flag
    volatile unsigned int movecounter;
    volatile bool movingactive;

    //Non-essential variables useful for troubleshooting
    double distanceerror;

	//PID loop variables
	double inputPID, setpointPID, outputPID;  //Outputs of MainPIDStage


	//Interrupt counter, ISR flags, and and ISR function
	volatile unsigned int VPIDinterruptcounter;  //Count number of timer 1 interrupts.

    //Number of samples and total sample variables
    //int ns_total, ns_reverse, ns_accel, ns_decel, ns_cv; //Number of samples for trapazoidal trajectory

    unsigned int tau2,tau3,tau4;
    unsigned int t_1, t_2, t_3, t_4;


};

#endif /* VPID_H_ */
