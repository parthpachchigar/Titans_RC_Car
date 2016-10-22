/*
 * coordinator.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include "coordinator.h"
#include "../_can_dbc/generated_can.h"
#include "can.h"
#include "io.hpp"
const uint32_t            SENSOR_HB__MIA_MS = 3000;
const SENSOR_HB_t         SENSOR_HB__MIA_MSG = { 8 };


// For the sake of example, we use global data storage for messages that we receive
SENSOR_HB_t sensor_can_msg = { 0 };
MOTOR_HB_t motor_can_msg = { 0 };


coordinator::coordinator() {
	// TODO Auto-generated constructor stub
	itsMotorNode  = new MotorControl();
	itsGeoNode    = new GeoControl();
	itsSensorNode = new SensorsControl();
	itsMobileNode = new MobileControl();

	itsMotorNode->setName();
	itsGeoNode->setName();
	itsSensorNode->setName();
	itsMobileNode->setName();
}



void coordinator::on1HzHearbeat(){
	getNodeStatus();

}
bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8])
{
    can_msg_t can_msg = { 0 };
    can_msg.msg_id                = mid;
    can_msg.frame_fields.data_len = dlc;
    memcpy(can_msg.data.bytes, bytes, dlc);

    return CAN_tx(can1, &can_msg, 0);
}
bool coordinator::sendHeartbeat(){
	MASTER_HB_t master_cmd ={0};
		master_cmd.MASTER_HEARTBEAT_cmd=3;

			    // This function will encode the CAN data bytes, and send the CAN msg using dbc_app_send_can_msg()
		dbc_encode_and_send_MASTER_HB(&master_cmd);
return true;
}
bool coordinator::getNodeStatus(){

	bool status_received = false;
    if(sendHeartbeat())
	printf("Hearbeat sent!\n");
    can_msg_t can_msg;
    uint32_t node_status_counter=0;
    if (CAN_rx(can1, &can_msg, 0))
       {
           // Form the message header from the metadata of the arriving message
    	  node_status_counter++;
           dbc_msg_hdr_t can_msg_hdr;
           can_msg_hdr.dlc = can_msg.frame_fields.data_len;
           can_msg_hdr.mid = can_msg.msg_id;
//enyer

           	//prinf();
           // Attempt to decode the message (brute force, but should use switch/case with MID)
           switch (can_msg_hdr.mid)
           {
             case MOTOR_MID:
                 dbc_decode_MOTOR_HB(&motor_can_msg, can_msg.data.bytes, &can_msg_hdr);
                 printf("Received status from Motor!:\n");
                 LD.setNumber(motor_can_msg.MOTOR_HEARTBEAT_cmd);
        	   break;
             case SENSORS_MID:
                 dbc_decode_SENSOR_HB(&sensor_can_msg, can_msg.data.bytes, &can_msg_hdr);
                 LD.setNumber(sensor_can_msg.SENSOR_HEARTBEAT_cmd);
              break;
           }




          // printf(" Received msg bytes %x\n",sensor_can_msg.SENSOR_HEARTBEAT_cmd);
           printf(" Received id %x\n",can_msg.msg_id);

       }
    dbc_handle_mia_SENSOR_HB(&sensor_can_msg, 10);

	return (node_status_counter > 0);// status_received;   TODO restore testing only
}

void coordinator::onStatusReceived(){
	printf("Message Received");
	//prinf();
	processAndSendOrder();
}
void coordinator::processAndSendOrder(){
	//TODO
		//
		printf("Master processing order \n");
		//call trajectory engine here TODO

		// then send the order
		if(itsMotorNode->sendOrder()){
			printf("Sending order\n");
		}
}
coordinator::~coordinator() {
	// TODO Auto-generated destructor stub
}

