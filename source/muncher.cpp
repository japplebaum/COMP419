#include "muncher.h"

Muncher::Muncher(Player* owner, CIwFVec2 position, Game* game)
		 : Unit(100.0f, 4.0f, owner, position, game)
{
	numFrames = 12;
	scale = .2f;
	texture_names.push_back(IwHashString("muncher_sprite_sheet"));
}

Muncher::Muncher(const Muncher& newMuncher) : Unit(newMuncher) { }

void Muncher::update() {
    curFrame = (curFrame + 1) % numFrames;   
	path();
}

Unit* Muncher::spawnCopy() {
	return new Muncher(*this);
}

unit_type Muncher::getType() {
	return MUNCHER;
}
