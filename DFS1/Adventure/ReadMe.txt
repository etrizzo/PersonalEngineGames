README.txt

ADVENTURE

Emily Rizzo

---------------
About Adventure
---------------

Adventure is a 2D roguelike, complete with procedurally generated maps, perma-death, a quest system, and a party system. Originally, this project was assigned over 5 weeks in my first software development class at Guildhall. I later decided to take the game further by continuing development over an 8-week directed focus study, and during my own time outside of class. My goal with development was to move beyond the standard roguelike formula, and give the player a sense of growth from quest to quest. I worked to achieve this by developing my map generation system, and modeling gameplay after my favorite arc in the popular Dungeons and Dragons podcast “The Adventure Zone.” The breadth of this project allowed me to explore many areas of interest for me – including procedural generation, AI, UI, and system design - and will continue to be a great project to return to and polish into a finished game.

In adventure, the player spawns into a large map, filled with both friendly and hostile entities. The player must explore the map to complete a main quest, which will bring them to the next encounter. But, the player can spend more time searching to complete side quests, to grant them more equipment, stats, and even new party members. As the player moves through encounters, the maps get progressively harder, with more and more high-level enemies spawning. The player must survive through as many encounters as possible, until their entire party dies out. 

To learn more, visit emilyrizzo.com/adventure.html

--------
Features
--------

	Map Generation
		- Data-defined sequence of generation steps
			- Fill and edge
			- Cellular automata
			- From-Image placement
		- Sub-area generation
			- Circular
			- Rectangular
			- Perlin noise
		- Organic edges to tiles

	Quest system
		- Main quests and Side quests tracked in pause menu
		- Side quests given and progressed through interacting with NPCs

	Party system
		- Allies can be gained by completing sidequests
		- Player can switch between playing as each party member

	Stats & Combat
		- Gameplay Stats (movement influences speed, strength influences arrow damage, defense influences damage taken)
		- Basic combat

	Items & Equipment
		- Items add to player stats when equipped
		- Inventory and Equipment (get and equip, no drop)

	User Interface
		- Simple menu, pause, and inventory screens
		- HUD (health, stats, equipped weapon)

	