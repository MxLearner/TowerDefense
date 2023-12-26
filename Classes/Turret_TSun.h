#include "Turret.h"
#include"cocos2d.h"
#include"Bullet.h"
#include"Monster.h"
#define DEBUG

USING_NS_CC;

class Turret_TSun :public Turret {

public:

	static Turret_TSun* createWithSpriteFrameName(const std::string name,const int grade) {
		// name ��ȡ�������������֣�����ͼƬ����
		std::string baseName = name + "_base.png"; // ������
		std::string turretName = name + "_" + std::to_string(grade) + ".png";
		Turret_TSun* sprite = new Turret_TSun();
		if (sprite && sprite->initWithSpriteFrameName(baseName)) {
			sprite->autorelease();
			auto turret = new Sprite();
			if (turret && turret->initWithSpriteFrameName(turretName)) {
				turret->setName("turret");
				sprite->addChild(turret);
			}
			// ��̨�ڵ�λ���е����⣬��������
            // ��ΪTMX����ת���ǣ�0��0��->��0��640�������Խ�����ê����Ϊ���Ͻǣ����������ٸ�
			sprite->setAnchorPoint(Vec2(0.5, 0.5));
			turret->setAnchorPoint(Vec2(0.5, 0.5));// ��̨ê����Ϊ�м䣬���������ת 
			// �ӽڵ�������������ǻ��ڸ��ڵ����½�Ϊ����ϵ����λ�Ƿֱ�������
			turret->setPosition(Vec2(sprite->getContentSize().width/2.0, sprite->getContentSize().height/2.0));// ������΢��һ�°�
			sprite->setScale(2.0);
			return sprite;
		}
		CCLOG("Cannot Build Turret");
		CC_SAFE_DELETE(sprite);
		return nullptr;
	}
	virtual bool init() override;
	virtual void ShootBullet() override;
};