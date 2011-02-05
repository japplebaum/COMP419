#include "thrower.h"

Thrower::Thrower(Player* owner, Game* game, float x, float y)
	: Unit(200.0f, 300.0f, 0.0f, 8.0f, 20.0f, 60.0f, 80.0f, 0.0f, 0.0f, owner, game)
{
    numFrames = 8;
    scale = 0.5f;
    setPosition(x, y);
	texture_names.push_back(IwHashString("thrower_walk_sprite_sheet"));
    framesUntilUpdate = 0;
}

Thrower::Thrower(const Thrower& newThrower) : Unit(newThrower) { }

bool Thrower::update(std::list<Unit*>::iterator itr){   
    curFrame = (curFrame + 1) % numFrames;
	return true;
}

unit_type Thrower::getType() {
	return THROWER;
}

Unit* Thrower::spawnCopy() {
    return new Thrower(*this);
} 

void Thrower::attack(){
    if((target->getPosition()-position).GetLength() <= range){
        target->receiveDamage(getDamage(target), this);
    }
}
