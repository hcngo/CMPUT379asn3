/*
 * helpers.c
 *
 *  Created on: Apr 4, 2013
 *      Author: hcngo
 */

#include "helpers.h"

struct saucer saucerArray[MAX_THREADS];
int saucerArrLen = 0;

struct rocket rocketArray[MAX_THREADS];
int rocketArrLen = 0;

struct launchSite lauSi;

struct status gameStatus = { 0, INIROCKETS, 0 };

pthread_mutex_t drawMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t rocketMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

void loseMessage() {
	clear();
	refresh();
	mvprintw(LINES / 2, COLS / 2, "YOU LOSE!");
	refresh();
	usleep(2000000);
	endwin();
	exit(0);
}

/*
 * find a free slot (thread data) to faciliate a new thread.
 * Return the free position or -1 if there is no free slot.
 */
int findFreeSaucerThread() {

	if (saucerArrLen < MAX_THREADS) {
		return saucerArrLen;
	} else {
		int i;
		for (i = 0; i < MAX_THREADS; i++) {
			if (saucerArray[i].threadStatus == 0) {
				return i;
			}
		}
		return -1;
	}
}

/*
 * find a free slot (thread data) to faciliate a new thread.
 * Return the free position or -1 if there is no free slot.
 */
int findFreeRocketThread() {
	if (rocketArrLen < MAX_THREADS) {
		return rocketArrLen;
	} else {
		int i;
		for (i = 0; i < MAX_THREADS; i++) {
			if (rocketArray[i].threadStatus == 0) {
				return i;
			}
		}
		return -1;
	}
}

int setup(int iniSaucers) {

	int numSaucers = (iniSaucers > MAX_THREADS ? MAX_THREADS : iniSaucers);

	int i;

	int j;
	for (j = 0; j < MAX_THREADS; j++) {
		saucerArray[j].threadStatus = 0;
		rocketArray[j].threadStatus = 0;
	}

	/*
	 * Assign row and velocity to each initial saucers.
	 */
	srand(getpid());
	for (i = 0; i < numSaucers; i++) {
		saucerArray[i].row = rand() % SAUCERS_ROWS; /* the row	*/
		saucerArray[i].col = 0;
		saucerArray[i].delay = 1 + (rand() % 15); /* a speed	*/
		saucerArray[i].threadStatus = 1;
		saucerArray[i].shape = "<--->";
	}

	/* set up curses */
	initscr();
	crmode();
	noecho();
	clear();
	mvprintw(LINES - 1, 0, "'Q' to quit, ',' to move left,"
	    " '.' to move right, 'f' fires "); // TODO
	refresh();
	struct launchSite ls = { LINES - 2, (int) COLS / 2, "|" };
	lauSi = ls;

	return numSaucers;
}

void printSaucer(const struct saucer *info, const char * str) {
	pthread_mutex_lock(&drawMutex); /* only one thread	*/
	move( info->row, info->col);
	addch(' ');
	/* at a the same time	*/
	addstr(str);
	addch(' ');
	/* reentrant		*/
	move(LINES-1, COLS-1);
	/* park cursor		*/refresh(); /* and show it */
	pthread_mutex_unlock(&drawMutex); /* done with curses	*/
}

void printRocket(const struct rocket *info, const char * str) {
	pthread_mutex_lock(&drawMutex);
	move(info->row + 1, info->col);
	addch(' ');
	move(info->row, info->col);
	addstr(str);
	move(info->row - 1, info->col);
	addch(' ');
	move(LINES - 1, COLS - 1);
	refresh();
	pthread_mutex_unlock(&drawMutex);
}

void *displayStatus(void *arg) {
	struct status * stat = arg;
	while (1) {
		usleep(100000);
		pthread_mutex_lock(&drawMutex);
		mvprintw(LINES - 1, (int) COLS / 2,
		    "score: %5d, rockets left: %5d, "
		    "escaped saucers: %5d",
		    stat->score, stat->rocketsLeft,
		    stat->escapedSaucers); // TODO
		move(LINES-1, COLS-1);
		refresh();
		pthread_mutex_unlock(&drawMutex);
	}
}

void updateLaunchSite(struct launchSite * ls, const char keyClick) {
	if (keyClick == ',') {
		if (ls->col > 0) {
			ls->col--;
		}
	} else if (keyClick == '.') {
		if (ls->col < COLS - 2) {
			ls->col++;
		}
	}
}

void *displayLaunchSite(void *arg) {
	struct launchSite * info = arg;
	pthread_mutex_lock(&drawMutex);
	move(info->row, info->col);
	addch(' ');
	addstr(info->shape);
	addch(' ');
	move(LINES-1, COLS-1);
	refresh(); /* and show it */
	pthread_mutex_unlock(&drawMutex);
	pthread_exit(NULL);
}

/* the code that runs in each thread */
void *drawSaucer(void *arg) {
	struct saucer *info = arg; /* point to info block	*/
	int len = strlen(info->shape);

	while (1) {
		usleep(info->delay * TUNIT);

		printSaucer(info, info->shape);

		/* move item to next column and check for bouncing	*/

		info->col++;

		if (info->col + len >= COLS) {
			printSaucer(info, ERASE_SHAPE_SAUCER);
			info->threadStatus = 0;
			/*
			 * Update escaped saucers.
			 */
			pthread_mutex_lock(&statusMutex);
			gameStatus.escapedSaucers++;
			if (gameStatus.escapedSaucers == MAX_ESCAPED_SAUCERS) {
				loseMessage();
			}
			pthread_mutex_unlock(&statusMutex);

			pthread_exit(NULL);
		}
	}
}

void *drawRocket(void *arg) {
	pthread_mutex_lock(&statusMutex);
	gameStatus.rocketsLeft--;
	if (gameStatus.rocketsLeft < 0) {
		loseMessage();
	}
	pthread_mutex_unlock(&statusMutex);

	struct rocket *info = arg;
	int len = strlen(info->shape);
	while (1) {
		usleep(ROCKET_SPEED * TUNIT);
		printRocket(info, info->shape);

		info->row--;
		if (info->row < 0) {
			printRocket(info, ERASE_SHAPE_ROCKET);
			info->threadStatus = 0;

			pthread_exit(NULL);
		}
	}
}

