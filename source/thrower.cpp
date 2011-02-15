#include "thrower.h"

Thrower::Thrower(Player* owner, CIwFVec2 position, Game* game)
	: AttackingUnit(200.0f, 300.0f, 0.0f, 8.0f, owner, position, game)
{
    numFrames = 8;
    scale = 0.5f;
	texture_names.push_back(IwHashString("thrower_walk_sprite_sheet"));
    framesUntilUpdate = 0;
}

Thrower::Thrower(const Thrower& newThrower) : AttackingUnit(newThrower) { }

void Thrower::update(){   
    curFrame = (curFrame + 1) % numFrames;
}

unit_type Thrower::getType() {
	return THROWER;
}

Unit* Thrower::spawnCopy() {
    return new Thrower(*this);
} 

void Thrower::attack() {

}
