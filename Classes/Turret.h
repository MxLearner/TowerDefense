#ifndef _Turret_H_
#define _Turret_H_





#include"cocos2d.h"
#include"Bullet.h"
#include"Monster.h"

USING_NS_CC;


// ��̨��
class Turret :public Sprite {
protected:
	std::string _name;          // ������
	bool _select = false;      // �����Ƿ񱻰���
	Bullet* _bullet;           //  �����������ڵ�����
	int _buildGold = 100;         // ����������
	int _updateGold = 100;     // ����������
	int _level = 1;            // ��ǰ�ȼ�
	int _range = 100;           // ������Χ
	Monster* _monster = nullptr;  // ��ǰ�����Ƿ�׷�ٵ��˹���
	float _shootFreq = 0.5;

	int _cost1;
	int _cost2;
	int _cost3;
	int _damage;
public:

	static Turret* createWithSpriteFrameName(const std::string name, const int grade) {
		// name ��ȡ�������������֣�����ͼƬ����
		std::string baseName = name + "_base.png"; // ������
		std::string turretName = name + "_" + std::to_string(grade) + ".png";
		Turret* sprite = new Turret();
		if (sprite && sprite->initWithSpriteFrameName(baseName)) {
			sprite->autorelease();
			auto turret = new Sprite();
			if (turret && turret->initWithSpriteFrameName(turretName)) {
				turret->setName("turret");
				sprite->addChild(turret);
			}
			// ��̨�ڵ�λ���е����⣬��������
			// ��ΪTMX����ת���ǣ�0��0��->��0��640�������Խ�����ê����Ϊ���Ͻǣ����������ٸ�
			sprite->setAnchorPoint(Vec2(0.5, 0.5));
			turret->setAnchorPoint(Vec2(0.5, 0.5));// ��̨ê����Ϊ�м䣬���������ת 
			// �ӽڵ�������������ǻ��ڸ��ڵ����½�Ϊ����ϵ����λ�Ƿֱ�������
			turret->setPosition(Vec2(sprite->getContentSize().width / 2.0, sprite->getContentSize().height / 2.0));// ������΢��һ�°�
			sprite->setScale(2.0);
			return sprite;
		}
		CCLOG("Cannot Build Turret");
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	virtual bool init() override;


	void ShootAtMonster(Monster* target);
	void update(float dt);
	// �����ӵ�
	virtual void ShootBullet();
	// set get

	void setName(std::string name) {
		_name = name;
	}
	std::string getName() {
		return _name;
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

	void setRange(int range) {
		_range = range;
	}
	int getRange() {
		return _range;
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
	void setDamage(int damage) {
		_damage = damage;
	}
	int getDamage() {
		return _damage;
	}
	void upgrade();

	void setShootFreq(float freq) {
		_shootFreq = freq;
	}
	float getShootFreq() {
		return _shootFreq;
	}
};

#endif // !_Turret_H_