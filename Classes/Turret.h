#pragma once

#include"cocos2d.h"
#include"SpriteBase.h"
#include"Bullet.h"

USING_NS_CC;


// 炮台类
class Turret :public SpriteBase {
private:
	std::string _bulletName;   // 对应的子弹名
	bool _select = false;      // 炮塔是否被安放
	Bullet* _bullet;           //  炮塔关联的炮弹对象

public:
	static Turret* create(const std::string& filename) {
		Turret* sprite = new Turret();
		if (sprite && sprite->initWithFile(filename)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	static Turret* createWithSpriteFrameName(const std::string& filename) {
		Turret* sprite = new Turret();
		if (sprite && sprite->initWithFile(filename)) {
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	// set get

	void setBulletName(std::string bulletName) {
		_bulletName = bulletName;
	}
	std::string getBulletName() {
		return _bulletName;
	}

	void setSelect(bool select) {
		_select = select;
	}
	bool getSelect() {
		return _select;
	}

	void setBullet(Bullet* bullet) {
		_bullet = bullet;
	}
	Bullet* getBullet() {
		return _bullet;
	}
};