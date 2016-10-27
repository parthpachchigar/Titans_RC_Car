/*
 * SensorsControl.h
 *
 *  Created on: Oct 10, 2016
 *      Author: home
 */

#ifndef L5_APPLICATION_RCMASTER_SENSORSCONTROL_H_
#define L5_APPLICATION_RCMASTER_SENSORSCONTROL_H_

#include "NodeControl.h"

class SensorsControl {//: public NodeControl {
public:
	SensorsControl();
	virtual ~SensorsControl();
	bool getStatus();
	bool sendOrder();
	void setName();
	void check_sensors(status_t& status);
	int get_right_state(void);
	int get_left_state(void);
	int get_center_state(void);
	void set_thresholds (int min_sensor_dist_, int med_sensor_dist_);


private:
	char* nodeName;
	int left_state;
	int right_state;
	int center_state;
	int min_sensor_dist;
	int med_sensor_dist;
};

enum sensorStates {
	clear,
	medium,
	too_close,
};

#endif /* L5_APPLICATION_RCMASTER_SENSORSCONTROL_H_ */
