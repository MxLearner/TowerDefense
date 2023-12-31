#pragma once



#include"cocos2d.h"


USING_NS_CC;

// ×Óµ¯Àà
class Bullet :public Sprite {
private:
	int _damage = 1;     // ¹¥»÷Á¦

public:
	static Bullet* createWithSpriteFrameName(const std::string& filename) {
		Bullet* sprite = new Bullet();
		if (sprite && sprite->initWithSpriteFrameName(filename)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}



	// set get 
	void setDamage(int damage) {
		_damage = damage;
	}
	int getDamage() {
		return _damage;
	}
};