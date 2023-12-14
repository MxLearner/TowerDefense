#include "LevelManager.h"
#include"ui/CocosGUI.h"

using namespace ui;


Scene* LevelManager::createScene()
{
    auto scene = Scene::create();
    auto layer = LevelManager::create();
    scene->addChild(layer);

    return scene;
}

bool LevelManager::init()
{
	if (!Layer::init()) {
		return false;
	}

	// 获取窗口大小
	Size winsize = Director::getInstance()->getVisibleSize();
	// 获取GL起源点
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	// 添加关闭按钮
	auto CloseItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", CC_CALLBACK_1(LevelManager::menuCloseCallback, this));


	    // 设置摆放位置
	CloseItem->setPosition(Vec2(origin.x + winsize.width - CloseItem->getContentSize().width / 2, origin.y + CloseItem->getContentSize().height / 2));
    
	// 创建菜单
	auto menu = Menu::create(CloseItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);	

	//默认选择第一关,索引下标从0开始
	_SelectLevelIndex = 0;

	// 创建分页容器
	auto PageView = PageView::create();

	PageView->setContentSize(Size(480.0f, 320.0f));
	PageView->setPosition(Vec2((winsize.width - PageView->getContentSize().width) / 2.0f, (winsize.height - PageView->getContentSize().height) * 0.6));

	// 循环添加3个layout关卡图片
	for (int i = 0; i < 3; i++) {
		Layout* layout = Layout::create();
		layout->cocos2d::Node::setContentSize(Size(480.0f, 320.0f));
		// 创建imageview
		ImageView* imageView = ImageView::create("");
		imageView->setContentSize(Size(480.0f, 320.0f));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2.0f, layout->getContentSize().height / 2.0f));
		layout->addChild(imageView);
		// 在i表示的位置添加layout
		PageView->insertPage(layout, i);
	}
	// 添加事件监听器 =========================
	PageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {

		switch (type)
		{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
			break;
		case cocos2d::ui::Widget::TouchEventType::ENDED:
			break;
		case cocos2d::ui::Widget::TouchEventType::CANCELED:
			break;
		default:
			break;
		}


		});




	return false;
}

void LevelManager::menuCloseCallback(Ref* pSender)
{
}
