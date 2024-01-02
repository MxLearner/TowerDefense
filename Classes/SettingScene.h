#pragma once

/*
* 内容：设置菜单场景的头文件
*/



#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class SettingScene : public Layer {
public:
    static Scene* createScene();
    virtual bool init();

    // a selector callback
    void ToMenuScene(Ref* pSender);
    void PlayBGM(Ref* pSender);
    void PlaySoundEffect(Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(SettingScene);


};

#endif __SETTING_SCENE_H__
