#include "Turret.h"
#include"cocos2d.h"
#include"Bullet.h"
#include"Monster.h"
#define DEBUG

USING_NS_CC;

class Turret_TSun :public Turret {

public:

	static Turret_TSun* createWithSpriteFrameName(const std::string name,const int grade) {
		// name 获取的是炮塔的名字，不是图片名字
		std::string baseName = name + "_base.png"; // 塔基名
		std::string turretName = name + "_" + std::to_string(grade) + ".png";
		Turret_TSun* sprite = new Turret_TSun();
		if (sprite && sprite->initWithSpriteFrameName(baseName)) {
			sprite->autorelease();
			auto turret = new Sprite();
			if (turret && turret->initWithSpriteFrameName(turretName)) {
				turret->setName("turret");
				sprite->addChild(turret);
			}
			// 炮台节点位置有点问题，回来调整
            // 因为TMX坐标转化是（0，0）->（0，640），所以将塔基锚点设为左上角，后续可以再改
			sprite->setAnchorPoint(Vec2(0.5, 0.5));
			turret->setAnchorPoint(Vec2(0.5, 0.5));// 炮台锚点设为中间，方便后续旋转 
			// 子节点这个坐标设置是基于父节点左下角为坐标系，单位是分辨率像素
			turret->setPosition(Vec2(sprite->getContentSize().width/2.0, sprite->getContentSize().height/2.0));// 先这样微调一下吧
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