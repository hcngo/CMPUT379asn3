/*
 * helpers.c
 *
 *  Created on: Apr 4, 2013
 *      Author: hcngo
 */

#include "helpers.h"

struct saucer saucerArray[MAX_THREADS];

struct rocket rocketArray[MAX_THREADS];

struct status gameStatus = { 0, INIROCKETS, 0 };

pthread_mutex_t drawMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t rocketMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * find a free slot (thread data) to faciliate a new thread.
 * Return the free position or -1 if there is no free slot.
 */
int findFreeSaucerThread() {
	int i;
	for (i = 0; i < MAX_THREADS; i++) {
		if (saucerArray[i].threadStatus == 0) {
			return i;
		}
	}
	return -1;
}

/*
 * find a free slot (thread data) to faciliate a new thread.
 * Return the free position or -1 if there is no free slot.
 */
int findFreeRocketThread() {
	int i;
	for (i = 0; i < MAX_THREADS; i++) {
		if (rocketArray[i].threadStatus == 0) {
			return i;
		}
	}
	return -1;
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
			" '.' to move right, 'SPACE' fires "); // TODO

	return numSaucers;
}

void printSaucer(const struct saucer *info, int col, const char * str) {
	pthread_mutex_lock(&drawMutex); /* only one thread	*/
	move( info->row, col);
	addch(' ');
	/* at a the same time	*/
	addstr(str);
	addch(' ');
	/* reentrant		*/
	move(LINES-1, COLS-1);
	/* park cursor		*/refresh(); /* and show it */
	pthread_mutex_unlock(&drawMutex); /* done with curses	*/
}

void *displayStatus(void *arg) {
	struct status * stat = arg;
	while (1) {
		usleep(100000);
		pthread_mutex_lock(&drawMutex);
		mvprintw(LINES - 3, 0, "score: %d, rockets left: %d, "
				"escaped saucers: %d", stat->score,
				stat->rocketsLeft, stat->escapedSaucers); // TODO
		move(LINES-1, COLS-1);
		refresh();
		pthread_mutex_unlock(&drawMutex);
	}
}

/* the code that runs in each thread */
void *drawSaucer(void *arg) {
	struct saucer *info = arg; /* point to info block	*/
	int len = strlen(info->shape);
	int col = 0;

	while (1) {
		usleep(info->delay * TUNIT);

		printSaucer(info, col, info->shape);

		/* move item to next column and check for bouncing	*/

		col++;

		if (col + len >= COLS) {
			printSaucer(info, col, "     ");
			info->threadStatus = 0;
			/*
			 * Update escaped saucers.
			 */
			pthread_mutex_lock(&statusMutex);
			gameStatus.escapedSaucers++;
			if (gameStatus.escapedSaucers == MAX_ESCAPED_SAUCERS) {
				fprintf(stderr, "error creating thread");
				endwin();
				exit(0);
			}
			pthread_mutex_unlock(&statusMutex);

			pthread_exit(NULL);
		}
	}
}

void *drawRocket(void *arg) {
	struct rocket *info = arg;
}

