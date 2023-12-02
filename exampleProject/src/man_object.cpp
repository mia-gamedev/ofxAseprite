#include "man_object.h"

man_object::man_object(ofVec2f position, ofVec2f size, const char* jsonFilePth, const char* spriteSheetPath)
	: position(position)
	, size(size)
	, velocity(10)
	, direction(0,0)
	, moveDirection(UP)
{
	this->SetSprite(jsonFilePth, spriteSheetPath);
	this->sprite->setTag("IdleRight");
}

void man_object::move(Direction direction)
{
	if (direction == UP) {
		this->direction.x = 0;
		this->direction.y = -1;
		this->sprite->setTag("WalkUp");
	}
	if (direction == DOWN) {
		this->direction.x = 0;
		this->direction.y = 1;
		this->sprite->setTag("WalkDown");
	}
	if (direction == LEFT) {
		this->direction.x = -1;
		this->direction.y = 0;
		this->sprite->setTag("WalkLeft");
	}
	if (direction == RIGHT) {
		this->direction.x = 1;
		this->direction.y = 0;
		this->sprite->setTag("WalkRight");
	}
	this->sprite->play();
	this->moveDirection = direction;
}

void man_object::stopMoving()
{
	this->direction.x = 0;
	this->direction.y = 0;

	if (this->moveDirection == UP)
	{
		this->sprite->setTag("IdleUp");
	}
	if (this->moveDirection == DOWN)
	{
		this->sprite->setTag("IdleDown");
	}
	if (this->moveDirection == LEFT)
	{
		this->sprite->setTag("IdleLeft");
	}
	if (this->moveDirection == RIGHT)
	{
		this->sprite->setTag("IdleRight");
	}
}

void man_object::draw()
{
	this->sprite->draw(this->position,10);
}

void man_object::update(float elapsedTime)
{
	this->position = this->position + this->direction * this->velocity;
	this->sprite->update(elapsedTime);
}

void man_object::SetSprite(const char* jsonFilePth, const char* spriteSheetPath)
{
	this->sprite = new ofxAseprite(jsonFilePth, spriteSheetPath);
}
