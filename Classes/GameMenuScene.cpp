#include "GameMenuScene.h"
#include"ui/CocosGUI.h"

using namespace ui;



Scene* GameMenu::createScene()
{
	auto scene = Scene::create();
	auto layer = GameMenu::create();
	scene->addChild(layer);
	return scene;
}

bool GameMenu::init()
{
	if (!Layer::init()) {
		return false;
	}

	// 获取窗口大小
	Size winsize = Director::getInstance()->getVisibleSize();
	// 获取GL起源点
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 将精灵表单添加到缓存
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile(RESOURCESPLIST);

	// 添加关闭按钮
	auto CloseItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(GameMenu::MenuCloseCallback, this));


	    // 设置摆放位置
	CloseItem->setPosition(Vec2(origin.x + winsize.width - CloseItem->getContentSize().width / 2, origin.y + CloseItem->getContentSize().height / 2));

	// 创建菜单
	auto menu = Menu::create(CloseItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	// 创建开始按钮
	auto StartButton = Button::create("");
	StartButton->setPosition(Vec2(winsize.width * 30 / 100.0, winsize.height * 25 / 100.0));

	    // 添加触摸监听
	StartButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {

		});

	this->addChild(StartButton, 1);

	// 创建选择关卡按钮
	auto SelectButton = Button::create("");
	SelectButton->setPosition(Vec2(winsize.width * 70 / 100.0, winsize.height * 25 / 100.0));

		// 添加触摸监听
	SelectButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {

		});

	this->addChild(SelectButton, 1);


	//
	auto BgSprite = Sprite::create("");
	BgSprite->setPosition(Vec2(winsize.width / 2, winsize.height / 2));

	this->addChild(BgSprite);

	return true;



}

void GameMenu::MenuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
