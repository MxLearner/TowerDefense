#pragma once

/*
* 内容：探险模式场景的头文件
*/



#ifndef __SKYLINE_SCENE_H__
#define __SKYLINE_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class SkyLineSelection : public Layer {
	\
private:
	int _selectLevelIndex;

public:
	static Scene* createScene();
	virtual bool init();

	void ToAdvantureScene(Ref* pSender);

	// implement the "static create()" method manually
	CREATE_FUNC(SkyLineSelection);

	int getSelectLevelIndex() {
		return _selectLevelIndex;
	}
	void setSelectLevelIndex(int index) {
		_selectLevelIndex = index;
	}

};

#endif __SKYLINE_SCENE_H__
