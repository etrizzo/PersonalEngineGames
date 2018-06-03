README.txt

ADVENTURE GOLD

Emily Rizzo

------------
Known Issues
------------

	- No known issues


--------------------
Implemented Features
--------------------

	Rendering Features
		- glDrawArrays
		- Map::RenderTiles()
		- Entity sorted draw order

	Physics Features
		- Actor v. actor disc corrective

	Map Metadata features
		- TileExtrInfo
		- HeatMap
		- tags class exists but isn't used anywhere

	Stats & Combat
		- Stats (movement influences speed, strength influences arrow damage, defense influences damage taken)
		- Combat (partial)
			- player and enemy entities shoot arrows, which interact with strength and defense stats

	Items & Equipment
		- Items which add to player stats
		- Inventory and Equipment (get and equip, no drop)

	MapGeneration
		- MapGenStep_SpawnActor
		- MapGenStep_SpawnItem
		- MapGenStep_CellularAutomata
		- MapGenStep_PerlinNoise (used in village map for flowers)
		- MapGenStep_RoomsAndPaths

	Audio
		- Music effects (unique to maps!)

	Overlay features
		- UI (main menu, inventory, paused
		- HUD (health, stats, equipped weapon)

	AI features
		- dijkstra pathfinding implemented, but not used for AI

	Custom Features
		- Multiple victory conditions for adventures and objective tracking
		- MapGenStep Conway - just really specific cellular automata for cave system
		- tile variants

----------
How to Use
----------

	Attract:
		- Xbox Controller:
			- Press 'START' or 'A' to begin

		- Keyboard:
			- Press 'P' or 'ENTER' to begin
			- Press 'M' to enter map generation mode
			- Press ESC to quit
	
	Playing:
		- Xbox Controller:
			- Use the left joystick to control the archer
			- press 'A' to fire arrows in the direction you are facing
			- press 'START' to open/close paused/objective screen
			- press 'BACK' to open/close inventory screen
				- D-pad up/down arrows are used to navigate inventory screen
				- press 'A' to equip/unequip an item
	
		- Keyboard:
			- Use the arrow keys to control the archer
			- press 'SPACE' to fire arrows in the direction you are facing
			- press 'P' to open/close paused/objective screen
				- press ESC to return to main menu
			- press 'I' to open/close inventory screen
				- up/down arrow keys are used to navigate inventory screen
				- press 'ENTER' to equip/unequip an item
				- press ESC to return to main menu
			- press 'M' to toggle full map mode
			- press 'F1' to toggle developer mode, which displays physics radius, drawing boxes, and Dijkstra distance map from the player (unused)

	Map Generation Mode:
		- Keyboard:
			- left/right arrow keys move between maps in the adventure
			- F5 regenerates the current map
				- note: due to the way that portals are generated, regenerating a map with portals that link to other maps will 
					cause multiple portals to be present on the linked map.
	


-------------
Deep Learning
-------------

	- I have learned a lot about the way that I plan during this assignment. One switching between planning for features that I wanted to add, and
	  features that were required for the assignment, which resulted in me half-way completing a lot of features. I think that strictly planning out
	  what I want to work on in a work session would be helpful for me, so that I don't lose focus because of other possibilities.
	- I also learned that it's always better to think about the ramifications of a feature before jumping into doing it. For instance, I just jumped
	  into my UI system, and while I'm happy with the way it turned out, it would have saved a lot of time for me to think about helper functions I
	  might want to add to the AABB2 class before jumping into it.
	- I also learned that it's usually best to re-write classes, even if a lot of functionality can be copied over. A lot of the code I brought over from
	  incursion ended up hindering my progress on adventure, just because it was written for a different game, and I wasn't as careful with my code when I 
	  first wrote it. 