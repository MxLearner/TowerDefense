#include "Turret.h"
#include"GameScene.h"
#include"Bullet.h"
#define DEBUG
bool Turret::init()
{   // ÿһ֡׷�ٹ���
	_monster = nullptr;
	scheduleUpdate();
	// �����������ӵ�
	this->schedule([this](float dt) {
		ShootBullet();
		}, 0.5f, "shootBullet");
	return true;
}
void Turret::ShootAtMonster(Monster* target) {
	// ��������ת��Ƕ�
	Vec2 targetPos = target->getPosition();
	Vec2 turretPos = getPosition();
	// ���Ǽ������������ļнǣ�����
	//float angle = turretPos.getAngle(targetPos);// ��λ�ǻ���
	// ���治��
	// ת����������� 
	targetPos.x -= turretPos.x;
	targetPos.y -= turretPos.y;
	// Ĭ����������ǹ������ָ����ô��׼����Ϊ��0��1����
	turretPos = Vec2(0, 1);
	// ��ת��Ϊ�Ƕ�
	float angle=CC_RADIANS_TO_DEGREES(turretPos.getAngle(targetPos));
	// ����������ת
	//setRotation ����ʹ�õ�����ʱ����ת������ϵ����������
	// ����-1.0*angle �ǶԵģ���Ҳ��֪��Ϊʲô
	this->getChildByName("turret")->setRotation(-1.0*angle);

}

 void Turret::update(float dt) {
	 // ��ȡ��ǰ����
	 auto Scene = Director::getInstance()->getRunningScene();
	 // ��ȡ layer ����
	 auto layer = dynamic_cast<GameScene*>(Scene->getChildByName("layer"));
	 const auto& monsters = layer->getMonsters();
	 for (const auto& monster : monsters) {
		 float distance = getPosition().distance(monster->getPosition());

		 // ����Ƿ��������
		 if (distance <= _range) {
			 // ������������
			 ShootAtMonster(monster);
			 _monster = monster;
			 break; // ׷���ǰ�Ĺ����ֻ׷һ��
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
	 // ע��monster�����쳣������bug����������������������
	 //**********************************************
	 //**********************************************
	 Vec2 targetPos = _monster->getPosition();
	 Monster* monster = _monster;
	 std::string bulletName = getName() + "_bullet.png";
	 auto bullet = Bullet::createWithSpriteFrameName(bulletName);
	 bullet->setPosition(getPosition());
	 Vec2 temp = bullet->getPosition();
	 getParent()->addChild(bullet, 9); // λ�ñ�������һ���������Ը�ס
	 
	 auto moveTo = MoveTo::create(0.2f, targetPos);
	 auto damageCallback = CallFunc::create([=]() {
		 if (monster != nullptr) {
			 int curLifeValue = monster->getLifeValue() - bullet->getDamage();
			 monster->setLifeValue(curLifeValue);
	         monster->setHP(curLifeValue);//����Ѫ��

		 }
		 bullet->removeFromParent();
		 });

	 auto sequence = Sequence::create(moveTo, damageCallback, nullptr);
	 bullet->runAction(sequence);
	 
 }
