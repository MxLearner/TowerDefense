#include "Turret_TSun.h"
#include"GameScene.h"
#include"Bullet.h"
#include "Music.h"

bool Turret_TSun::init()
{   // 每一帧追踪怪物
	_monster = nullptr;
	scheduleUpdate();
	// 创建并发射子弹
	this->schedule([this](float dt) {
		ShootBullet();
		}, 0.5f, "shootBullet");
	return true;
}

void Turret_TSun::ShootBullet()
{
	Monster* monster1 = nullptr;
	// 获取当前场景
	auto Scene = Director::getInstance()->getRunningScene();
	// 获取 layer 对象
	auto layer = dynamic_cast<GameScene*>(Scene->getChildByName("layer"));
	const auto& monsters = layer->getMonsters();
	for (const auto& monster : monsters) {
		float distance = getPosition().distance(monster->getPosition());

		// 检测是否在射程内
		if (distance <= _range) {
			monster1 = monster;
		}
	}
	if (monster1 != nullptr) {
		Vec2 targetPos = monster1->getPosition();
		std::string bulletName = getName() + "_bullet.png";
		auto bullet = Bullet::createWithSpriteFrameName(bulletName);
		bullet->setDamage(_damage);
		bullet->setPosition(getPosition());
		bullet->setName(bulletName);
		Vec2 temp = bullet->getPosition();
		getParent()->addChild(bullet, 9); // 位置比塔基低一层这样可以盖住
		bullet->setContentSize(Size(0, 0));
		auto sizeAction = ResizeTo::create(0.3f, Size(_range * 2.0, _range * 2.0)); // 注意range是半径，大小要设为直径

		auto removeCallback = CallFunc::create([=]() {
			for (const auto& monster : monsters) {
				if (monster != nullptr) {


					float distance = getPosition().distance(monster->getPosition());
					// 检测是否在射程内
					if (distance <= _range) {
						if (getName() == "TSun")
							MusicManager::getInstance()->sunSound();
						monster->setLifeValue(monster->getLifeValue() - _damage);
					}
				}
				bullet->removeFromParent();
			}
			});
		auto sequence = Sequence::create(sizeAction, removeCallback, nullptr);
		bullet->runAction(sequence);

	}
}

