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

	// ��Ӷ�����ʾ��
	auto topLabel = Label::createWithSystemFont(StringUtils::format("Select level"), "Arial", 32);
	topLabel->setColor(Color3B::WHITE);
	topLabel->setPosition(screenSize.width * 0.49, screenSize.height * 0.95);
	this->addChild(topLabel, 2);

	//��ӱ���ͼƬ
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

	//��ñ���ͼƬ�ʹ��ڵ����ű���
	float backgroundScale = screenSize.width / backgroundImage->getContentSize().width;

	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	//��ӷ��ذ�ť
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


	//������ҷ�ҳ��ť
	// ����PageView
	auto pageView = PageView::create();
	pageView->setContentSize(Size(screenSize.width, screenSize.height));
	pageView->setPosition(Vec2::ZERO);
	this->addChild(pageView);

	// �����ͼͼƬ����
	std::vector<std::string> mapImages = {
		"CarrotGuardRes/UI/SkyLineLevel_1.png",
		"CarrotGuardRes/UI/SkyLineLevel_2.png",
		"CarrotGuardRes/UI/SkyLineLevel_3.png",
		"CarrotGuardRes/UI/SkyLineLevel_4.png"
	};


	// ���ҳ��
	for (int i = 0; i < mapImages.size(); ++i) {
		Layout* layout = Layout::create();
		layout->setContentSize(Size(screenSize.width, screenSize.height));
		ImageView* imageView = ImageView::create(mapImages[i]);
		imageView->setContentSize(Size(screenSize.width, screenSize.height));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height / 2));
		imageView->setScale(2.0f);
		layout->addChild(imageView, 1);
		pageView->addPage(layout);
	}


	// ��Ӵ����¼�������
	pageView->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			PageView* pageView = dynamic_cast<PageView*>(pSender);
			int currentIndex = pageView->getCurrentPageIndex();

			if (currentIndex < 2) {
				// ����ȷ�ϰ�ť
				auto confirmButton = Button::create("CarrotGuardRes/UI/fileNormal.png", "CarrotGuardRes/UI/fileSelected.png");  // �滻����İ�ťͼƬ
				confirmButton->setName("confirmButton");
				confirmButton->setPosition(Vec2(screenSize.width * 0.6, screenSize.height * 0.13));
				confirmButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
					if (type == Widget::TouchEventType::ENDED) {
						// ����浵��tmp=1
						int tmp = 1;
						auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
						Director::getInstance()->replaceScene(gameScene);
					}
					});
				this->addChild(confirmButton);

				// ����ȡ����ť
				auto cancelButton = Button::create("CarrotGuardRes/UI/startNormal.png", "CarrotGuardRes/UI/startSelected.png");  // �滻����İ�ťͼƬ
				cancelButton->setName("cancelButton");
				cancelButton->setPosition(Vec2(screenSize.width * 0.4, screenSize.height * 0.13));  // ����λ��
				cancelButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
					if (type == Widget::TouchEventType::ENDED) {
						// ������浵��tmp=0
						int tmp = 0;
						auto gameScene = GameScene::createSceneWithLevel(currentIndex + 1, tmp);
						Director::getInstance()->replaceScene(gameScene);
					}
					});
				this->addChild(cancelButton);
			}
			else {
				auto lockedButton = Button::create("CarrotGuardRes/UI/locked.png");  // �滻����İ�ťͼƬ
				lockedButton->setName("lockedButton");
				lockedButton->setPosition(Vec2(screenSize.width * 0.5, screenSize.height * 0.13));  // ����λ��
				this->addChild(lockedButton);
			}

		}
		});

	// ������ҷ�ҳ��ť
	auto leftButton = Button::create("CarrotGuardRes/UI/leftButtonNormal.png", "CarrotGuardRes/UI/leftButtonSelected.png");
	leftButton->setPosition(Vec2(screenSize.width * 0.1, screenSize.height / 2));
	leftButton->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type) {
		if (type == Widget::TouchEventType::ENDED) {
			int currentIndex = pageView->getCurrentPageIndex();
			if (currentIndex > 0) {
				pageView->scrollToPage(currentIndex - 1);

				//������һҳ��ͼ��ѡ��ť
				auto confirm = this->getChildByName("confirmButton");
				auto cancel = this->getChildByName("cancelButton");
				auto locked = this->getChildByName("lockedButton");
				if (confirm && cancel) {
					this->removeChildByName("confirmButton");
					this->removeChildByName("cancelButton");
				}
				if(locked)
					this->removeChildByName("lockedButton");
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
				currentIndex = 0;  //  δ֪ԭ���¿�ʼ��ʱ��index��-1����ʱ����������
			if (currentIndex < mapImages.size() - 1) {
				pageView->scrollToPage(currentIndex + 1);

				auto confirm = this->getChildByName("confirmButton");
				auto cancel = this->getChildByName("cancelButton");
				auto locked = this->getChildByName("lockedButton");
				if (confirm && cancel) {
					this->removeChildByName("confirmButton");
					this->removeChildByName("cancelButton");
				}
				if (locked)
					this->removeChildByName("lockedButton");
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
