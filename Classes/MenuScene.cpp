#include "MenuScene.h"
#include "AdvantureScene.h"
#include "SettingScene.h"
#include"clientGameScene.h"
#include "ui/CocosGUI.h"
#include "Music.h"
using namespace ui;
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
}
static int ALLGAMESAVE = 0;
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

	if (ALLGAMESAVE == 0) {
		// 记录关卡是否通关,随便写写
		cocos2d::FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
		std::string path = "AllGameSave.json";
		fileUtils->writeStringToFile("100000000000000000000000", fileUtils->getWritablePath() + path);
		ALLGAMESAVE = 1;
	}

	//添加背景图片
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

	//获得背景图片和窗口的缩放比例
	float backgroundScale = visibleSize.width / backgroundImage->getContentSize().width;


	//添加背景左叶子图片
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

	//添加背景右叶子图片
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

	//添加背景中间叶子图片
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

	//添加背景萝卜图片
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

	//添加背景名称图片
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






	//添加关闭按钮
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


		//添加冒险模式按钮
	auto advantureButton = MenuItemImage::create("CarrotGuardRes/UI/AdvantureButtonNormal.png", "CarrotGuardRes/UI/AdvantureButtonSelected.png", CC_CALLBACK_1(MenuScene::ToAdvantureScene, this));
	if (advantureButton == nullptr ||
		advantureButton->getContentSize().width <= 0 ||
		advantureButton->getContentSize().height <= 0)
	{
		problemLoading("'AdvantureButtonNormal.png' and 'AdvantureButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.2;
		float y = visibleSize.height * 0.15;
		advantureButton->setPosition(Vec2(x, y));
		advantureButton->setScale(1.5f * backgroundScale);
		menu->addChild(advantureButton);
	}

	//添加boss模式按钮
	auto bossButton = MenuItemImage::create("CarrotGuardRes/UI/BossButtonNormal.png", "CarrotGuardRes/UI/BossButtonSelected.png");
	if (bossButton == nullptr ||
		bossButton->getContentSize().width <= 0 ||
		bossButton->getContentSize().height <= 0)
	{
		problemLoading("'CarrotGuardRes/UI/BossButtonNormal.png'and 'CarrotGuardRes/UI/BossButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.5;
		float y = visibleSize.height * 0.14;
		bossButton->setPosition(Vec2(x, y));
		bossButton->setScale(backgroundScale);
		menu->addChild(bossButton);
	}
	//添加观战模式按钮
	auto witnessButton = MenuItemImage::create("CarrotGuardRes/UI/WitnessButtonNormal.png", "CarrotGuardRes/UI/WitnessButtonSelected.png", CC_CALLBACK_1(MenuScene::ToClientGameScene, this));
	if (witnessButton == nullptr ||
		witnessButton->getContentSize().width <= 0 ||
		witnessButton->getContentSize().height <= 0)
	{
		problemLoading("'CarrotGuardRes/UI/WitnessButtonNormal.png'and 'CarrotGuardRes/UI/WitnessButtonSelected.png'");
	}
	else
	{
		float x = visibleSize.width * 0.8;
		float y = visibleSize.height * 0.15;
		witnessButton->setPosition(Vec2(x, y));
		witnessButton->setScale(1.5*backgroundScale);
		menu->addChild(witnessButton);
	}

	//设置界面按钮
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

void MenuScene::menuCloseCallback(Ref* pSender) {
	MusicManager::getInstance()->buttonSound();
	Director::getInstance()->end();
}


void MenuScene::ToAdvantureScene(Ref* pSender) {
	MusicManager::getInstance()->buttonSound();
	auto advantureScene = AdvantureScene::createScene();
	Director::getInstance()->replaceScene(advantureScene);
}

void MenuScene::ToSettingScene(Ref* pSender) {
	MusicManager::getInstance()->buttonSound();
	auto settingScene = SettingScene::createScene();
	Director::getInstance()->replaceScene(settingScene);
}

void MenuScene::ToClientGameScene(Ref* pSender) {
	MusicManager::getInstance()->buttonSound();
	auto witnessScene = ClientGameScene::createScene();
	if (witnessScene != nullptr) {
		Director::getInstance()->pushScene(witnessScene);

	}
}