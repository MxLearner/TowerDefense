#include "Turret.h"
#include"GameScene.h"
#include"Bullet.h"
#define DEBUG
bool Turret::init()
{   // 每一帧追踪怪物
	_monster = nullptr;
	scheduleUpdate();
	// 创建并发射子弹
	this->schedule([this](float dt) {
		ShootBullet();
		},0.5f, "shootBullet");
	return true;
}
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

}

 void Turret::update(float dt) {
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
			 _monster = monster;
			 break; // 追踪最靠前的怪物，且只追一个
		 }
		 else {
			 _monster = nullptr;
		 }
	 }
 }

 void Turret::ShootBullet()
 {
	 if (_monster == nullptr||_monster->getLifeValue()<=0) {
		 return;
	 }
	 // 注意monster访问异常，严重bug！！！！！！！！！！！
	 // 最近没出现了，出现了记得及时记录！！！！！！！！！
	 //**********************************************
	 //**********************************************
	 Vec2 targetPos = _monster->getPosition();
	 std::string bulletName = getName() + "_bullet.png";
	 auto bullet = Bullet::createWithSpriteFrameName(bulletName);
	 bullet->setDamage(_damage);
	 bullet->setPosition(getPosition());
	 Vec2 temp = bullet->getPosition();
	 getParent()->addChild(bullet, 9); // 位置比塔基低一层这样可以盖住
	 
	 auto moveTo = MoveTo::create(0.2f, targetPos);
	 auto damageCallback = CallFunc::create([=]() {
		 if (_monster != nullptr&&_monster->getLifeValue()>0) {
			 int curLifeValue =_monster->getLifeValue() - bullet->getDamage();
			 _monster->setLifeValue(curLifeValue);
			 monster->setHP(curLifeValue);//更新血条
		 }});
		 
	 auto removeCallback = CallFunc::create([=]() {

		 bullet->removeFromParent();
		 });
	 auto sequence = Sequence::create(moveTo, damageCallback, nullptr);
	 bullet->runAction(sequence);
 }
