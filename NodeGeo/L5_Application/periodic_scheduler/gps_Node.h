/*
 * gps_Node.h
 *
 *  Created on: Oct 13, 2016
 *      Author: ythao
 */
#ifndef L5_APPLICATION_PERIODIC_SCHEDULER_GPS_NODE_H_
#define L5_APPLICATION_PERIODIC_SCHEDULER_GPS_NODE_H_

/*
 * Defines all GPS Address types.
 * Converts these types into string so that strcpy() function can take them in as a parameter
 */
#define GPS C(GNVTG)C(GNGGA)C(GNGSA)C(GPGSV)C(GLGSV)C(GNGLL)C(GNRMC)C(GPRMC)
#define C(x) x,
enum gps_name { GPS CODE };
#undef C
#define C(x) #x,
const char * const gps_addr[] = { GPS };

typedef struct{
	int valid_bit;
	uint16_t counter;
	uint32_t latitude;
	uint32_t longitude;
}GPS_DATA;

void serialInit(void);
void check_reset_canbus(void);

uint32_t floatToDecimalDegree(float strDegree);

void readGPS(gps_name addr, GPS_DATA *data_r);
/*----- GPS Address and their Types of Data-----*/
/*
GNVTG - 	Course and speed relative to the ground.

GNGGA - 	Time, position, and fix related data of the receiver.

GNGSA x2 - 	Used to represent the ID�s of satellites which are used for position fix. When both GPS and GLONASS
			satellites are used in position solution, a $GNGSA sentence is used for GPS satellites and another
			$GNGSA sentence is used for GLONASS satellites. When only GPS satellites are used for position fix, a
			single $GPGSA sentence is output. When only GLONASS satellites are used, a single $GLGSA sentence
			is output.

GPGSV x3 -	Satellite information about elevation, azimuth and CNR, $GPGSV is used for GPS satellites, while
			$GLGSV is used for GLONASS satellites

GLGSV -		Satellite information about elevation, azimuth and CNR, $GPGSV is used for GPS satellites, while
			$GLGSV is used for GLONASS satellites

GNGLL - 	Position, time and fix status.

GNRMC - 	Time, date, position, course and speed data.
 */

bool dbc_app_send_can_msg(uint32_t mid, uint8_t dlc, uint8_t bytes[8]);

void sendCan1_Any_Message(uint32_t id, uint32_t frame, uint32_t data_len, uint64_t data);


#endif /* L5_APPLICATION_PERIODIC_SCHEDULER_GPS_NODE_H_ */
