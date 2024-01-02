#pragma once

#include"cocos2d.h"

USING_NS_CC;


// Point代理类,
class PointDelegate : public Ref
{
private:
	float _x=0;
	float _y=0;

public:
	static PointDelegate* create(float x, float y)
	{
		PointDelegate* p = new PointDelegate();
		if (p && p->initPoint(x, y)) {
			p->autorelease();
			return p;
		}
		CC_SAFE_DELETE(p);
		return nullptr;
	}
	// 初始化赋值
	bool initPoint(float x, float y) {
		_x = x;
		_y = y;
		return true;
	}
	// set get
	void setX(float x) {
		_x = x;
	}
	float getX() {
		return _x;
	}

	void setY(float y) {
		_y = y;
	}
	float getY() {
		return _y;
	}

};