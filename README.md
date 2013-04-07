#Design Documetation Assignment \#3 CMPUT 379
##Name: Hieu Ngo

1.	###Overall Structure of my code
	
	My program is broken down into two main files: *helpers.c* and *saucer.c*.  
	*helpers.c* contains most of the functions and global variables, including
	arrays storing thread data(*saucerArray*, *rocketArray*, ...), functions to
	be scheduled in threads(*drawSaucer*, *drawRocket*,... ).  
	*saucer.c* contains the main function which runs at all time to accept key
	click from the user. Also it includes two functions to be scheduled in
	threads: *updateTimer()*, simulating clock to emit saucers after random 
	interval, and *collide()*, thread to discover collision between any saucer
	and rocket.
	
	The main function is to call *setup()* to set up the curses screen and 
	generate some innitial saucers as well as some other global variables, and then
	to create threads for each saucer, each rocket, a timer, displaying game
	status(score, rockets left, escaped saucers), displaying launch site, and
	collision recognition. 
	
	There are three main data structures in the program, namely *saucer*, *rocket*
	and *status* and *launchSite*. *saucer* and *rocket* structs are quite similar,
	each of which is comprised of row, column, speed, data validity bit and the shape.
	*status* contains score, rockets remaining and number of escaped saucers.
	
2.	###Threads Description

	The following threads are used in my program:
	1.	drawSaucer(): this thread function is to draw and redraw the location
		of saucers on the game screen. The location of saucer is updated after
		some interval which is determined by the speed of the saucer passed as
		parameter to this thread. Upon a saucer escaping, the *escapedSaucers*
		in *gameStatus* is incremented before the thread is terminated.
	
	2.	drawRocket(): this thread function is to draw and redraw the location
		of rockets on the game screen. Similar to saucer, rocket's location is 
		updated using its speed. Upon one rocket being emitted(one new thread is scheduled), 
		the info *rocketsLeft* in *gameStatus* is decremented. 
		Upon rocket exiting the screen, the thread is terminated.
		
	3.	updateTimer(): its jobs is to simulate the clock and create a new thread for
		a new saucer after some random interval.
		
	4.	displayStatus(): display the status of the game, including score, rockets
		remaining and escaped saucers. 
		
	5.	displayLaunchSite(): display the position of the launch site where rockets
		are launched.
	
	6.	collide(): sweep through all rockets and saucers to discover collision.
		Upon a collision between a rocket and saucer, score is incremented by one
		point and a rocket is rewared to the user, i.e. increment rockets remainging.  
		NOTE: IF MULTIPLE SAUCERS OCCUPY A LOCATION, A ROCKET JUST COLLIDE WITH ONE
		ARBITRARY SAUCER.
		
3.	###Critical Sections 
	
	The following mutex variables are used to protect critical sections in the program:
	1.	drawMutex: to protect the sharing of the screen used by curses. Almost all of the
		threads(except for *updateTimer()*) need to update the screen constantly so the *drawMutex*
		mutex variable is to ensure only one thread can update the screen at any point of time.
	
	2.	statusMutex: to protect the sharing of *gameStatus* global variable. *drawSaucer()*,
		*drawRocket()* and *collide()* threads can access to *gameStatus* to update it so the 
		statusMutex is for protecting data integrity of *gameStatus*.
		
	NOTE: In some cases, some thread may attempt to read data used by other threads and 
	that thread doesn't need to acquire a mutex lock to execute the READ operation.
	
4.	###User Interface

	The user interface of this game is implemented with *ncurses* library in C.
	The instruction to play the game is displayed on the game screen at all time.
	
	>',' : move launch site to the left  
	>'.' : move launch site to the right  
	>'f' : fire rocket  
	>'Q'	: quit playing
	
	
