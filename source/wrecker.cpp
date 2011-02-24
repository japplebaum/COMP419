#include "wrecker.h"

Wrecker::Wrecker(Player* owner, CIwFVec2 position, Game* game)
	: AttackingUnit(350.0f, 3.0f, 40.0f, owner, position, game) {
	numFrames = 6;
    scale = 0.2;
		
	texture_names.push_back(IwHashString("wrecker_walk_sprite_sheet"));
	texture_names.push_back(IwHashString("wrecker_attack_sprite_sheet"));
}
 
Wrecker::Wrecker(const Wrecker& newWrecker) : AttackingUnit(newWrecker) { }

void Wrecker::update() {
	curFrame = (curFrame + 1) % numFrames;
	path();
}

void Wrecker::setAttackSprite() {
	if(current_texture_index != 1) {
		UnitBucket buckets = *game->getUnitBucket();
		std::set<Unit*>* ourPool = buckets[texture_names[current_texture_index]];
		ourPool->erase(ourPool->find(this));
		buckets[texture_names[1]]->insert(this);
		numFrames = 8;
		current_texture_index = 1;
	}
}

void Wrecker::setIdleSprite() {
	if(current_texture_index != 0) {
		UnitBucket buckets = *game->getUnitBucket();
		std::set<Unit*>* ourPool = buckets[texture_names[current_texture_index]];
		ourPool->erase(ourPool->find(this));
		buckets[texture_names[0]]->insert(this);
		numFrames = 6;
		current_texture_index = 0;
	}
}

unit_type Wrecker::getType() {
	return WRECKER;
}

Unit* Wrecker::spawnCopy() {
	return new Wrecker(*this);
}

void Wrecker::attack(){ 

}