#ifndef _Monster_H_
#define _Monster_H_






#include"ui/CocosGUI.h"
#include"cocos2d.h"
#include"Data/PointDelegate.h"
USING_NS_CC;
using namespace ui;

// Monster类
class Monster : public Sprite {
private:
	std::string _name;          // 怪物名
	float _lifeValue = 10;        // 生命值
	float _maxLifeValue = 10;     // 最大生命值
	LoadingBar* _HP;       // 进度条效果表示血条
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点
	int _gold = 10;         // 消灭怪物获得的金币
	float _speed = 1;        // 移动速度
	bool isLastPoint = false;  // 怪物到达终点？
	// 用于保存进度
	int _step = 0;   // 记录怪物走了几步

public:
	// 使用精灵帧创建
	static Monster* createWithSpriteFrameName(const std::string& filename) {
		Monster* sprite = new Monster();
		if (sprite && sprite->initWithSpriteFrameName(filename)) {
			sprite->autorelease();

			auto screedSize = Director::getInstance()->getVisibleSize();
			//添加血条
			sprite->_HP = LoadingBar::create("CarrotGuardRes/UI/HPbar.png");
			sprite->_HP->setPercent(100); // 初始血量为满血
			sprite->_HP->setPosition(Vec2(sprite->getContentSize().width / 2, sprite->getContentSize().height * 1.3f));
			sprite->addChild(sprite->_HP, 1);
			//添加血条背景
			auto HPBackground = Sprite::create("CarrotGuardRes/UI/HPBackground.png");
			HPBackground->setPosition(Vec2(sprite->getContentSize().width / 2, sprite->getContentSize().height * 1.3f));
			sprite->addChild(HPBackground, 0);
			return sprite;
		}
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}

	bool init();
	// set get 
	void setLifeValue(float lifeValue) {
		_lifeValue = lifeValue;
	}
	void setMaxLifeValue(float maxLifeValue) {
		_lifeValue = maxLifeValue;
		_maxLifeValue = maxLifeValue;
	}
	float getLifeValue() {
		return _lifeValue;
	}

	void setHP() {
		if (_lifeValue > 0) {
			float percent = _lifeValue / _maxLifeValue * 100.0f;
			_HP->setPercent(percent);
		}
	}

	void removeHP() {
		if (_HP) {
			_HP->removeFromParent();
			_HP = nullptr;
		}
	}

	LoadingBar* getHP() {
		return _HP;
	}
	void setPointPath(Vector<PointDelegate*>& pathPoints) {
		_pathPoints = pathPoints;
	}

	// monster移动
	void startMoving();

	void setGold(int gold) {
		_gold = gold;
	}
	int getGold() {
		return _gold;
	}
	void setSpeed(float speed) {
		_speed = speed;
	}
	bool getisLastPoint() {
		return isLastPoint;
	}
	void setStep(int step) {
		_step = step;
	}
	int getStep() {
		return _step;
	}
	float getSpeed() {
		return _speed;
	}
	float getMaxLifeValue() {
		return _maxLifeValue;
	}

	void setName(std::string name) {
		_name = name;
	}
	std::string getName() {
		return _name;
	}
};

#endif // !_Monster_H
