#include "MenuScene.h"
#include "SettingScene.h"
#include "ui/CocosGUI.h"
#include "AudioEngine.h"
using namespace ui;

//��ǵ�ǰ�����Ƿ��ڲ���
bool isMusicOn = true;

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
}

Scene* SettingScene::createScene()
{
    auto scene = Scene::create();
    auto layer = SettingScene::create();
    scene->addChild(layer);
    return scene;
}

bool SettingScene::init() {
    if (!Layer::init())
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();


    //��ӱ���ͼƬ
    auto settingBackground = Sprite::create("CarrotGuardRes/UI/SettingBackground.png");
    if (settingBackground == nullptr)
    {
        problemLoading("'SettingBackground.png'");
    }
    else
    {
        settingBackground->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        settingBackground->setScale(visibleSize.width / settingBackground->getContentSize().width);
        this->addChild(settingBackground, 0);
    }

    //��ñ���ͼƬ�ʹ��ڵ����ű���
    float backgroundScale = visibleSize.width / settingBackground->getContentSize().width;

    //��ӷ��ذ�ť
    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    auto returnButton = MenuItemImage::create("CarrotGuardRes/UI/SettingReturnNormal.png", "CarrotGuardRes/UI/SettingReturnSelected.png", CC_CALLBACK_1(SettingScene::ToMenuScene, this));
    if (returnButton == nullptr ||
        returnButton->getContentSize().width <= 0 ||
        returnButton->getContentSize().height <= 0)
    {
        problemLoading("'SettingReturnNormal.png' and 'SettingReturnSelected.png'");
    }
    else
    {
        float x = origin.x + returnButton->getContentSize().width / 2 + visibleSize.width * 0.05f;
        float y = origin.y + visibleSize.height - returnButton->getContentSize().width / 2 - visibleSize.height * 0.04f;
        returnButton->setPosition(Vec2(x, y));
        returnButton->setScale(1.8f* backgroundScale);
        menu->addChild(returnButton);
    }

    //��Ӳ���������ť
    auto playMusicButton= MenuItemImage::create("CarrotGuardRes/UI/MusicOn.png", "CarrotGuardRes/UI/MusicClose.png", CC_CALLBACK_1(SettingScene::PlayBackgroundMusic, this));
    if (playMusicButton == nullptr)
    {
        problemLoading("'MusicOn.png' and 'MusicClose.png'");
    }
    else
    {
        playMusicButton->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.3f, visibleSize.height / 2 + origin.y - visibleSize.height * 0.135f));
        playMusicButton->setScale(1.7f* backgroundScale);
        menu->addChild(playMusicButton);
    }


    return true;
}


void SettingScene::ToMenuScene(Ref* pSender) {
    auto menuScene = MenuScene::createScene();
    Director::getInstance()->replaceScene(menuScene);
}

void SettingScene::PlayBackgroundMusic(Ref* pSender) {
    if (isMusicOn) {
        AudioEngine::play2d("CarrotGuardRes/UI/test_music.mp3", false, 0.5f);
    }
    else {
        AudioEngine::stopAll();
    }
    //�л�ͼƬ
    isMusicOn = !isMusicOn;
    MenuItemImage* button = static_cast<MenuItemImage*>(pSender);
    if (isMusicOn) {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/MusicOn.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/MusicOn.png"));
    }
    else {
        button->setNormalImage(Sprite::create("CarrotGuardRes/UI/MusicClose.png"));
        button->setSelectedImage(Sprite::create("CarrotGuardRes/UI/MusicClose.png"));
    }
}


//void SettingScene::PlayBackgroundMusic(Ref* pSender) {
//    AudioEngine::play2d("test_music.mp3", false, 0.5f);
//}