#pragma once

#ifndef _Music_H_
#define _Music_H_



#include"AudioEngine.h"
#include "cocos2d.h"
USING_NS_CC;


class MusicManager {
public:
    static MusicManager* getInstance();

    void playBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();

    void playSoundEffect(const std::string& soundEffectFile);
    void stopSoundEffect();

private:
    MusicManager();
    ~MusicManager();

    static MusicManager* instance;

    float soundEffectVolume;
    float backgroundMusicVolume;
};

void MusicManager::playBackgroundMusic() {
    AudioEngine::play2d("CarrotGuardRes/Music/bgm.mp3", false, 0.5f);
}





#endif __Music_H__

