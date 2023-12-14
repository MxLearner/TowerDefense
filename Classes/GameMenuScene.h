#pragma once


#include"cocos2d.h"


USING_NS_CC;

#define RESOURCESPLIST ""
#define MUSIC_FILE ""
#define EFFECT_FILE1 ""
#define EFFECT_FILE2 ""

// ”Œœ∑≤Àµ•¿‡
class GameMenu :public Layer {
public:
	static Scene* createScene();
	virtual bool init();
	void MenuCloseCallback(Ref* pSender);
	CREATE_FUNC(GameMenu);
};