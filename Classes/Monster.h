#ifndef _Monster_H_
#define _Monster_H_






#include"ui/CocosGUI.h"
#include"cocos2d.h"
#include"PointDelegate.h"
USING_NS_CC;
using namespace ui;

// Monster��
class Monster : public Sprite {
private:
	int _lifeValue=10;        // ����ֵ
	LoadingBar* _HP;       // ������Ч����ʾѪ��
	float _HPInterval;     //  Ѫ��������
	Vector<PointDelegate*> _pathPoints;   // ��¼��Ч·����
	int _gold=10;         // ��������õĽ��
	float _speed=1;        // �ƶ��ٶ�
	bool isLastPoint = false;  // ���ﵽ���յ㣿

public:
	// ʹ�þ���֡����
	static Monster* createWithSpriteFrameName(const std::string& filename) {
		Monster* sprite = new Monster();
		if (sprite && sprite->initWithSpriteFrameName(filename)) {
			sprite->autorelease();

			auto screedSize = Director::getInstance()->getVisibleSize();
			//���Ѫ��
			sprite->_HP = LoadingBar::create("CarrotGuardRes/UI/HPbar.png");
			sprite->_HP->setPercent(100); // ��ʼѪ��Ϊ��Ѫ
			sprite->_HP->setPosition(Vec2(sprite->getContentSize().width / 2, sprite->getContentSize().height * 1.3f));
			sprite->addChild(sprite->_HP, 1);
			//���Ѫ������
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
	void setLifeValue(int lifeValue) {
		_lifeValue = lifeValue;
	}
	int getLifeValue() {
		return _lifeValue;
	}

	void setHP() {
		float percent = static_cast<float>(_lifeValue-_HPInterval) / _lifeValue * 100.0f;
		_HP->setPercent(percent);
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

	void setHPInterval(float HPInterval) {
		_HPInterval = HPInterval;
	}
	float getHPInterval() {
		return _HPInterval;
	}


	void setPointPath(Vector<PointDelegate*> &pathPoints) {
		_pathPoints = pathPoints;
	}
	// ����get����

	// monster�ƶ�
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

};

#endif // !_Monster_H
