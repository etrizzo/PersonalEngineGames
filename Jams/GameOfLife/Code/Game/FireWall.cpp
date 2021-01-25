/* Question 1: Wall of fire
ASSUMPTIONS:
	- For the purposes of this question, I will use coordinate system: X= RIGHT, Y=UP, Z=FORWARD
	- the "height" of the wall should be measured along world up (Y).
		In part 3, I'll discuss some better solutions to this.
	- the damage range for the wall starts at the edge of the wall, not at the center of the wall.
	- for conveyance's sake, any enemies inside of the wall's 60x20x1 bounding box take normal damage 
		from the wall, even if they are inside of the wall but "behind" the center point (away from the normal)
	- For Entities, I'm assuming a capsule collider, and I am assuming that any collider overlap with the 
		"damage zone" of the wall should result in damage being applied, even if the center of the entity is slightly outside of the range

Thought process:
	- The wall creates a rectangular area, inside of which all entities should be damaged. My goal is to loop through all enemy entities,
		and damage anyone who overlaps with that area. This area needs to account for the wall's range & direction, but also the wall's
		thickness and the entity's collision radius.
*/


// ideally, these would be data driven somewhere so 
// that designers could easily tweak values for balance. 
constexpr int WALL_HEIGHT = 20;
constexpr int WALL_LENGTH = 60;
constexpr int WALL_THICKNESS = 1;
constexpr int WALL_DAMAGE_RANGE = 10;
constexpr int WALL_DAMAGE = 50;

float DotProduct(Vector3 a, Vector3 b) {
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}


void WallOfFire::DamageEnemies(std::vector<Entity*> entities, Vector3 wallCenterOnGround, Vector3 wallNormal) {	//get the point on the ground that is at the back edge of the wall, rather than the center, to make calculations simpler.	Vector3 backCenterPointOnGround = wallCenterOnGround - (wallNormal * WALL_THICKNESS * .5f);	for (Entity* entity : entities) {		if (entity->isEnemy()) {			Vector3 entityPosition = entity->getPosition();		//bottom center of the capsule collider			float entityRadius = entity->getCollider().getRadius();			Vector3 displacement = entity->getPosition() - backCenterPointOnGround;			if (displacement.y < WALL_HEIGHT) {				//within range vertically, along world up				float distanceToPlaneAlongNormal = DotProduct(displacement, wallNormal);				float maxDistanceFromPlaneToDamage = WALL_DAMAGE_RANGE + WALL_THICKNESS + entityRadius;				if (distanceToPlaneAlongNormal > 0 && distanceToPlaneAlongNormal < maxDistanceFromBackCenterToDamage) {					//the entity is on the correct side of the plane (or inside the wall), and within range along plane's forward					//	Note: if being inside of the wall should have some additional/alternate effect, this					//	could be checked here by comparing distanceToPlaneAlongNormal against WALL_THICKNESS					Vector3 displacementNormalComponent = distanceToPlaneAlongNormal * wallNormal;					Vector3 displacementUpComponent = Vector3(0.f, displacement.y, 0.0f);					Vector3 displacementPlanarComponent = displacement - displacementNormalComponent - displacementUpComponent;					if (displacementPlanarComponent.GetLength() < ((WALL_LENGTH * 0.5f) + entityRadius)) {						entity->Damage(WALL_DAMAGE, DAMAGE_TYPE_FIRE);					}				}			}		}	}}constexpr int RING_HEIGHT = 20;constexpr int RING_DIAMETER = 20;constexpr int RING_RADIUS = RING_DIAMETER * 0.5f;constexpr int RING_RANGE = 10;constexpr int RING_THICKNESS = 1;constexpr int RING_DAMAGE = 50;/*Question 2: Ring of fireASSUMPTIONS:	- For the ring of fire, I will also assume a height along world up.	- The thickness of the ring is centered around the radius specified.	- damage range starts at the outer or inner edge of the ring's thickness, depending on 		whether enemies are damaged inside or outside	- once again, entities have a capsule collider and any overlap with the ring or its damage range should		result in damage being dealt*/std::vector<Entity> WallOfFire::CollectEnemiesToDamage(std::vector<Entity> entities, Vector3 ringCenterOnGround, bool damageEnemiesOutside, bool damageEnemiesInside) {
	std::vector<Entity*> entitiesToDamage = std::vector<Entity>();
	for (Entity* entity : entities) {
		if (entity->isEnemy()) {
			Vector3 entityPosition = entity->getPosition();		//bottom center of the capsule collider			float entityRadius = entity->getCollider().getRadius();			Vector3 displacement = entity->getPosition() - ringCenterOnGround;
			if (displacement.y < RING_HEIGHT) {
				//in range vertically
				Vector3 displacementHorizontal = Vector3(displacement.x, 0.0f, displacement.z);
				float distanceFromCenterHorizontal = displacementHorizontal.getLength();
				if (damageEnemiesInside) {
					if (distanceFromCenterHorizontal < RING_RADIUS + entityRadius + (RING_THICKNESS * .5f)) {
						//entity overlaps with the ring or is inside of it
						float minDistanceFromCenterToDamage = RING_RADIUS - (RING_THICKNESS * 0.5f) - RING_RANGE - entityRadius;
						if (distanceFromCenterHorizontal >= minDistanceFromCenterToDamage) {
							//entity is within the damage range of the wall
							entitiesToDamage.push_back(entity);
						}
					}
				}
				else if (damageEnemiesOutside){
					float maxDistanceFromCenterToDamage = RING_RADIUS + (RING_THICKNESS * 0.5f) + RING_RANGE + entityRadius;
					if(distanceFromCenterHorizontal < maxDistanceFromCenterToDamage){
						//entity is within the damage range of the ring, must check that they are not inside the ring (or overlapping the ring)
						float minDistanceFromCenterToDamage = RING_RADIUS - (RING_THICKNESS * 0.5f) - entityRadius;
						if (distanceFromCenterHorizontal >= minDistanceFromCenterToDamage) {
							entitiesToDamage.push_back(entity);
						}
					}
				}
			}
		}
	}
	return entitiesToDamage;
}



/*
PART 3: IMPROVEMENTS
3a. WALL OF FIRE:
	- IN CPP, the vector of entities passed in (std::vector<Entity> entities) is a copy of the entities - to actually apply damage to the in-game entities
		and not the copy, I have updated the vector to be a vector of Entity pointers.
	- The wall of fire does not have a specified up, which means that behavior when cast on an sloped surface may not be what is expected, because how the "height" of
		the wall is measured is ambiguous.
		- if the wall should always extend horizontally even if cast on an incline, the wallNormal provided should have the up component removed (wallNormal.x, 0.0f, wallNormal.y)
		- if the wall should angle with the slope of the terrain, the terrain normal at wallCenterOnGround should be passed in as well, and this should be used
			as the up component in the function instead.
				-we could also assume that we want the up vector closest to world up on the plane, which would require us to calculate this by doing two cross products:
					Vector3 rightVector = CrossProduct(Vector3::WORLD_UP, wallNormal);
					Vector3 calculatedUp = CrossProduct(wallNormal, rightVector);
		- to be safe, the wall's intended "up" component should be passed into the function alongside the wall normal.
*/

void
WallOfFire::DamageEnemies_Improved(
	std::vector<Entity*> entities,
	Vector3 wallCenterOnGround,
	Vector3 wallNormal,
	Vector3 wallUp);		//assume this is normalized and orthogonal to wallNormal

//check against wall height becomes:
float distanceFromGroundAlongUp = DotProduct(displacement, wallUp);
if (distanceFromGroundAlongUp < WALL_HEIGHT) {
	// continue checking...
}


/*
3b. RING OF FIRE:
	- IN CPP, the vector of entities passed in (std::vector<Entity> entities) is a copy of the entities - to actually apply damage to the in-game entities
		and not the copy, I have updated the vector to be a vector of Entity pointers.
	- having separate bools for damageEnemiesOutside and damageEnemiesOutside in the declaration suggests that it is possible to damage enemies inside & out simultaneously by passing 
		in true for both values, or to damage no enemies by passing in false for both values. The design of the problem specifies that enemies can be damaged inside OR out, not both -
		to remedy this, only one bool should be used, damageEnemiesOutside, with a false value indicating that the damage should be applied inside the ring.
	- This problem could also benefit by specifying an up vector, as it is ambiguous what the behavior should be if cast on a sloped surface.
		(the cylinder of fire either extends at an angle along the ground's normal, or straight along world up)
*/

std::vector<Entity*>
WallOfFire::CollectEnemiesToDamage(
	std::vector<Entity*> entities,
	Vector3 ringCenterOnGround,
	Vector3 ringUpVector,
	bool damageEnemiesOutside);


//check for wall height:
Vector3 displacement = entity->getPosition() - ringCenterOnGround;
float distanceFromGroundAlongUp = 
if (distanceFromGroundAlongUp < RING_HEIGHT) {
	//...
}