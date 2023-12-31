#pragma once

/*
* 内容：探险模式场景的头文件
*/



#ifndef __ADVANTURE_SCENE_H__
#define __ADVANTURE_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class AdvantureScene : public Layer {
public:
	static Scene* createScene();
	virtual bool init();

	void ToMenuScene(Ref* pSender);
	void ToSkyLineSelection(Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(AdvantureScene);


};

#endif __ADVANTURE_SCENE_H__
