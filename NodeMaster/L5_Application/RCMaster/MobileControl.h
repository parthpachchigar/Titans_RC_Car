/*
 * MobileControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_MOBILECONTROL_H_
#define L5_APPLICATION_RCMASTER_MOBILECONTROL_H_

#include "NodeControl.h"

class MobileControl : public NodeControl {
public:
	MobileControl();
	virtual ~MobileControl();
	void setName(char* name);
};

#endif /* L5_APPLICATION_RCMASTER_MOBILECONTROL_H_ */
