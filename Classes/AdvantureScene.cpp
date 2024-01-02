#include "MenuScene.h"
#include "AdvantureScene.h"
#include "SkyLineSelection.h"
#include "ui/CocosGUI.h"
#include "Music.h"
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

	auto screenSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();




	//添加提示词
	auto topLabel = Label::createWithSystemFont(StringUtils::format("Select level"), "Arial", 32);
	topLabel->setColor(Color3B::WHITE);
	topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
	this->addChild(topLabel, 2);

	float backgroundScale = 1.0;

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
		backgroundScale = screenSize.width / backgroundImage->getContentSize().width;
		this->addChild(backgroundImage, 0);
	}


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
		"CarrotGuardRes/UI/SkyLine.png",
		"CarrotGuardRes/UI/Desert.png",
		 "CarrotGuardRes/UI/Jungle.png"
	};


	// 添加页面
	for (int i = 0; i < mapImages.size(); ++i) {
		Layout* layout = Layout::create();
		layout->setContentSize(Size(screenSize.width, screenSize.height));
		ImageView* imageView = ImageView::create(mapImages[i]);
		imageView->setContentSize(Size(screenSize.width, screenSize.height));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		imageView->setScale(1.5);
		layout->addChild(imageView, 1);
		pageView->addPage(layout);
	}


	// 添加触摸事件监听器
	pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			MusicManager::getInstance()->buttonSound();
			PageView* pageView = dynamic_cast<PageView*>(pSender);
			int currentIndex = pageView->getCurrentPageIndex();
			Scene* skylineScene;
			// 处理页面切换完成后的逻辑
			switch (currentIndex) {
			case 0:
				skylineScene = SkyLineSelection::createScene();
				Director::getInstance()->replaceScene(skylineScene);
				break;
			case 1:
				break;
			default:
				break;
			}

		}
		});

	// 添加左右翻页按钮
	auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
	leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
	leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			MusicManager::getInstance()->buttonSound();
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
			MusicManager::getInstance()->buttonSound();
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

void AdvantureScene::ToMenuScene(Ref* pSender) {
	MusicManager::getInstance()->buttonSound();
	auto menuScene = MenuScene::createScene();
	Director::getInstance()->replaceScene(menuScene);
}

void AdvantureScene::ToSkyLineSelection(Ref* pSender) {
	MusicManager::getInstance()->buttonSound();
	auto skylineScene = SkyLineSelection::createScene();
	Director::getInstance()->replaceScene(skylineScene);
}