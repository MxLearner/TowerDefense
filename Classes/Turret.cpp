#include "Turret.h"
#include"GameScene.h"
#define DEBUG
 void Turret::ShootAtMonster(Monster* target) {
	// 计算炮塔转向角度
	Vec2 targetPos = target->getPosition();
	Vec2 turretPos = getPosition();
	float angle = CC_RADIANS_TO_DEGREES(turretPos.getAngle(targetPos));
	// 设置炮塔旋转
	this->getChildByName("turret")->setRotation(angle);
	// 创建并发射子弹
	//////////////////////
}

 void Turret::update(float dt) {
	 // 获取当前场景
	 auto Scene = Director::getInstance()->getRunningScene();
	 // 获取 layer 对象
	 auto layer = dynamic_cast<GameScene*>(Scene->getChildByName("layer"));
	 const auto& monsters = layer->getMonsters();
	 for (const auto& monster : monsters) {
		 float distance = getPosition().distance(monster->getPosition());
#ifdef DEBUG
		 CCLOG("distance=%f", distance);
#endif // DEBUG

		 // 检测是否在射程内
		 if (distance <= _range) {
			 // 炮塔攻击怪物
			 ShootAtMonster(monster);
			 break; // 追踪最靠前的怪物，且只追一个
		 }
	 }
 }
