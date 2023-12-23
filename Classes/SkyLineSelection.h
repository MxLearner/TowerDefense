#pragma once

/*
* ���ݣ�̽��ģʽ������ͷ�ļ�
*/



#ifndef __SKYLINE_SCENE_H__
#define __SKYLINE_SCENE_H__

#include "cocos2d.h"
USING_NS_CC;

class SkyLineSelection : public Layer {
public:
    static Scene* createScene();
    virtual bool init();

    void ToAdvantureScene(Ref* pSender);
    void ToGameScene(Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(SkyLineSelection);


};

#endif __SKYLINE_SCENE_H__
