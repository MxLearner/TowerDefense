#include "MenuScene.h"
#include "AdvantureScene.h"
#include "SkyLineSelection.h"
#include "ui/CocosGUI.h"
using namespace ui;


static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
}

Scene* AdvantureScene::createScene()
{
    auto scene = Scene::create();
    auto layer = AdvantureScene::create();
    scene->addChild(layer);
    return scene;
}

bool AdvantureScene::init() {
    if (!Layer::init())
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();



    //添加背景图片
    auto backgroundImage = Sprite::create("CarrotGuardRes/UI/AdvantureBackground.png");
    if (backgroundImage == nullptr)
    {
        problemLoading("'AdvantureBackground.png'");
    }
    else
    {
        backgroundImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        backgroundImage->setScale(visibleSize.width / backgroundImage->getContentSize().width);
        this->addChild(backgroundImage, 0);
    }

    //获得背景图片和窗口的缩放比例
    float backgroundScale = visibleSize.width / backgroundImage->getContentSize().width;


    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    //添加返回按钮
    auto returnButton = MenuItemImage::create("CarrotGuardRes/UI/ReturnButtonNormal.png", "CarrotGuardRes/UI/ReturnButtonSelected.png", CC_CALLBACK_1(AdvantureScene::ToMenuScene, this));
    if (returnButton == nullptr ||
        returnButton->getContentSize().width <= 0 ||
        returnButton->getContentSize().height <= 0)
    {
        problemLoading("'ReturnButtonNormal.png' and 'ReturnButtonSelected.png'");
    }
    else 
    {
        float x = origin.x + returnButton->getContentSize().width / 2 + visibleSize.width * 0.05f;
        float y = origin.y + visibleSize.height - returnButton->getContentSize().width / 2 - visibleSize.height * 0.04f;
       /* float x = origin.x + visibleSize.width * 0.15f; 
        float y = origin.y + visibleSize.height * 0.88f;*/
        returnButton->setPosition(Vec2(x, y));
        returnButton->setScale(3.0f* backgroundScale);
        menu->addChild(returnButton);
    }

    //天际线部分选项
    auto skylineImage = MenuItemImage::create("CarrotGuardRes/UI/SkyLine.png","CarrotGuardRes/UI/SkyLine.png", CC_CALLBACK_1(AdvantureScene::ToSkyLineSelection, this));
    if (skylineImage == nullptr)
    {
        problemLoading("'SkyLine.png'");
    }
    else
    {
        skylineImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        skylineImage->setScale(1.7f* backgroundScale);
        menu->addChild(skylineImage);
    }




    return true;
}

void AdvantureScene::ToMenuScene(Ref* pSender) {
    auto menuScene = MenuScene::createScene();
    Director::getInstance()->replaceScene(menuScene);
}

void AdvantureScene::ToSkyLineSelection(Ref* pSender) {
    auto skylineScene = SkyLineSelection::createScene();
    Director::getInstance()->replaceScene(skylineScene);
}