/*
 * asn3.c
 *
 *  Created on: Apr 4, 2013
 *      Author: hcngo
 */

/*
 * NOTE:
 * 1.The user can position a launch site one line up from the bottom line of
 * the screen.
 * 2.The bottom line display status(score, rockets remaining, escaped saucers)
 * For the saucers:
 * At random intervals, a saucer may appear at the left of the screen in one
 * of the saucer rows.
 * Saucers must move from left to right.
 * Multiple saucers can exist per row.
 * Different saucers can move at different speeds.
 * Saucers can pass each other on a row.
 * When a saucer leaves the right side of the screen it "escapes"
 * After a predetermined number of saucers escape the game is over.
 *
 * For the rockets:
 * Firing a rocket must decrease the available rockets
 * When a rocket and saucer occupy the same location, both must disappear.
 * Exception: if multiple saucers occupy a location you can define the behavior,
 * and document it.
 * Destroying a saucer scores a point for the player
 * Destroying a saucer rewards the player with additional rockets
 * When the user runs out of rockets, the game is over.
 * To increase player enjoyment of the game, you might consider gradually
 * increasing the rate at which saucers are generated, saving high scores to
 * a file, or any other interesting features you can come up with.
 * Be sure to document any such features in your design document.
 */
#include "saucer.h"
#include "helpers.h"

/*
 void updateTimer() {
 pthread_mutex_lock(&timerMutex);
 timer++;
 if (timer == intervalSaucer) {
 int slot = findFreeSaucerThread();
 if (slot != -1) {
 saucerArray[slot].row = rand() % SAUCERS_ROWS;
 saucerArray[slot].delay = 1 + (rand() % 15);
 saucerArray[slot].threadStatus = 1;
 saucerArray[slot].shape = "<--->";

 pthread_create(&saucerThreads[slot], NULL, drawSaucer,
 &saucerArray[slot]);
 }
 timer = 0;
 intervalSaucer = (rand() % 4 + 1) * RANDOM_INTERVAL;
 }
 pthread_mutex_unlock(&timerMutex);
 }
 */

int timer = 0;
int intervalSaucer = RANDOM_INTERVAL;
pthread_t saucerThreads[MAX_THREADS]; /* the threads*/
pthread_t rocketThreads[MAX_THREADS];
pthread_t launchSiteThread;

void *updateTimer(void *arg) {
	while (1) {
		usleep(800000);
		timer++;
		if (timer == intervalSaucer) {
			int slot = findFreeSaucerThread();
			if (slot != -1) {
				saucerArray[slot].row = rand() % SAUCERS_ROWS;
				saucerArray[slot].col = 0;
				saucerArray[slot].delay = 1 + (rand() % 10);
				saucerArray[slot].threadStatus = 1;
				saucerArray[slot].shape = "<--->";

				pthread_create(&saucerThreads[slot], NULL, drawSaucer,
						&saucerArray[slot]); // TODO
				saucerArrLen++;
			}
			timer = 0;
			intervalSaucer = (rand() % 3 + 1) * RANDOM_INTERVAL;
		}
	}
}

int main(int ac, char *av[]) {
	int c; /* user input		*/
	pthread_t timerThread; /* Thread for simulating timer*/
	pthread_t displayStatusThread;
	int iniSaucers; /* number of strings	*/
	int i;
	int j;

	iniSaucers = setup(SAUCERS_ROWS);

	/* create all the threads */
	for (i = 0; i < iniSaucers; i++) {
		if (pthread_create(&saucerThreads[i], NULL, drawSaucer,
				&saucerArray[i])) { // TODO
			fprintf(stderr, "error creating thread");
			endwin();
			exit(0);
		}
		saucerArrLen++;
	}

	if (pthread_create(&timerThread, NULL, updateTimer, NULL)) {
		fprintf(stderr, "error creating thread");
		endwin();
		exit(0);
	}

	if (pthread_create(&displayStatusThread, NULL, displayStatus,
			&gameStatus)) { // TODO
		fprintf(stderr, "error creating thread");
		endwin();
		exit(0);
	}

	if (pthread_create(&launchSiteThread, NULL, displayLaunchSite, &lauSi)) { // TODO
		fprintf(stderr, "error creating thread");
		endwin();
		exit(0);
	}

	/* process user input */
	while (1) {
		c = getch();
		if (c == 'Q') {
			break;
		} else if (c == ',' || c == '.') {
			updateLaunchSite(&lauSi, c);
			if (pthread_create(&launchSiteThread, NULL, displayLaunchSite,
					&lauSi)) { // TODO
				fprintf(stderr, "error creating thread");
				endwin();
				exit(0);
			}
		} else if (c == 'f') {
			int slot = findFreeRocketThread();
			if (slot != -1) {
				rocketArray[slot].row = lauSi.row;
				rocketArray[slot].col = lauSi.col;
				rocketArray[slot].delay = ROCKET_SPEED;
				rocketArray[slot].threadStatus = 1;
				rocketArray[slot].shape = "^";

				pthread_create(&rocketThreads[slot], NULL, drawRocket,
						&rocketArray[slot]); // TODO
				rocketArrLen++;
			}

		}
	}

	/* cancel all the threads */
	pthread_mutex_lock(&drawMutex);
	for (i = 0; i < saucerArrLen; i++) {
		pthread_cancel(saucerThreads[i]);
	}
	for (j = 0; j < rocketArrLen; j++) {
		pthread_cancel(rocketThreads[j]);
	}
	pthread_cancel(timerThread);
	pthread_cancel(displayStatusThread);
	pthread_cancel(launchSiteThread);
	echo();
	endwin();
	exit(0);
}

