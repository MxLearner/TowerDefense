#ifndef _Turret_H_
#define _Turret_H_





#include"cocos2d.h"
#include"Bullet.h"
#include"Monster.h"

USING_NS_CC;


// 炮台类
class Turret :public Sprite {
protected:
	std::string _name;          // 炮塔名
	bool _select = false;      // 炮塔是否被安放
	Bullet* _bullet;           //  炮塔关联的炮弹对象
	int _level = 1;            // 当前等级
	int _range = 100;           // 攻击范围
	Monster* _monster = nullptr;  // 当前炮塔是否追踪到了怪物

	int _cost1;
	int _cost2;
	int _cost3;
	int _damage;
public:

	static Turret* createWithSpriteFrameName(const std::string name, const int grade) {
		// name 获取的是炮塔的名字，不是图片名字
		std::string baseName = name + "_base.png"; // 塔基名
		std::string turretName = name + "_" + std::to_string(grade) + ".png";
		Turret* sprite = new Turret();
		if (sprite && sprite->initWithSpriteFrameName(baseName)) {
			sprite->autorelease();
			auto turret = new Sprite();
			if (turret && turret->initWithSpriteFrameName(turretName)) {
				turret->setName("turret");
				sprite->addChild(turret);
			}
			// 炮台节点位置有点问题，回来调整
			// 因为TMX坐标转化是（0，0）->（0，640），所以将塔基锚点设为左上角，后续可以再改
			sprite->setAnchorPoint(Vec2(0.5, 0.5));
			turret->setAnchorPoint(Vec2(0.5, 0.5));// 炮台锚点设为中间，方便后续旋转 
			// 子节点这个坐标设置是基于父节点左下角为坐标系，单位是分辨率像素
			turret->setPosition(Vec2(sprite->getContentSize().width / 2.0, sprite->getContentSize().height / 2.0));// 先这样微调一下吧
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
	// 发射子弹
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
	void setLevel(int level) {
		_level = level;
	}
	int getLevel() {
		return _level;
	}
	void upgrade();
};

#endif // !_Turret_H_