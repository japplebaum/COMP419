#include "unit.h"
#include "leader.h"
#include "IwGeom.h"

Unit::Unit(const Unit& newUnit): WorldObject(newUnit), hp(newUnit.hp), 
	speed(newUnit.speed), curFrame(0), numFrames(newUnit.numFrames), 
	scale(newUnit.scale), pathMode(TOLEADER)
{
	setOwner(newUnit.owner);
	setPosition(position);
}

Unit::Unit(float hp, float speed, Player* owner, CIwFVec2 position, Game* game): WorldObject(position, game), hp(hp), 
	speed(speed), curFrame(0), pathMode(TOLEADER)
{
    setOwner(owner);
}

int Unit::getDamage(Unit* unit){ return (int) statAttacks[unit->getType()][getType()]; }

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
    rot.SetRotZ(IW_ANGLE_FROM_RADIANS(game->getRotation()));
    IwGxDebugPrimCircle(pMat*rot, 1, 2,IwGxGetColFixed(IW_GX_COLOUR_YELLOW), false);*/
}

void Unit::displayOnScreen(CIwSVec2 pos) {    
    CIwMaterial *mat = new CIwMaterial();
	mat->SetTexture((CIwTexture*)game->getSprites()->GetResHashed(getTextureName(), IW_GX_RESTYPE_TEXTURE));
    mat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    mat->SetAlphaMode(CIwMaterial::ALPHA_DEFAULT);
    IwGxSetMaterial(mat);
    
	CIwSVec2 xy(pos.x-45, pos.y-45);
    CIwSVec2 duv(IW_FIXED(1.0/numFrames), IW_GEOM_ONE);
    
	static CIwSVec2 wh(90, 90);
	static CIwSVec2 uv(IW_FIXED(0), IW_FIXED(0));	
    
    IwGxSetScreenSpaceSlot(1);
    IwGxDrawRectScreenSpace(&xy, &wh, &uv, &duv);
    
    delete mat;
}

void Unit::setOwner(Player* p) {
	owner = p;
	if (owner == game->getLocalPlayer()) enemyLeaderPos = game->getOpponentLeaderPos();
	else enemyLeaderPos = game->getLocalLeaderPos();
}

float Unit::getSize() { return UNIT_SPRITE_SIZE * scale; }

float Unit::getAngle() {
    CIwFVec2 norm = velocity.GetNormalised();
    return PI + atan2(norm.y, norm.x);
}

void Unit::receiveDamage(float amount) { hp -= amount; }


//
//Pathing/AI methods
//


CIwFVec2 Unit::getLeaderForce() { return NAV_ATTRACT_FACTOR * (enemyLeaderPos-position).GetNormalised(); }

CIwFVec2 Unit::getRepulsiveForce() {
	CIwFVec2 sumForces = CIwFVec2::g_Zero;
	CList<Unit> units = *(game->getUnits()); //all the units in the game
	CIwFVec2 dirToward; //vector from current repelling unit to this unit
	Unit* curUnit; //current repelling unit in loop

	for (std::list<Unit *>::iterator itr = units.backingList.begin() ; itr != units.backingList.end(); ++itr) {
		curUnit = *(itr);
		
		if (curUnit != this && curUnit->getType() != PROJECTILE) {
			dirToward = position - curUnit->getPosition();
			float dist = dirToward.GetLengthSquared();
			sumForces += dirToward.GetNormalised() * (curUnit->getSize() * REPEL_FACTOR / pow(dist, 1.875));
		}
	}
	
	return sumForces;
}

CIwFVec2 Unit::getCircularForce() {
	float rDiff = (game->getWorldRadius().y + game->getWorldRadius().x)/2.0 - r;
	return position * ((rDiff < 0 ? -1 : 1) * CIRCLE_SPRING * SQ(rDiff)); // Ternary is experimentally faster
}

CIwFVec2 Unit::getWallForce(CIwFVec2 sumForces) {
	CIwFVec2 worldRadius = game->getWorldRadius();
	CIwFVec2 normal;
	
	float toInner = getR() - worldRadius.x,
		  toOuter = worldRadius.y - getR(),
		  myRadius = getSize()/2,
		  dot;
	
	if (toInner <= myRadius) {
		normal = CIwFVec2(cos(getTheta()), sin(getTheta()));
		dot = sumForces.GetNormalised().Dot(-1 * normal);
		return normal * (dot > 0 ? dot : 0) * sumForces.GetLength();
	}
	else if(toOuter <= myRadius) {
		CIwFVec2 normal = CIwFVec2(-cos(getTheta()), -sin(getTheta()));
		dot = sumForces.GetNormalised().Dot(-1 * normal);
		return normal * (dot > 0 ? dot : 0) * sumForces.GetLength();
	}
	else {
		return CIwFVec2::g_Zero;
	}
}

CIwFVec2 Unit::getNavTargetForce() { return NAV_ATTRACT_FACTOR * (navTarget-position).GetNormalised(); }

void Unit::path() {
	CIwFVec2 repulsionSum, sumForces = CIwFVec2::g_Zero;
	
	repulsionSum = getRepulsiveForce();
	sumForces += repulsionSum;
	if (pathMode != ESCAPE) sumForces += getCircularForce();
	if (pathMode == TOLEADER) sumForces += getLeaderForce();
	else sumForces += getNavTargetForce();
	sumForces += getWallForce(sumForces);
	
	if (sumForces.GetLengthSquared() < FORCE_THRESHOLD && pathMode != ESCAPE) {
		s3eDebugOutputString("enter escape pathing");
		createEscapeTarget();
		pathMode = ESCAPE;
	}
	else if (pathMode == ESCAPE) {
		CIwFVec2 normal = -1 * (CIwFVec2(-sumForces.y, sumForces.x)).GetNormalised();
		if (repulsionSum.Dot(normal) < NORMAL_FORCE_THRESHOLD){
			pathMode = pathMode == OBJECTIVE ? OBJECTIVE : TOLEADER;
			s3eDebugOutputString("below normal threshold");
		} 
		else {
			updateEscapeTarget(sumForces);
		}
	}
	
	float speedAdjustment = (sumForces.GetLength())/NAV_ATTRACT_FACTOR;
	velocity = speed * (speedAdjustment) * sumForces.GetNormalised();
	setPosition(position + velocity);
}


int Unit::dirToEnemyAngular() {
	if (owner == game->getLocalPlayer()) return getTheta() > 0 ? 1 : -1;
	else return getTheta() > PI ? 1 : -1;
}


void Unit::createEscapeTarget() {
	CIwFVec2 worldRad = game->getWorldRadius();
	
	float innerDist = getR() - worldRad.x,
		  outerDist = worldRad.y - getR();
	
	if (innerDist > outerDist) navTarget.x = worldRad.x - getSize();
	else navTarget.x = worldRad.y + getSize();
	
	navTarget.y = getTheta() + .3 * dirToEnemyAngular(); 
	polarToXY(navTarget);
}

void Unit::updateEscapeTarget(CIwFVec2 sumForces) {
	polarize(navTarget);
	float direction = sumForces.GetLengthSquared() <  SQ(NAV_ATTRACT_FACTOR) ? -1 : 1;
	navTarget.y += .01 * direction * dirToEnemyAngular();
	polarToXY(navTarget);
}