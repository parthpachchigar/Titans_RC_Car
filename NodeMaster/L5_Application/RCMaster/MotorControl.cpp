/*
 * MotorControl.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#include <RCMaster/MotorControl.h>


MotorControl::MotorControl() {
	// TODO Auto-generated constructor stub
}

MotorControl::~MotorControl() {
	// TODO Auto-generated destructor stub
}

bool MotorControl::sendOrder(){
	printf("Sending motor order!\n)");
	return true;
}

void MotorControl::setName(){
    strcpy(nodeName, "MotorNode");
}
