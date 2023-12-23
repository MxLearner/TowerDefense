#pragma once

#include"cocos2d.h"

USING_NS_CC;


// TurretData ���ڴ洢��̨��Ϣ
class TurretData :public Ref
{
private:
	std::string _name;       // ��̨����,���ļ�·����eg��"TB"
	int _cost1;               // ��������������
	int _cost2;
	int _cost3;
	float _range;
	int _damage;
public:

	// ����TurretData
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

	// ˽�г�Ա����������set����ȡget
	void setName(std::string name) {
		_name = name;
	}
	std::string getName() {
		return _name;
	}

	void setCost1(int cost1) {
		_cost1 = cost1;
	}
	int getCost1() {
		return _cost1;
	}

	void setCost2(int cost2) {
		_cost2 = cost2;
	}
	int getCost2() {
		return _cost2;
	}
	void setCost3(int cost3) {
		_cost3 = cost3;
	}
	int getCost3() {
		return _cost3;
	}

	void setRange(float range) {
		_range = range;
	}
	float getRange() {
		return _range;
	}
	void setDamage(int damage) {
		_damage = damage;
	}
	int getDamage() {
		return _damage;
	}
};