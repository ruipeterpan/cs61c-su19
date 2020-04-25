/*
 * CS61C Summer 2019
 * Name: Rui (Peter) Pan
 * Login: cs61c-ace
 */

#ifndef FLIGHT_STRUCTS_H
#define FLIGHT_STRUCTS_H

#include "timeHM.h"

typedef struct flightSys flightSys_t;
typedef struct airport airport_t;
typedef struct flight flight_t;

/* Hold all the airports in the system. */
struct flightSys {
	airport_t* airport;
};

/* Hold its name and a schedule of all the flights departing from it. */
struct airport {
	char name[4]; // IATA airport code is a three-letter code
	flight_t* flightList;
	airport_t* next;
};

/*
Holds a pointer to the destination airport,
time of departure,
time of arrival,
and the cost of the flight.
*/
struct flight {
	airport_t* destination;
	timeHM_t* departure;
	timeHM_t* arrival;
	int cost;
	flight_t *next;
};

#endif

