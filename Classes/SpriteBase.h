#pragma once

#include"cocos2d.h"


USING_NS_CC;

// 扩展精灵类
class SpriteBase :public Sprite {
private:
	std::string _name;  // 精灵名称
	int _gold;          // 精灵金币值

public: 
	static SpriteBase* create(const std::string filename) {
		SpriteBase* sprite = new SpriteBase();
		if (sprite && sprite->initWithFile(filename)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	static SpriteBase* createWithSpriteFrameName(const std::string& filename) {
		SpriteBase* sprite = new SpriteBase();
		if (sprite && sprite->initWithSpriteFrameName(filename)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	// set get
	void setName(std::string name) {
		_name = name;
	}
	std::string getName() {
		return _name;
	}

	void setGold(int gold) {
		_gold = gold;
	}
	int getGold() {
		return _gold;
	}
};
