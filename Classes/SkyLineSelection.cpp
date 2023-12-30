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

	auto screenSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 添加顶部提示词
	auto topLabel = Label::createWithSystemFont(StringUtils::format("Select level"), "Arial", 32);
	topLabel->setColor(Color3B::WHITE);
	topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
	this->addChild(topLabel, 2);

	//添加背景图片
	auto backgroundImage = Sprite::create("CarrotGuardRes/UI/AdvantureBackground.png");
	if (backgroundImage == nullptr)
	{
		problemLoading("'AdvantureBackground.png'");
	}
	else
	{
		backgroundImage->setPosition(Vec2(screenSize.width / 2 + origin.x, screenSize.height / 2 + origin.y));
		backgroundImage->setScale(screenSize.width / backgroundImage->getContentSize().width);
		this->addChild(backgroundImage, 0);
	}

	//获得背景图片和窗口的缩放比例
	float backgroundScale = screenSize.width / backgroundImage->getContentSize().width;

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
		float x = origin.x + returnButton->getContentSize().width / 2 + screenSize.width * 0.05f;
		float y = origin.y + screenSize.height - returnButton->getContentSize().width / 2 - screenSize.height * 0.04f;
		returnButton->setPosition(Vec2(x, y));
		returnButton->setScale(3.0f * backgroundScale);
		menu->addChild(returnButton);
	}


	//添加左右翻页按钮
	// 创建PageView
	auto pageView = PageView::create();
	pageView->setContentSize(Size(screenSize.width, screenSize.height));
	pageView->setPosition(Vec2::ZERO);
	this->addChild(pageView);

	// 定义地图图片数组
	std::vector<std::string> mapImages = {
		"CarrotGuardRes/UI/SkyLineLevel_1.png",
		"CarrotGuardRes/UI/SkyLineLevel_2.png",
		"CarrotGuardRes/UI/SkyLineLevel_3.png"
	};


	// 添加页面
	for (int i = 0; i < mapImages.size(); ++i) {
		Layout* layout = Layout::create();
		layout->setContentSize(Size(screenSize.width, screenSize.height));
		ImageView* imageView = ImageView::create(mapImages[i]);
		imageView->setContentSize(Size(screenSize.width, screenSize.height));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		layout->addChild(imageView, 1);
		pageView->addPage(layout);
	}


	// 添加触摸事件监听器
	pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			PageView* pageView = dynamic_cast<PageView*>(pSender);
			int currentIndex = pageView->getCurrentPageIndex();

			// 创建确认按钮
			auto confirmButton = Button::create("CarrotGuardRes/UI/SettingNormal.png");  // 替换成你的按钮图片
			confirmButton->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2 + 100));
			confirmButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
				if (type == Widget::TouchEventType::ENDED) {
					// 载入存档，tmp=1
					int tmp = 1;
					auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
					Director::getInstance()->replaceScene(gameScene);
				}
				});
			this->addChild(confirmButton);

			// 创建取消按钮
			auto cancelButton = Button::create("CarrotGuardRes/UI/SettingNormal.png");  // 替换成你的按钮图片
			cancelButton->setPosition(Vec2(screenSize.width / 2, screenSize.height / 2 - 100));  // 调整位置
			cancelButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
				if (type == Widget::TouchEventType::ENDED) {
					// 不载入存档，tmp=0
					int tmp = 0;
					auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
					Director::getInstance()->replaceScene(gameScene);
				}
				});
			this->addChild(cancelButton);

		}
		});

	// 添加左右翻页按钮
	auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
	leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
	leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			int currentIndex = pageView->getCurrentPageIndex();
			CCLOG("currentIndex:  %d", currentIndex);
			if (currentIndex > 0) {
				pageView->scrollToPage(currentIndex - 1);
			}
		}
		});
	this->addChild(leftButton, 2);

	auto rightButton = Button::create("CarrotGuardRes/UI/rightButtonNormal.png", "CarrotGuardRes/UI/rightButtonSelected.png");
	rightButton->setPosition(Vec2(screenSize.width * 0.9, screenSize.height / 2));
	rightButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex < 0)
				currentIndex = 0;  //  未知原因导致开始的时候index是-1，暂时像这样处理。
			CCLOG("currentIndex:  %d", currentIndex);
			if (currentIndex < mapImages.size() - 1) {
				pageView->scrollToPage(currentIndex + 1);
			}
		}
		});
	this->addChild(rightButton, 2);

	return true;

}


void SkyLineSelection::ToAdvantureScene(Ref* pSender) {
	auto advantureScene = AdvantureScene::createScene();
	Director::getInstance()->replaceScene(advantureScene);
}
