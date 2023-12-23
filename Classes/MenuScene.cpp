#include "MenuScene.h"
#include "AdvantureScene.h"
#include "SettingScene.h"
#include "ui/CocosGUI.h"
using namespace ui;
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
}

Scene* MenuScene::createScene()
{
    auto scene = Scene::create();
    auto layer = MenuScene::create();
    scene->addChild(layer);
    return scene;
}


bool MenuScene::init() {
	if (!Layer::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();



    //Ìí¼Ó±³¾°Í¼Æ¬
	auto backgroundImage = Sprite::create("CarrotGuardRes/UI/BasicBackground.png");
    if (backgroundImage == nullptr)
    {
        problemLoading("'BasicBackground.png'");
    }
    else
    {
        backgroundImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
        backgroundImage->setScale(visibleSize.width / backgroundImage->getContentSize().width);
        this->addChild(backgroundImage, 0);
    }

    //»ñµÃ±³¾°Í¼Æ¬ºÍ´°¿ÚµÄËõ·Å±ÈÀý
    float backgroundScale = visibleSize.width / backgroundImage->getContentSize().width;

   
    //Ìí¼Ó±³¾°×óÒ¶×ÓÍ¼Æ¬
    auto leafImage_1 = Sprite::create("CarrotGuardRes/UI/LeafBackground_1.png");
    if (leafImage_1 == nullptr)
    {
        problemLoading("'LeafBackground_1.png'");
    }
    else
    {
        leafImage_1->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.06f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.18f));
        leafImage_1->setScale(1.35f * backgroundScale);
        this->addChild(leafImage_1, 0);
    }

    //Ìí¼Ó±³¾°ÓÒÒ¶×ÓÍ¼Æ¬
    auto leafImage_3 = Sprite::create("CarrotGuardRes/UI/LeafBackground_3.png");
    if (leafImage_3 == nullptr)
    {
        problemLoading("'LeafBackground_3.png'");
    }
    else
    {
        leafImage_3->setPosition(Vec2(visibleSize.width / 2 + origin.x + visibleSize.width * 0.075f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.18f));
        leafImage_3->setScale(backgroundScale);
        this->addChild(leafImage_3, 0);
    }

    //Ìí¼Ó±³¾°ÖÐ¼äÒ¶×ÓÍ¼Æ¬
    auto leafImage_2 = Sprite::create("CarrotGuardRes/UI/LeafBackground_2.png");
    if (leafImage_2 == nullptr)
    {
        problemLoading("'LeafBackground_2.png'");
    }
    else
    {
        leafImage_2->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y + visibleSize.height * 0.20f));
        leafImage_2->setScale(1.5f * backgroundScale);
        this->addChild(leafImage_2, 0);
    }

    //Ìí¼Ó±³¾°ÂÜ²·Í¼Æ¬
    auto carrotImage = Sprite::create("CarrotGuardRes/UI/CarrotBackground.png");
    if (carrotImage == nullptr)
    {
        problemLoading("'CarrotBackground.png'");
    }
    else
    {
        carrotImage->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.005f, visibleSize.height / 2 + origin.y + visibleSize.height * 0.06f));
        carrotImage->setScale(1.5f * backgroundScale);
        this->addChild(carrotImage, 0);
    }

    //Ìí¼Ó±³¾°Ãû³ÆÍ¼Æ¬
    auto NameImage = Sprite::create("CarrotGuardRes/UI/NameBackground.png");
    if (NameImage == nullptr)
    {
        problemLoading("'NameBackground.png'");
    }
    else
    {
        NameImage->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - visibleSize.height * 0.055f));
        NameImage->setScale(1.6f * backgroundScale);
        this->addChild(NameImage, 0);
    }



    auto menu = Menu::create();
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);






    //Ìí¼Ó¹Ø±Õ°´Å¥
    auto closeButton = MenuItemImage::create("CarrotGuardRes/UI/CloseNormal.png", "CarrotGuardRes/UI/CloseSelected.png", CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    if (closeButton == nullptr ||
        closeButton->getContentSize().width <= 0 ||
        closeButton->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeButton->getContentSize().width / 2;
        float y = origin.y + closeButton->getContentSize().height / 2;
        closeButton->setPosition(Vec2(x, y));
        closeButton->setScale(backgroundScale);
        menu->addChild(closeButton);
       
    }


   

    /*auto advantureButton = Button::create("AdvantureButtonNormal.png");
    advantureButton->setPosition(Vec2(visibleSize.width * 30 / 100.0, visibleSize.height * 25 / 100.0));

    advantureButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
        MenuScene::ToAdvantureScene;
        });*/

    
    //Ìí¼ÓÃ°ÏÕÄ£Ê½°´Å¥
    auto advantureButton = MenuItemImage::create("CarrotGuardRes/UI/AdvantureButtonNormal.png", "CarrotGuardRes/UI/AdvantureButtonSelected.png", CC_CALLBACK_1(MenuScene::ToAdvantureScene, this));
    if (advantureButton == nullptr ||
        advantureButton->getContentSize().width <= 0 ||
        advantureButton->getContentSize().height <= 0)
    {
        problemLoading("'AdvantureButtonNormal.png' and 'AdvantureButtonSelected.png'");
    }
    else
    {
        float x = visibleSize.width * 20 / 100.0;
        float y = visibleSize.height * 15 / 100.0;
        advantureButton->setPosition(Vec2(x, y));
        advantureButton->setScale(1.5f * backgroundScale);
        menu->addChild(advantureButton);
    }

    //ÉèÖÃ½çÃæ°´Å¥
    auto settingImage = MenuItemImage::create("CarrotGuardRes/UI/SettingNormal.png", "SettingSelected.png", CC_CALLBACK_1(MenuScene::ToSettingScene, this));
    if (settingImage == nullptr)
    {
        problemLoading("'SettingNormal.png' and 'SettingSelected.png'");
    }
    else
    {
        settingImage->setPosition(Vec2(visibleSize.width / 2 + origin.x - visibleSize.width * 0.3f, visibleSize.height / 2 + origin.y - visibleSize.height * 0.135f));
        settingImage->setScale(1.7f * backgroundScale);
        menu->addChild(settingImage);
    }


   

    return true;

}

void MenuScene::menuCloseCallback(Ref* pSender){
    Director::getInstance()->end();
}


void MenuScene::ToAdvantureScene(Ref* pSender) {
    auto advantureScene = AdvantureScene::createScene();
    Director::getInstance()->replaceScene(advantureScene);
}

void MenuScene::ToSettingScene(Ref* pSender) {
    auto settingScene = SettingScene::createScene();
    Director::getInstance()->replaceScene(settingScene);
}