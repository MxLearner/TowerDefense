#include "Turret.h"
#include"GameScene.h"
#define DEBUG
 void Turret::ShootAtMonster(Monster* target) {
	// 计算炮塔转向角度
	Vec2 targetPos = target->getPosition();
	Vec2 turretPos = getPosition();
	// 这是计算两个向量的夹角！！！
	//float angle = turretPos.getAngle(targetPos);// 单位是弧度
	// 上面不对
	// 转化成相对向量 
	targetPos.x -= turretPos.x;
	targetPos.y -= turretPos.y;
	// 默认所有炮塔枪口向上指，那么基准向量为（0，1）；
	turretPos = Vec2(0, 1);
	// 在转化为角度
	float angle=CC_RADIANS_TO_DEGREES(turretPos.getAngle(targetPos));
	// 设置炮塔旋转
	//setRotation 函数使用的是逆时针旋转的坐标系！！！！！
	// 反正-1.0*angle 是对的，我也不知道为什么
	this->getChildByName("turret")->setRotation(-1.0*angle);
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
