#pragma once

#include"cocos2d.h"

USING_NS_CC;



// MonsterData�࣬�洢������Ϣ
class MonsterData :public Ref 
{
private:
	std::string _name; // ��������
	int _lifeValue;    // ��������
	int _gold;         // ��������õĽ��

public:
	// ����MonsterData
	static MonsterData* create()
	{
		MonsterData* sprite = new MonsterData();
		if (sprite) 
		{
			sprite->autorelease();
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	// ˽�г�Ա����������set����ȡget
	void setName(std::string name) {
		_name = name;
	}
	std::string getName() {
		return _name;
	}

	void setLifeValue(int lifeValue) {
		_lifeValue = lifeValue;
	}
	int getLifeValue() {
		return _lifeValue;
	}

	void setGold(int gold) {
		_gold = gold;
	}
	int getGold() {
		return _gold;
	}




};