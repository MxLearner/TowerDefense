#pragma once

#include"cocos2d.h"
#include"Bullet.h"
#include"GameScene.h"
#include"Monster.h"

USING_NS_CC;


// 炮台类
class Turret :public Sprite {
private:
	std::string _name;          // 炮塔名
	bool _select = false;      // 炮塔是否被安放
	Bullet* _bullet;           //  炮塔关联的炮弹对象
	int _buildGold=100;         // 建造所需金币
	int _updateGold = 100;     // 升级所需金币
	int _level = 1;            // 当前等级
	int _range = 100;           // 攻击范围
 
public:

	static Turret* createWithSpriteFrameName(const std::string name) {
		// name 获取的是炮塔的名字，不是图片名字
		std::string baseName = name + "_base.png"; // 塔基名
		std::string turretName = name + "_1.png";  // 初始等级为1的炮塔
		Turret* sprite = new Turret();
		if (sprite && sprite->initWithSpriteFrameName(baseName)) {
			sprite->autorelease();
			auto turret = new Sprite();
			if (turret && turret->initWithSpriteFrameName(turretName)) {
				sprite->addChild(turret);
			}
			// 炮台节点位置有点问题，回来调整
            // 因为TMX坐标转化是（0，0）->（0，640），所以将塔基锚点设为左上角，后续可以再改
			sprite->setAnchorPoint(Vec2(0, 1));
			turret->setAnchorPoint(Vec2(0.5, 0.5));// 炮台锚点设为中间，方便后续旋转 
			// 子节点这个坐标设置是基于父节点左下角为坐标系，单位是分辨率像素
			turret->setPosition(Vec2(20, 20));// 先这样微调一下吧
			sprite->setScale(2.0);
			return sprite;
		}
		CCLOG("Cannot Build Turret");
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	void ShootAtMonster(Monster* target) {
		// 计算炮塔转向角度
		Vec2 targetPos = target->getPosition();
		Vec2 turretPos = getPosition();
		float angle = CC_RADIANS_TO_DEGREES(turretPos.getAngle(targetPos));
		// 设置炮塔旋转
		setRotation(angle);
		// 创建并发射子弹
		//////////////////////
	}
	void update(float dt) {
		// 获取当前场景
        auto Scene = Director::getInstance()->getRunningScene();
        // 获取 layer 对象
        auto layer = dynamic_cast<GameScene*>(Scene->getChildByName("layer"));
		const auto& monsters = layer->getMonsters();
		for (const auto& monster : monsters) {
			float distance = getPosition().distance(monster->getPosition());

			// 检测是否在射程内
			if (distance <= _range) {
				// 炮塔攻击怪物
				ShootAtMonster(monster);
			}
		}
	}


	// set get

	void setName(std::string name) {
		_name = name;
	}
	std::string getname() {
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