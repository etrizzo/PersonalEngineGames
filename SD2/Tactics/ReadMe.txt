README.txt

ASTEROIDS GOLD - Assignment 3

Emily Rizzo

------------
Known Issues
------------

	- No known issues


----------
How to Use
----------

	- Controlling the ship 
		- Keyboard: 
			- Use either the 'Up', 'Left', and 'Right' arrow keys, or the 'E, 'S', and 'F' keys.

			- To thrust the ship in the direction it is currently facing, press 'Up' or 'E'

			- To rotate the ship left, press'Left' or 'S' 
	
			- To rotate the ship right, press 'Right' or 'F'

		- Xbox Controller:
			- Use the left joystick to control the ship

			- Ship direction is determined by the angle of the joystick
		
			- Thrust is determined by the magnitude of the joystick position
	

	- To fire a bullet, press 'Space' on keyboard, or 'A' on the Xbox Controller

	- Pressing and holding 'X' on the keyboard or 'X' on the Xbox controller will charge a starburst attack.
		- The player cannot control the ship while charging the shot
		- The current charge is indicated by the two rings that form around the ship. When both have filled up,
		  the attack is fully charged.

	- Releasing 'X' will fire a burst of bullets in an arc around the ship. The size of the arc will vary depending on how
	  long the button was held down, up to a maximum of 2 full circles around the ship
		- The attack has a set cooldown that is represented by the orange circle in the top right corner of the screen.

	- To enter Developer mode, press 'F1'

	- To spawn a new asteroid, press 'I'

	- To delete the most recently spawned asteroid, press 'O'

	- To slow down time, press and hold 'T'

	- To pause time entirely, press 'P.' Pressing 'P' again will unpause the game.

	- To quit, press 'ESC'


------
JUICE
-----
	- Starburst charge attack
	- Spaceship fade-in
	- FACES


-------------
Deep Learning
-------------

	- This assignment taught me a pretty valuable lesson in organization. Starting out, 
	  I didn't think very critically about what should or should not go in the Entity class,
	  and ended up having to reorganize how I initialized, updated, and rendered the entities in 
	  my program several times, which made refactoring my code for Gold that much harder. I believe that
	  thinking about code from a top-down approach has helped me a lot within the last week - while Gold was
	  definitely the largest assignment in the project, a lot of the coding went much more smoothly as I started
	  to think more critically about what my code should be doing, rather than focusing on the actual programming.

	- Time management was also critical for this assignment. Even though I made a solid effort to spread my time more
	  evenly over the course of the Gold assignment, I still ended up feeling rushed when adding juice, and my code 
	  suffered for it. In the future, I need to budget more time at the end of assignments for cleanup and polish.