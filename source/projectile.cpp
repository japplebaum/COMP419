#include "projectile.h"

Projectile::Projectile(Player* owner, CIwFVec2 position, Game* game)
			: AttackingUnit(100.0f, 50.0f, 10.0f, 10.0f, owner, position, game)
{
	numFrames = 8;
    scale = 0.1f;
	texture_names.push_back(IwHashString("projectile_sprite_sheet"));
	worldRad = game->getWorldRadius();
	minTargetDist = SQ(target->getSize()/2);
}

void Projectile::update() {
    curFrame = (curFrame + 1) % numFrames;
	setPosition(position + speed*velocity);
}

unit_type Projectile::getType() {
	return PROJECTILE;
}

void Projectile::attack() {
	
}
