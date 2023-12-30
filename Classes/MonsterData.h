#pragma once

#include"cocos2d.h"

USING_NS_CC;



// MonsterData�࣬�洢������Ϣ
class MonsterData :public Ref
{
private:
	std::string _name = ""; // ��������,�洢ͼƬ·��
	int _lifeValue = 1;    // ��������
	int _gold = 10;         // ��������õĽ��
	float _speed = 1;      //  �ƶ��ٶ� 
	//���ڱ������
	int _curLifeValue;    // ���ﵱǰ����
	Vec2 _position;      // λ��
	int _step;             // ���߲���


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

	void setSpeed(float speed) {
		_speed = speed;
	}
	float getSpeed() {
		return _speed;
	}



};