/*
 * CS61C Summer 2019
 * Name: Rui (Peter) Pan
 * Login: cs61c-ace
 */

// Test: Editing this file with Cyberduck
// Command for using valgrind: valgrind ./RouteTime timeHM.o flights.o RouteTime.o
// Update: That was actually incorrect. Use:
// make flights-memcheck

/*
References used:
1. free nodes in a linked list
https://stackoverflow.com/questions/6417158/c-how-to-free-nodes-in-the-linked-list

2. malloc for struct and pointer
https://stackoverflow.com/questions/14768230/malloc-for-struct-and-pointer-in-c

3. How to copy char *str to char c[] in C
https://stackoverflow.com/questions/1508838/how-to-copy-char-str-to-char-c-in-c

4. char array not assignable
https://stackoverflow.com/questions/27867437/char-array-not-assignable/27867453

5. Valgrin debugging: Use of uninitialised value of size 8
https://stackoverflow.com/questions/7098253/why-would-this-give-a-use-of-uninitialised-value-of-size-8

6. Valgrind Invalid read of size 4
https://stackoverflow.com/questions/13546650/valgrind-invalid-read-of-size-4-sigsegv-works-fine-without-valgrind-and-in

7. Understanding Valgrind errors (Extremely useful)
https://derickrethans.nl/valgrind-null.html

8. Segfault 11
https://stackoverflow.com/questions/12762944/segmentation-fault-11

9. boolean in C
https://en.wikibooks.org/wiki/C_Programming/stdbool.h
*/

/*
TODOs:
Finish writing the function validateFlightPath()
debug getNextFlight() and validateFlightPath()
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h> // boolean value to be used in getNextFlight()
#include "flights.h"
#include "flight_structs.h"
#include "timeHM.h"

/*
 *  This should be called if memory allocation failed.
 */
static void allocation_failed(void) {
  fprintf(stderr, "Out of memory.\n");
  exit(EXIT_FAILURE);
}


/*
 *  Creates and initializes a flight system, which stores the flight schedules of several airports.
 *  Returns a pointer to the system created.
 */
flightSys_t* createSystem(void) {
  flightSys_t* newSys = (flightSys_t *) malloc(sizeof(flightSys_t)); // Good practice to use sizeof() and typecasting
  if (newSys == NULL) {allocation_failed();}
  newSys->airport = NULL;
  return newSys;
}


/*
 *   Given a destination airport, departure and arrival times, and a cost, return a pointer to new flight_t.
 *
 *   Note that this pointer must be available to use even after this function returns.
 *   (What does this mean in terms of how this pointer should be instantiated?)
 *   Additionally you CANNOT assume that the `departure` and `arrival` pointers will persist after this function completes.
 *   (What does this mean about copying dep and arr?)
 */
flight_t* createFlight(airport_t* destination, timeHM_t* departure, timeHM_t* arrival, int cost) {
  if (destination == NULL || departure == NULL || arrival == NULL || isAfter(departure, arrival) ||cost < 0) return NULL;
  if (isAfter(departure, arrival) || cost < 0) { // Invalid input
    printf("Input not valid!\n");
    return NULL;
  }

  flight_t* newFlight = (flight_t *) malloc(sizeof(flight_t));
  if (newFlight == NULL) {allocation_failed();}
  newFlight->destination = destination;

  timeHM_t* newDeparture = (timeHM_t *) malloc(sizeof(timeHM_t));
  if (newDeparture == NULL) {allocation_failed();}
  newDeparture->hours = departure->hours;
  newDeparture->minutes = departure->minutes;
  newFlight->departure = newDeparture;

  timeHM_t* newArrival = (timeHM_t *) malloc(sizeof(timeHM_t));
  if (newArrival == NULL) {allocation_failed();}
  newArrival->hours = arrival->hours;
  newArrival->minutes = arrival->minutes;
  newFlight->arrival = newArrival;

  newFlight->cost = cost;
  newFlight->next = NULL;

  return newFlight;
}

/*
 *  Frees all memory associated with this system; that's all memory you dynamically allocated in your code.
 */
void deleteSystem(flightSys_t* system) {
  if (system == NULL) return NULL;
  // free the linked list of flights in the system
  airport_t* currAirport = system->airport;
  airport_t* temp = NULL;

  // delete all airports
  while (currAirport != NULL) {
    temp = currAirport;
    currAirport = currAirport->next;
    flight_t* currFlight = temp->flightList;
    flight_t* tempFlight;
    while (currFlight != NULL) {
      tempFlight = currFlight;
      currFlight = currFlight->next;
      deleteFlight(tempFlight);
    }
    free(temp);
  }
  free(system);
}

/*
 *  Frees all memory allocated for a single flight. You may or may not decide
 *  to use this in delete system but you must implement it.
 */
void deleteFlight(flight_t* flight) {
  if (flight == NULL) return NULL;
  free(flight->departure);
  free(flight->arrival);
  free(flight);
}

/* Returns a pointer to a newly created airport with the given name. */
airport_t* createAirport(char* name) {
  airport_t* newAirport = (airport_t *) malloc(sizeof(airport_t));
  if (newAirport == NULL) {allocation_failed();}
  newAirport->flightList = NULL;
  newAirport->next = NULL;
  strncpy(newAirport->name, name, 4);
  return newAirport;
}

/*
 *  Adds an airport with the given name to the system. You must copy the string and store it.
 *  Do not store `name` (the pointer) as the contents it point to may change.
 */
void addAirport(flightSys_t* system, char* name) {
  if (system == NULL || name == NULL) return NULL;
  airport_t* currAirport = system->airport;

  // create a new airport
  airport_t* newAirport = createAirport(name);

  // Traverse through the list of airports and add the new airport to the end of the linked list.
  if (currAirport == NULL) {
    system->airport = newAirport;
    return;
  }
  while (currAirport->next != NULL) {
    currAirport = currAirport->next;
  }
  currAirport->next = newAirport;
}



/*
 *  Returns a pointer to the airport with the given name.
 *  If the airport doesn't exist, return NULL.
 */
airport_t* getAirport(flightSys_t* system, char* name) {
  if (system == NULL || name == NULL) return NULL;
  airport_t* currAirport = system->airport;

  // Traverse through the list of airports and compare their names to the name passed in
  while (currAirport != NULL) {
    if (strcmp(currAirport->name, name) == 0) {
      return currAirport;
    }
    currAirport = currAirport->next;
  }
  // If the airport doesn't exist, return NULL
  return NULL;
}


/*
 *  Print each airport name in the order they were added through addAirport, one on each line.
 *  Make sure to end with a new line. You should compare your output with the correct output
 *  in `flights.out` to make sure your formatting is correct.
 */
void printAirports(flightSys_t* system) {
  if (system == NULL) return NULL;
  airport_t* currAirport = system->airport;

  while (currAirport != NULL) {
    printf("%s\n", currAirport->name);
    currAirport = currAirport->next;
  }
}


/*
 *  Adds a flight to source's schedule, stating a flight will leave to destination at departure time and arrive at arrival time.
 */
void addFlight(airport_t* source, airport_t* destination, timeHM_t* departure, timeHM_t* arrival, int cost) {
  if (source == NULL || destination == NULL || departure == NULL || arrival == NULL || cost <= 0) return NULL;
  // use createFlight to create a new flight structure
  flight_t* newFlight = createFlight(destination, departure, arrival, cost);
  // add to source airport's departing schedule
  flight_t* currFlight = source->flightList;

  if (currFlight == NULL) {
    // currFlight = newFlight;
    source->flightList = newFlight; // If I use currFlight = newFlight, it won't work. This is really weird. Now the bug is gone but I really want to know why that happened.
    // Oh I think I got it. If I do currFlight = newFlight, currFlight points to NULL and it won't assign newFlight to anything? Not really sure tho
    return;
  }

  while (currFlight->next != NULL) {
    currFlight = currFlight->next;
  }

  // Now currFlight is the last flight departing from the airport, we connect the newFlight with currFlight.
  currFlight->next = newFlight;
}


/*
 *  Prints the schedule of flights of the given airport.
 *
 *  Prints the airport name on the first line, then prints a schedule entry on each
 *  line that follows, with the format: "destination_name departure_time arrival_time $cost_of_flight".
 *
 *  You should use `printTime()` (look in `timeHM.h`) to print times, and the order should be the same as
 *  the order they were added in through addFlight. Make sure to end with a new line.
 *  You should compare your output with the correct output in flights.out to make sure your formatting is correct.
 */
void printSchedule(airport_t* airport) {
  if (airport == NULL) return NULL;
  // SFO 07:00 11:00 $230
  printf("%s\n", airport->name);
  flight_t* flightList = airport->flightList;

  while (flightList != NULL) {
    airport_t* destination = flightList->destination;
    printf("%s ", destination->name);
    printTime(flightList->departure);
    printf(" ");
    printTime(flightList->arrival);
    printf(" $%d\n", flightList->cost);
    flightList = flightList->next;
  }
}


/*
 *   Given a source and destination airport, and the time now, finds the next flight to take based on the following rules:
 *   1) Finds the earliest arriving flight from source to destination that departs after now.
 *   2) If there are multiple earliest flights, take the one that costs the least.
 *
 *   If a flight is found, you should store the flight's departure time, arrival time, and cost in the `departure`, `arrival`,
 *   and `cost` params and return true. Otherwise, return false.
 *
 *   Please use the functions `isAfter()` and `isEqual()` from `timeHM.h` when comparing two timeHM_t objects and compare
 *   the airport names to compare airports, not the pointers.
 */
bool getNextFlight(airport_t* source, airport_t* destination, timeHM_t* now, timeHM_t* departure, timeHM_t* arrival,
                   int* cost) {
  if (source == NULL || destination == NULL || now == NULL || departure == NULL || arrival == NULL || !cost || cost <= 0) return false;
  // When I used if (*cost < 0) return false; it won't pass the test (which is really weird)

  // Initialize arrival so the first iteration of the loop works
  arrival->hours = 24;
  arrival->minutes = 0; // largest, or latest time possible

  // get the list of flights that departures from the source airport
  flight_t* currFlight = source->flightList;
  bool found = false;
  // traverse through the list of flights
  while (currFlight != NULL) {
    if (strcmp(currFlight->destination->name, destination->name) == 0 && // if current flight goes to the airport we want
        isAfter(currFlight->departure, now) && // and the current flight departs after now
        (isAfter(arrival, currFlight->arrival) || (isEqual(arrival, currFlight->arrival) && (currFlight->cost < *cost)))) { 
      found = true;
      *departure = *(currFlight->departure);
      *arrival = *(currFlight->arrival);
      *cost = currFlight->cost;
    }
    currFlight = currFlight->next;
  }
  return found;
}

/*
 *  Given a list of flight_t pointers (`flight_list`) and a list of destination airport names (`airport_name_list`),
 *  first confirm that it is indeed possible to take these sequences of flights, (i.e. be sure that the i+1th flight departs
 *  after or at the same time as the ith flight arrives) (HINT: use the `isAfter()` and `isEqual()` functions).
 *  Then confirm that the list of destination airport names match the actual destination airport names of the provided flight_t structs.
 *
 *  `size` tells you the number of flights and destination airport names to consider. Be sure to extensively test for errors.
 *  As one example, if you encounter NULL's for any values that you might expect to be non-NULL return -1, but test for other possible errors too.
 *
 *  Return from this function the total cost of taking these sequence of flights.
 *  If it is impossible to take these sequence of flights,
 *  if the list of destination airport names doesn't match the actual destination airport names provided in the flight_t struct's,
 *  or if you run into any errors mentioned previously or any other errors, return -1.
 */
int validateFlightPath(flight_t** flight_list, char** airport_name_list, int size) {
  if (flight_list == NULL || airport_name_list == NULL || size <= 0) return -1;
  int totalCost = 0;
  char* currentAirportName = airport_name_list[0]; // Alright this works. Yayyyyyy
  flight_t* currFlight = flight_list[0];
  airport_t* currDestination = currFlight->destination;
  timeHM_t* now = currFlight->departure; // was not initialized

  for (int i = 0; i < size; i++) {
    currentAirportName = airport_name_list[i];
    currFlight = flight_list[i];
    currDestination = currFlight->destination;

    if (strcmp(currDestination->name, currentAirportName) != 0 ||
        isAfter(now, currFlight->departure) ||
        !isEqual(now, currFlight->departure)) {
      return -1;
    }

    totalCost += currFlight->cost;
    now = currFlight->arrival;
  }

  return totalCost;
}


