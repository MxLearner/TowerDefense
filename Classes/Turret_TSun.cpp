#include "Turret_TSun.h"
#include"GameScene.h"
#include"Bullet.h"


bool Turret_TSun::init()
{   // ÿһ֡׷�ٹ���
	_monster = nullptr;
	scheduleUpdate();
	// �����������ӵ�
	this->schedule([this](float dt) {
		ShootBullet();
		},0.5f, "shootBullet");
	return true;
}

void Turret_TSun::ShootBullet()
{
	Monster* monster1 = nullptr;
	// ��ȡ��ǰ����
	auto Scene = Director::getInstance()->getRunningScene();
	// ��ȡ layer ����
	auto layer = dynamic_cast<GameScene*>(Scene->getChildByName("layer"));
	const auto& monsters = layer->getMonsters();
	for (const auto& monster : monsters) {
		float distance = getPosition().distance(monster->getPosition());

		// ����Ƿ��������
		if (distance <= _range) {
			monster1 = monster;
		}
	}
	if (monster1 != nullptr) {
	 Vec2 targetPos = _monster->getPosition();
	 std::string bulletName = getName() + "_bullet.png";
	 auto bullet = Bullet::createWithSpriteFrameName(bulletName);
	 bullet->setDamage(_damage);
	 bullet->setPosition(getPosition());
	 Vec2 temp = bullet->getPosition();
	 getParent()->addChild(bullet, 9); // λ�ñ�������һ���������Ը�ס
	 bullet->setContentSize(Size(0, 0));
	 auto sizeAction = ResizeTo::create(0.3f, Size(_range*2.0, _range*2.0)); // ע��range�ǰ뾶����СҪ��Ϊֱ��

	 auto removeCallback = CallFunc::create([=]() {
		for (const auto& monster : monsters) {
			float distance = getPosition().distance(monster->getPosition());

			// ����Ƿ��������
			if (distance <= _range) {
				int curLifeValue = monster->getLifeValue() - _damage;
				monster->setHPInterval((float)(monster->getLifeValue() - curLifeValue));
				monster->setLifeValue(monster->getLifeValue() - _damage);
				monster->setHP();
			}
		}
		bullet->removeFromParent();
		});
	 auto sequence = Sequence::create(sizeAction, removeCallback, nullptr);
	 bullet->runAction(sequence);

	}
}

