#pragma once



#include"cocos2d.h"
#include"SpriteBase.h"


USING_NS_CC;

// 子弹类
class Bullet :public SpriteBase {
private:
	bool _shoot = false; // 炮弹是否发射

public:
	static Bullet* create(const std::string& filename) {
		Bullet* sprite = new Bullet();
		if (sprite && sprite->initWithFile(filename)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	static Bullet* createWithSpriteFrameName(const std::string& filename) {
		Bullet* sprite = new Bullet();
		if (sprite && sprite->initWithFile(filename)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}



	// set get 
	void setShoot(bool shoot) {
		_shoot = shoot;
	}
	bool isShoot() {
		return _shoot;
	}
};