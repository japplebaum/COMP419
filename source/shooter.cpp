#include "shooter.h"
#include "projectile.h"

Shooter::Shooter(Player* owner, CIwFVec2 position, Game* game)
	: AttackingUnit(200.0f, 250.0f, 50.0f, 0.0f, owner, position, game)
{
	numFrames = 7;
    scale = 0.35;
	texture_names.push_back(IwHashString("shooter_sprite_sheet"));
}

Shooter::Shooter(const Shooter& newShooter) : AttackingUnit(newShooter) { }

void Shooter::update(){
    curFrame = (curFrame + 1) % numFrames;
}

unit_type Shooter::getType() {
	return SHOOTER;
}

Unit* Shooter::spawnCopy() {
	return new Shooter(*this);
}

void Shooter::attack(){
	
}

