/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * This contains the period callback functions for the periodic scheduler
 *
 * @warning
 * These callbacks should be used for hard real-time system, and the priority of these
 * tasks are above everything else in the system (above the PRIORITY_CRITICAL).
 * The period functions SHOULD NEVER block and SHOULD NEVER run over their time slot.
 * For example, the 1000Hz take slot runs periodically every 1ms, and whatever you
 * do must be completed within 1ms.  Running over the time slot will reset the system.
 */

#include "stdint.h"
#include "io.hpp"
#include "periodic_callback.h"
#include "can.h"
#include "stdio.h"
#include "utilities.h"
#include "string.h"
#include "lpc_pwm.hpp"
#include "lpc_sys.h"
#include "../source/LCD_Display/LCD_Display_includes.hpp"

SENSOR_DATA_t sensor_data = { 0 };
COMPASS_Data_t compass_heading = { 0 };
// PWM FREQUENCY USED FOR STEER AND SPEED 100 Hz
// TIME PERIOD : 10 ms
// FOR SPEED_LEVEL_FAST DUTY CYCLE : 20% of TIME PERIOD

#define SPEED_LEVEL_STOP 				14
#define SPEED_LEVEL_SLOW 				15.8
#define SPEED_LEVEL_MEDIUM 				18
#define SPEED_LEVEL_FAST 				20
#define REVERSE							12.8	//Or 13.00
#define MOTOR_CONTROLLER_INIT			14.5	//Duty Cycle Required for DC MOTOR INIT

#define STEERING_POS_LEFT				10
#define STEERING_POS_SLIGHT_LEFT		12
#define STEERING_POS_CENTER				14
#define STEERING_POS_SLIGHT_RIGHT		16
#define STEERING_POS_RIGHT				18

int reverse_count = 0;


PWM carSteer(PWM::pwm3, 8);
PWM carSpeed(PWM::pwm2, 8);

const MASTER_HB_t master_mia_msg = {0};
const uint32_t master_mia_ms = 10;

const uint32_t            MASTER_HB__MIA_MS = 300;
const MASTER_HB_t         MASTER_HB__MIA_MSG = { 8 };

MASTER_HB_t master_can_msg = { 0 };


bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}

//added this line in dev branch

/// This is the stack size used for each of the period tasks (1Hz, 10Hz, 100Hz, and 1000Hz)
const uint32_t PERIOD_TASKS_STACK_SIZE_BYTES = (512 * 4);

/**
 * This is the stack size of the dispatcher task that triggers the period tasks to run.
 * Minimum 1500 bytes are needed in order to write a debug file if the period tasks overrun.
 * This stack size is also used while calling the period_init() and period_reg_tlm(), and if you use
 * printf inside these functions, you need about 1500 bytes minimum
 */
const uint32_t PERIOD_DISPATCHER_TASK_STACK_SIZE_BYTES = (512 * 3);

/// Called once before the RTOS is started, this is a good place to initialize things once
bool period_init(void)
{
    CAN_init(can1,100,3,3,NULL,NULL);
    //CAN_bypass_filter_accept_all_msgs();
    const can_std_id_t slist[]  = { CAN_gen_sid(can1, 0x010), CAN_gen_sid(can1, 0x011),// Sensor data
    								CAN_gen_sid(can1, 0x020),CAN_gen_sid(can1, 0x021), //master HB
									CAN_gen_sid(can1, 0x042),CAN_gen_sid(can1, 0x043)};// Compass heading

    CAN_setup_filter(slist, 4, NULL, 0, NULL, 0, NULL, 0);
    CAN_reset_bus(can1);
    carSpeed.set(MOTOR_CONTROLLER_INIT);
    carSteer.set(STEERING_POS_CENTER);
    return true; // Must return true upon success
}

/// Register any telemetry variables
bool period_reg_tlm(void)
{
    // Make sure "SYS_CFG_ENABLE_TLM" is enabled at sys_config.h to use Telemetry
    return true; // Must return true upon success
}


/**
 * Below are your periodic functions.
 * The argument 'count' is the number of times each periodic task is called.
 */


void period_1Hz(uint32_t count)
{
  //  LE.toggle(1);
}

MASTER_HB_t master_hb_msg = { 0 };
MOTOR_STATUS_t motor_msg={0};
bool reverse = false;

void period_10Hz(uint32_t count)
{
	can_msg_t can_msg;

	        // Empty all of the queued, and received messages within the last 10ms (100Hz callback frequency)
	       while (CAN_rx(can1, &can_msg, 0))
	        {
	                 // Form the message header from the metadata of the arriving message
	    	   	dbc_msg_hdr_t can_msg_hdr;
	            can_msg_hdr.dlc = can_msg.frame_fields.data_len;
	            can_msg_hdr.mid = can_msg.msg_id;

	            // Attempt to decode the message (brute force, but should use switch/case with MID)

	            motor_msg.MOTOR_STATUS_speed_mph = 9.0;
	            if(can_msg_hdr.mid == 0x20)
	            {
	            	 	 dbc_decode_MASTER_HB(&master_hb_msg, can_msg.data.bytes, &can_msg_hdr);
//////
	            	 	 switch(master_hb_msg.MASTER_STEER_cmd)
	            	 	 {
	            	 	 	 case 0:
	            	 	 		 	 carSteer.set(STEERING_POS_LEFT);
	            	 	 		 	 break;
	            	 	 	 case 1:
	            	 	 		 	 carSteer.set(STEERING_POS_SLIGHT_LEFT);
	            	 	 		 	 break;
	            	 	 	 case 2:
	            	 	 		 	 carSteer.set(STEERING_POS_CENTER);
	            	 	 		 	 break;
	            	 	 	 case 3:
	            	 	 		 	 carSteer.set(STEERING_POS_SLIGHT_RIGHT);
	            	 	 		 	 break;
	            	 	 	 case 4:
	            	 	 		 	 carSteer.set(STEERING_POS_RIGHT);
	            	 	 		 	 break;
	            	 	 }


	            	 	 switch(master_hb_msg.MASTER_SPEED_cmd)
	            	 	 {
	            	 	 	 case 0:
	            	 	 		 	 carSpeed.set(SPEED_LEVEL_STOP);
	            	 	 		 	 LD.setNumber(master_hb_msg.MASTER_SPEED_cmd);
	            	 	 		 	reverse_count =0;
	            	 	 		 	 break;
	            	 	 	 case 1:
	            	 	 		 	 carSpeed.set(SPEED_LEVEL_SLOW);
	            	 	 		 	 LD.setNumber(master_hb_msg.MASTER_SPEED_cmd);
	            	 	 		 	 reverse_count =0;
	            	 	 		 	 break;
	            	 	 	 case 2:
	            	 	 		 	 carSpeed.set(SPEED_LEVEL_MEDIUM);
	            	 	 		 	 LD.setNumber(master_hb_msg.MASTER_SPEED_cmd);
	            	 	 		 	 reverse_count =0;
	            	 	 		 	 break;
	            	 	 	 case 3:
	            	 	 		 	 carSpeed.set(SPEED_LEVEL_FAST);
	            	 	 		 	 LD.setNumber(master_hb_msg.MASTER_SPEED_cmd);
	            	 	 		 	 reverse_count =0;
	            	 	 		 	 break;
	            	 	 	 case 4:

	            	 	 		 	 LD.setNumber(master_hb_msg.MASTER_SPEED_cmd);
	            	 	 		 	 if(reverse_count ==  0)
	            	 	 		 	 {
	            	 	 		 		 carSpeed.set(MOTOR_CONTROLLER_INIT);
	            	 	 		 	 }
	            	 	 		 	 if(reverse_count == 5)
	            	 	 		 	 {
	            	 	 		 		 carSpeed.set(REVERSE);
	            	 	 		 	 }
	            	 	 		 	 if(reverse_count == 10)
	            	 	 		 	 {
	            	 	 		 		 carSpeed.set(MOTOR_CONTROLLER_INIT);
	            	 	 		 	 }
	            	 	 		 	 if(reverse_count == 15)
	            	 	 		 	 {
	            	 	 		 		 carSpeed.set(REVERSE);
	            	 	 		 	 }
	            	 	 		 	 if(reverse_count == 50)
	            	 	 		 	 {
	            	 	 		 		 carSpeed.set(MOTOR_CONTROLLER_INIT);
	            	 	 		 	 }

	            	 	 		 	reverse_count++;

	            	 	 		 	 break;
	            	 	 }
/////
	            dbc_encode_and_send_MOTOR_STATUS(&motor_msg);
	           }
	            /*Sensor data for LCD display*/
	           else if(can_msg_hdr.mid == 0x10)
	            {
	            	dbc_decode_SENSOR_DATA(&sensor_data, can_msg.data.bytes, &can_msg_hdr);
	            	LE.toggle(3);
	            }
	            /*Compass heading for LCD display
	           else if(can_msg_hdr.mid == 0x42)
	           {
	        	dbc_decode_COMPASS_Data(&compass_heading,can_msg.data.bytes, &can_msg_hdr);
	        	LE.toggle(4);
	           }*/
	        }

	       /*Handle MIA for master heart beat*/
	        if(dbc_handle_mia_MASTER_HB(&master_can_msg,10))
	       	{
	       		carSpeed.set(SPEED_LEVEL_STOP);
	       		LD.setNumber(88);
	       	}

	        if(CAN_is_bus_off(can1))
	        {
	        	LD.setNumber(80);
	            CAN_reset_bus(can1);
	        }
}

void period_100Hz(uint32_t count)
{
   // LE.toggle(3);
}

// 1Khz (1ms) is only run if Periodic Dispatcher was configured to run it at main():
// scheduler_add_task(new periodicSchedulerTask(run_1Khz = true));
void period_1000Hz(uint32_t count)
{
  //  LE.toggle(4);
}
