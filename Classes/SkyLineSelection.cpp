#include "MenuScene.h"
#include "AdvantureScene.h"
#include "SkyLineSelection.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"
using namespace ui;


static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
}

Scene* SkyLineSelection::createScene()
{
	auto scene = Scene::create();
	auto layer = SkyLineSelection::create();
	scene->addChild(layer);
	return scene;
}

bool SkyLineSelection::init() {
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
	auto returnButton = MenuItemImage::create("CarrotGuardRes/UI/ReturnButtonNormal.png", "CarrotGuardRes/UI/ReturnButtonSelected.png", CC_CALLBACK_1(SkyLineSelection::ToAdvantureScene, this));
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
		returnButton->setPosition(Vec2(x, y));
		returnButton->setScale(3.0f * backgroundScale);
		menu->addChild(returnButton);
	}

	//添加level_1
	auto level_1_Image = MenuItemImage::create("CarrotGuardRes/UI/SkyLineLevel_1.png", "CarrotGuardRes/UI/SkyLineLevel_1.png", CC_CALLBACK_1(SkyLineSelection::ToGameScene, this));
	if (level_1_Image == nullptr)
	{
		problemLoading("'SkyLineLevel_1.png'");
	}
	else
	{
		level_1_Image->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
		level_1_Image->setScale(2.0f * backgroundScale);
		menu->addChild(level_1_Image);
	}



	return true;
}

void SkyLineSelection::ToAdvantureScene(Ref* pSender) {
	auto advantureScene = AdvantureScene::createScene();
	Director::getInstance()->replaceScene(advantureScene);
}

void SkyLineSelection::ToGameScene(Ref* pSender) {

	auto gameScene = GameScene::createSceneWithLevel(1);
	Director::getInstance()->replaceScene(gameScene);
}