/*
 * helpers.h
 *
 *  Created on: Apr 4, 2013
 *      Author: hcngo
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curses.h>
#include <pthread.h>

/*
 * The user initially has a reasonably small number of rockets
 */
#define	INIROCKETS	30

/*
 * Saucers can only occupy the top few lines of the screen
 */
#define	SAUCERS_ROWS	5

#define	SAUCER_SHAPE	"<--->"

#define ERASE_SHAPE_SAUCER	"     "

#define ERASE_SHAPE_ROCKET	" "

/*
 * random time interval saucers are created
 */
#define 	RANDOM_INTERVAL	1

/*
 * Max number of saucers and rockets in the game
 * This is one third of the total allowable threads in a process.
 */
#define	MAX_THREADS	5289

/*
 * timeunits in microseconds.
 */
#define	TUNIT		15000

/*
 * Predetermined number of saucers escape the game is over
 */
#define	MAX_ESCAPED_SAUCERS	20

#define	ROCKET_SPEED	8

struct saucer {
	/*
	 * The row the saucer occupies
	 */
	int row;

	/*
	 * the column the saucer occupies
	 */
	int col;

	/*
	 * delay in time units
	 */
	int delay;

	/*
	 * status of thread data. Indicate if this data is still valid
	 * Not valid -> reclaim this data for another thread.
	 */
	int threadStatus;

	/*
	 * Shape of the saucer
	 */
	char * shape;
};

struct rocket {
	/*
	 * the row the rocket occupies.
	 */
	int row;

	/*
	 * the column the corket occupies
	 */
	int col;

	/*
	 * delay in time units
	 */
	int delay;

	/*
	 * status of thread data. Indicate if this data is still valid
	 * Not valid -> reclaim this data for another thread.
	 */
	int threadStatus;

	/*
	 * Shape of the rocket.
	 */
	char * shape;

};

struct status {
	int score;
	int rocketsLeft;
	int escapedSaucers;
};

struct launchSite {
	int row;
	int col;
	char * shape;
};

extern pthread_mutex_t drawMutex;
extern struct saucer saucerArray[MAX_THREADS];
extern struct status gameStatus;
extern struct launchSite lauSi;
extern int saucerArrLen;

extern int rocketArrLen;

void updateLaunchSite(struct launchSite * ls, const char keyClick );

void *displayLaunchSite(void *arg);

void *displayStatus(void *arg);

void *drawSaucer(void *arg);

int setup(int iniSaucers);

void printSaucer(const struct saucer *info, const char * str);

/*
 * find a free slot (thread data) to faciliate a new thread.
 * Return the free position or -1 if there is no free slot.
 */
int findFreeSaucerThread();

#endif /* HELPERS_H_ */
