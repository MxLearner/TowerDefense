#pragma once

#include"cocos2d.h"

USING_NS_CC;



// MonsterData类，存储怪物信息
class MonsterData :public Ref
{
private:
	std::string _name = ""; // 怪物名称,存储图片路径
	int _lifeValue = 1;    // 怪物生命
	int _gold = 10;         // 消灭怪物获得的金币
	float _speed = 1;      //  移动速度 
	//用于保存进度
	int _curLifeValue;    // 怪物当前生命
	Vec2 _position;      // 位置
	int _step;             // 已走步数
	Vec2 position;		// 位置


public:
	// 创造MonsterData
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
	// 私有成员变量的设置set，获取get
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

	void setSpeed(float speed) {
		_speed = speed;
	}
	float getSpeed() {
		return _speed;
	}

	void setCurLifeValue(int curLifeValue) {
		_curLifeValue = curLifeValue;
	}
	int getCurLifeValue() {
		return _curLifeValue;
	}
	void setposition(Vec2 position) {
		_position = position;
	}
	Vec2 getposition() {
		return _position;
	}
	void setStep(int step) {
		_step = step;
	}
	int getStep() {
		return _step;
	}
};