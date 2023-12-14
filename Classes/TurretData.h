#pragma once

#include"cocos2d.h"

USING_NS_CC;


// TurretData 用于存储炮台信息
class TurretData :public Ref
{
private:
	std::string _name;       // 炮台名称
	int _gold;               // 建造所需金币
	std::string _bulletName;  // 炮台发射子弹名

public:

	// 创造TurretData
	static TurretData* create()
	{
		TurretData* sprite = new TurretData();
		if (sprite) 
		{
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	// 私有成员变量的设置set，获取get
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

	void setBulletName(std::string bulletName) {
		_bulletName = bulletName;
	}
	std::string getBulletName() {
		return _bulletName;
	}
};