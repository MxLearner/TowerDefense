#pragma once

#ifndef _Music_H_
#define _Music_H_



#include"AudioEngine.h"
#include "cocos2d.h"
USING_NS_CC;


class MusicManager {
public:
    static MusicManager* getInstance();

    void preloadSoundEffect(const std::string& soundEffectFile);
    int getIsBGMPlay();
    void setIsBGMPlay(int option);
    int getIsSoundPlay();
    void setIsSoundPlay(int option);
    int getIsBGMPause();
    void setIsBGMPause(int option);

    void playBackgroundMusic();
    void pauseBackgroundMusic();
    void buttonSound();
    void pageSound();
    void buildSound();
    void sellSound();
    void upgradeSound();
    void sunSound();
    void bottleSound();
    void fanSound();
    void carrotSound();
    void normalSound();
    void countSound();
    void downSound();

private:
    MusicManager();
    ~MusicManager();
    static MusicManager* instance;
    int isBGMPlay=1;
    int isBGMPause = 0;
    int isSoundPlay = 1;
    int bgmID = 0;
 

};



#endif __Music_H__

