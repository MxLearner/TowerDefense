#pragma once

/*
* 内容：菜单场景的头文件
*/



#ifndef __MENU_SCENE_H__
#define __MENU_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class MenuScene : public Layer {
public:
	static Scene* createScene();
	virtual bool init();

	// a selector callback
	void menuCloseCallback(Ref* pSender);
	void ToAdvantureScene(Ref* pSender);
	void ToSettingScene(Ref* pSender);
	void ToClientGameScene(Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(MenuScene);


};

#endif __MENU_SCENE_H__
