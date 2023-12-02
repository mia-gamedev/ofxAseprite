#pragma once

#include "ofxAseprite.h"

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

class man_object
{
public:
	ofVec2f position, direction, size;
	ofxAseprite *sprite;
	Direction moveDirection;
	float velocity;

	man_object(ofVec2f position, ofVec2f size, const char* jsonFilePth, const char* spriteSheetPath);

	void move(Direction direction);
	void stopMoving();
	void draw();
	void update(float elapsedTime);
	void SetSprite(const char* jsonFilePth, const char* spriteSheetPath);
};
