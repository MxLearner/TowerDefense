#pragma once

/*
* ���ݣ����ò˵�������ͷ�ļ�
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
    void PlayBackgroundMusic(Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(SettingScene);


};

#endif __SETTING_SCENE_H__
