#pragma once

#include"cocos2d.h"
#include"LevelData.h"


USING_NS_CC;
// 关卡管理类
class LevelManager:public Layer
{
protected:
	int _SelectLevelIndex;


public:
	static Scene* createScene();
	virtual bool init();
	// 选择器的回调
	void menuCloseCallback(Ref* pSender);

	CREATE_FUNC(LevelManager);

};

