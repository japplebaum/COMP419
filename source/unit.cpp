#include "unit.h"
#include "IwGeom.h"

Unit::Unit(const Unit& newUnit): WorldObject(newUnit), hp(newUnit.hp), cost(newUnit.cost), 
	speed(newUnit.speed), curFrame(0), numFrames(newUnit.numFrames), pathMode(NORMAL)
{
	setOwner(newUnit.owner);
	setPosition(position);
}

Unit::Unit(float hp, float cost, float speed, Player* owner, CIwFVec2 position, Game* game): WorldObject(position, game), hp(hp), 
	cost(cost), speed(speed), curFrame(0), pathMode(NORMAL)
{
    setOwner(owner);
}

int Unit::getDamage(Unit* unit){
	return (int) statAttacks[unit->getType()][getType()];
}

void Unit::display() {
	IwGxSetColStream(owner->getColors(), 4);
    renderImageWorldSpace(position, getAngle(), scale, UNIT_SPRITE_SIZE, game->getRotation(), curFrame, numFrames, 0.0f);

    //UNCOMMENT TO DRAW DEBUG PRIMITIVES. Yellow circle = Unit Sight. Blue circle = Unit bounding volume
    
	/*CIwFVec2 circle = navTarget;
	polarize(circle);
	circle.y *= -1;
	polarToXY(circle);
	
    CIwMat pMat = CIwMat::g_Identity;
    pMat.SetTrans(CIwVec3(circle.x, circle.y, 1));
    CIwMat rot = CIwMat::g_Identity;
    rot.SetRotZ(IW_ANGLE_FROM_RADIANS(game->getRotation()))
    //IwGxDebugPrimCircle(pMat*rot, sight, 2,IwGxGetColFixed(IW_GX_COLOUR_YELLOW), false);*/
}

void Unit::displayOnScreen(int x, int y){    
    CIwMaterial *mat = new CIwMaterial();
	mat->SetTexture((CIwTexture*)game->getSprites()->GetResHashed(getTextureName(), IW_GX_RESTYPE_TEXTURE));
    mat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    mat->SetAlphaMode(CIwMaterial::ALPHA_DEFAULT);
    IwGxSetMaterial(mat);
    
	CIwSVec2 xy(x-45, y-45);
    CIwSVec2 duv(IW_FIXED(1.0/numFrames), IW_GEOM_ONE);
    
	static CIwSVec2 wh(90, 90);
	static CIwSVec2 uv(IW_FIXED(0), IW_FIXED(0));	
    
    IwGxSetScreenSpaceSlot(1);
    IwGxDrawRectScreenSpace(&xy, &wh, &uv, &duv);
    
    delete mat;
}

//
// Getter and setters
//
int Unit::getId(){ return uid; }

void Unit::setId(int uid){ this->uid = uid; }

Player& Unit::getOwner(){ return *owner; }

void Unit::setOwner(Player* p){
	owner = p;
    localPlayedOwnsThis = owner == game->getLocalPlayer();
	
	if (localPlayedOwnsThis) {
		enemyLeaderPos = ((Unit*)(game->getOpponentPlayer()->getLeader()))->getPosition();
	}
	else {
		enemyLeaderPos = ((Unit*)(game->getLocalPlayer()->getLeader()))->getPosition();
	}
}

float Unit::getHp(){ return hp; }

float Unit::getSpeed(){ return speed; }

float Unit::getSize(){ return UNIT_SPRITE_SIZE*scale; }

CIwFVec2 Unit::getVelocity(){ return velocity; }

float Unit::getAngle(){
    CIwFVec2 norm = velocity.GetNormalised();
    return PI + atan2(norm.y, norm.x);
}

void Unit::receiveDamage(float amount){ hp -= amount; }

//
//Pathing/AI methods
//
void Unit::path() {
	
	// Normalized vector from this unit to the enemy leader. 
	// Need this for normal pathing and escape pathing.
	CIwFVec2 toLeader = (enemyLeaderPos-position).GetNormalised();
	
	CIwFVec2 repulsionSum = CIwFVec2::g_Zero;
	
	/********************************
	 **** Begin Force Calculation ***
	 ********************************/
	
	
	std::list<Unit*>* units = game->getUnits(); //all the units in the game
	CIwFVec2 force = CIwFVec2::g_Zero; //sum of all forces on this unit
	CIwFVec2 dirToward = CIwFVec2::g_Zero; //vector from current repelling unit to this unit
	Unit* curUnit; //current repelling unit in loop
	
	//sum up all of the repulsive forces on this unit
	for (std::list<Unit*>::iterator itr = units->begin() ; itr != units->end(); ++itr) {
		curUnit = *(itr);
		
		if (curUnit != this && curUnit->getType() != PROJECTILE) {
			dirToward = position - curUnit->getPosition();
			float dist = dirToward.GetLengthSquared();
			repulsionSum += dirToward.GetNormalised() * (curUnit->getSize()*REPEL_FACTOR / pow(dist, 1.875));
		}
	}
	
	force += repulsionSum;

	if (pathMode == OBJECTIVE) {
		/*if (target != NULL) {
			navTarget = target->getPosition();
		}
		else {
			pathMode = NORMAL;
		}*/
	}
	
	// if not escape pathing, add at least the circular pathing force 
	if(pathMode != ESCAPE) {
		float rDiff = (game->getWorldRadius().y + game->getWorldRadius().x)/2.0 - r;
		force += position * ((rDiff < 0 ? -1 : 1) * CIRCLE_SPRING * SQ(rDiff)); // Ternary is experimentally faster
		
		// only add enemy leader force if doing normal pathing
		if (pathMode == NORMAL) {
			force += LEADER_ATTRACTION * toLeader;
		}
	}
	
	// if in a special pathing mode, add force toward the navigation target
	if (pathMode != NORMAL) {
		force += NAV_ATTRACT_FACTOR * (navTarget-position).GetNormalised();
	}
	
	// if we're running into walls, add their normal force to the sum
	CIwFVec2 worldRad = game->getWorldRadius();
	float unitRad = getSize()/2;
	float toInner = getR() - worldRad.x,
	toOuter = worldRad.y - getR();
	
	/********************************
	 **** End Force Calculation *****
	 ********************************/
	
	if (force.GetLengthSquared() < FORCE_THRESHOLD) {
		setEscapeTarget(toLeader, force);
	}

	if (pathMode == ESCAPE) {
		CIwFVec2 normal = (CIwFVec2(-force.y, force.x)).GetNormalised();
		CIwFVec2 repulsionNorm = repulsionSum.GetNormalised();
		CIwFVec2 normalForce = repulsionSum * normal.Dot(repulsionNorm);
		
		if (normalForce.GetLengthSquared() < NORMAL_FORCE_THRESHOLD) {
			pathMode = NORMAL;
		}
		else {
			setEscapeTarget(toLeader, force);
		}

	}
	
	if (toInner <= unitRad) {
		CIwFVec2 normal = CIwFVec2(cos(getTheta()), sin(getTheta()));
		float dot = force.GetNormalised().Dot(-1 * normal);
		force += normal * (dot > 0 ? dot : 0) * force.GetLength();
		if(pathMode == ESCAPE) setEscapeTarget(toLeader, force);
	}
	else if(toOuter <= unitRad) {
		CIwFVec2 normal = CIwFVec2(-cos(getTheta()), -sin(getTheta()));
		float dot = force.GetNormalised().Dot(-1 * normal);
		force += normal * (dot > 0 ? dot : 0) * force.GetLength();
		if(pathMode == ESCAPE) setEscapeTarget(toLeader, force);
	}
	
	if (pathMode == OBJECTIVE /*&& (navTarget-position).GetLengthSquared() <= SQ(range)*/) {
		velocity = force.GetNormalised();
	}
	else {
		float curSpeed = 10 * speed * force.GetLengthSquared()/(SQ(LEADER_ATTRACTION));
		curSpeed = (curSpeed <= speed) ? curSpeed : speed;
		velocity = curSpeed * force.GetNormalised();
		setPosition(position + velocity);
	}
}

									   
// Create a navigation target at the wall furthest from this
// unit, slightly ahead of it's current theta position.
void Unit::setEscapeTarget(CIwFVec2 toLeader, CIwFVec2 force) {	
	
	float thetaDir; //the "forward" direction toward the enemy base
	
	if (owner == game->getLocalPlayer()) {
		thetaDir = getTheta() > 0 ? 1 : -1;
	}
	else {
		thetaDir = getTheta() > PI ? 1 : -1;
	}

	CIwFVec2 worldRad = game->getWorldRadius();
	float innerDist = getR() - worldRad.x;
	float outerDist = worldRad.y - getR();
	
	// if already escape pathing, don't change the target wall, else
	// figure out which wall is the furthest from us, and go to it
	if (pathMode != ESCAPE) {
		if (innerDist > outerDist) {
			navTarget.x = worldRad.x - getSize();
		}
		else {
			navTarget.x = worldRad.y + getSize();
		}
		
		navTarget.y = getTheta() + .3 * thetaDir; 
		polarToXY(navTarget);
	}
	else {
		polarize(navTarget);
		navTarget.y += .01 * thetaDir * (force.GetLengthSquared() < SQ(NAV_ATTRACT_FACTOR) ? -1 : (PI - abs(getTheta())) * 10);
		polarToXY(navTarget);
	}
	
	pathMode = ESCAPE;
}