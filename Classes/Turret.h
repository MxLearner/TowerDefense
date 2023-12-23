#ifndef _Turret_H_
#define _Turret_H_





#include"cocos2d.h"
#include"Bullet.h"
#include"Monster.h"

USING_NS_CC;


// ��̨��
class Turret :public Sprite {
private:
	std::string _name;          // ������
	bool _select = false;      // �����Ƿ񱻰���
	Bullet* _bullet;           //  �����������ڵ�����
	int _buildGold=100;         // ����������
	int _updateGold = 100;     // ����������
	int _level = 1;            // ��ǰ�ȼ�
	int _range = 100;           // ������Χ
	Monster* _monster = nullptr;  // ��ǰ�����Ƿ�׷�ٵ��˹���
 
public:

	static Turret* createWithSpriteFrameName(const std::string name) {
		// name ��ȡ�������������֣�����ͼƬ����
		std::string baseName = name + "_base.png"; // ������
		std::string turretName = name + "_1.png";  // ��ʼ�ȼ�Ϊ1������
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
			turret->setPosition(Vec2(20, 20));// ������΢��һ�°�
			sprite->setScale(2.0);
			return sprite;
		}
		CCLOG("Cannot Build Turret");
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
    bool init();


	void ShootAtMonster(Monster* target);
	void update(float dt);
	// �����ӵ�
	void ShootBullet();
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
};

#endif // !_Turret_H_