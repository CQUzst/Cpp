/* =================================================================================
File name:  HVBLDC_Sensored-Settings.H                     

Description: Incremental Build Level control file.

=================================================================================  */
#ifndef PROJ_SETTINGS_H

//------------------------------------------------------------------------------

#define PI 3.14159265358979

// Define the system frequency (MHz)
#define SYSTEM_FREQUENCY 150

// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 20
#define PWM_FREQUENCY 20

// This machine parameters are based on BLDC motor
// Define the BLDC motor parameters 
#define RS 	2.35    	           	// Stator resistance (ohm)
#define RR   			               	// Rotor resistance (ohm) 
#define LS   	0.007	      			// Stator inductance (H) 
#define LR   			  				// Rotor inductance (H) 	
#define LM   			   				// Magnatizing inductance (H)
#define POLES  2						// Number of poles

// Define the base quantites
#define BASE_VOLTAGE    236	        // Base peak phase voltage (volt)
#define BASE_CURRENT    10             // Base peak phase current (amp)
#define BASE_TORQUE         		    // Base torque (N.m)
#define BASE_FLUX       	       	    // Base flux linkage (volt.sec/rad)
#define BASE_FREQ        100           // Base electrical frequency (Hz)

#endif

