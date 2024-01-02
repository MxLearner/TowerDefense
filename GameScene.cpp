#include"GameScene.h"
#include"ui/CocosGUI.h"
#include"Turret/Turret_TB.h"
#include"Turret/Turret_TFan.h"
#include"Turret/Turret_TSun.h"
#include"SkyLineSelection.h"
#include <regex>
#include "Music.h"
using namespace ui;
USING_NS_CC;

static int currentLevel = 0;  // ��ǰ�ؿ�
static int  IS_LOAD_SAVE_GAME = 1; // �Ƿ���ش浵

static int IS_BEGAN_SERVER = 1;// �Ƿ�������������Ĭ�Ͽ����ɣ���Ȼ��Ҫ�Ӹ���ť

#define DEBUG
void GameScene::startServer()
{
	std::lock_guard<std::mutex> lock(serverMutex);

	if (udpserver.Start()) {
		udpserver.Receive();
	}
	else {
		CCLOG("server start failed");
	}
}

// ���ݹؿ���Ŵ�����Ϸ�ؿ�����
Scene* GameScene::createSceneWithLevel(int selectLevel, int isSave)
{   // ��ùؿ����
	currentLevel = selectLevel;
	IS_LOAD_SAVE_GAME = isSave;
	auto scene = Scene::create();

	auto layer = GameScene::create();
	layer->setName("layer"); // �������
	scene->addChild(layer);
	return scene;
}
// �ؿ�������ʼ��
bool GameScene::init()
{

	int isMusicOn = MusicManager::getInstance()->getIsBGMPlay();
	if (isMusicOn) {
		MusicManager::getInstance()->pauseBackgroundMusic();
		MusicManager::getInstance()->setIsBGMPlay(0);
		MusicManager::getInstance()->setIsBGMPause(1);
	}

	if (!Layer::init()) {
		return false;
	}
	// ��ȡ��Ļ����
	screenSize = Director::getInstance()->getVisibleSize();
	_screenWidth = screenSize.width;
	_screenHeight = screenSize.height;
#ifdef DEBUG
	CCLOG("screenWidth:  %lf, screenHeight:  %lf", _screenWidth, _screenHeight);
#endif // DEBUG

	// ���������
	if (IS_BEGAN_SERVER) {
		std::thread serverThread(&GameScene::startServer, this);
		serverThread.detach();
	}


	// ��ȡ�ؿ����� 
	LoadLevelData();
	// ���йؿ���ʼ��
	initLevel();
	// ��ȡ�浵
	if (IS_LOAD_SAVE_GAME) {
		LoadSaveGame();
	}
	// ������Ļ����
	TopLabel();
	// ��ʼ��Ϸʱ������ʱ
	CountDown();
	// ���ش浵
	if (IS_LOAD_SAVE_GAME) {
		initSaveGame();
	}

	// ���������ʱ�䣬���ڽ���
	auto listener = EventListenerMouse::create();
	//listener->onMouseDown = CC_CALLBACK_1(GameScene::onClicked, this);
	listener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);



	//��ʼ�����������������ĵ���¼�
	setBuildEvent(NULL);
	setHasBuild(0);
	setHasUpgrade(0);
	createTouchLayer();
	setTouchLayer(NULL);
	createTouchListener();
	setTouchListener(NULL);
	//��ʼ�����һ���ı��
	setIsFinalWave(0);
	return true;
}
void GameScene::LoadLevelData()
{
	// rapidjson ����
	rapidjson::Document document;

	// ���ݴ��ݵĹؿ�ֵselectLevel��ö�Ӧ�Ĺؿ������ļ�
	std::string filePath = FileUtils::getInstance()->
		fullPathForFilename(StringUtils::format("CarrotGuardRes/LeveL_%d.data", currentLevel));

	// ��ȡ�ļ�����
	std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);

	// ����contentStr��json���ݣ����浽document��
	document.Parse<0>(contentStr.c_str());

	//***************��ȡ�ļ�����********************
	// 1. ��ȡ��ͼ�ļ�
	_tileFile = document["tileFile"].GetString();
	// 2. ��������
	_monsterAll = document["monsterAll"].GetInt();
	// 3. ���ﲨ��
	_monsterWave = document["monsterWave"].GetInt();
	// 4.ÿ����������
	const rapidjson::Value& waveArray = document["everyWave"];
	for (int i = 0; i < waveArray.Size(); i++) {
		_everyWave.push_back(waveArray[i].GetInt());
	}
	// 5. ��ùؿ��趨�Ĺ���
	const rapidjson::Value& monsterArray = document["monsters"];
	for (int i = 0; i < monsterArray.Size(); i++) {
		// ���ÿһ����������
		std::string name = monsterArray[i]["name"].GetString();
		int lifeValue = monsterArray[i]["lifeValue"].GetInt();
		int gold = monsterArray[i]["gold"].GetInt();
		float speed = monsterArray[i]["speed"].GetFloat();
		// �����ݴ���MonsterData������
		auto monsterData = MonsterData::create();
		monsterData->setName(name);
		monsterData->setLifeValue(lifeValue);
		monsterData->setGold(gold);
		monsterData->setSpeed(speed);
		// ���䴫���ؿ����Ｏ����
		_monsterDatas.pushBack(monsterData);
	}
	// 6. ��ùؿ��趨������
	const rapidjson::Value& turretArray = document["turrets"];
	for (int i = 0; i < turretArray.Size(); i++) {
		// ��ȡ��������
		std::string name = turretArray[i]["name"].GetString();
		int cost1 = turretArray[i]["Cost1"].GetInt();
		int cost2 = turretArray[i]["Cost2"].GetInt();
		int cost3 = turretArray[i]["Cost3"].GetInt();
		float range = turretArray[i]["range"].GetFloat();
		int damage = turretArray[i]["damage"].GetInt();
		// ����turretdata
		auto turretData = TurretData::create();
		turretData->setName(name);
		turretData->setCost1(cost1);
		turretData->setCost2(cost2);
		turretData->setCost3(cost3);
		turretData->setRange(range);
		turretData->setDamage(damage);
		// �����ؿ�����
		_turretDatas.pushBack(turretData);
	}
	// 7. ���ﾭ����·��
	// 
	// ��Ϊ·�����������Ҫ�ȼ��ص�ͼ��ֻ�ܰ�����ǰ����
	// ����TMXTiledMap ��ȡ��Ƭ��ͼ
	_tileMap = TMXTiledMap::create(_tileFile);
	//********************************************************************
	//Fix incorrect rendering of tilemaps with csv data on windows
	// https://github.com/cocos2d/cocos2d-x/pull/20483/files
	// ���windows�¼��ص�ͼ���⣡��������������
	//********************************************************************

	// ���ó��������Ĵ�СΪ���ڴ�С
	//this->setContentSize(size);
	// ������Ƭ��ͼ��ʹ������������Ļ
	_tileMap->setScaleX(screenSize.width / _tileMap->getContentSize().width);
	_tileMap->setScaleY(screenSize.height / _tileMap->getContentSize().height);
	// �ѵ�ͼê���λ�ö�����Ϊԭ�㣬ʹ��ͼ���½�����Ļ���½Ƕ���
	_tileMap->setAnchorPoint(Vec2::ZERO);
	_tileMap->setPosition(Vec2::ZERO);
	_tileMap->setName("_tileMap"); // �������
	this->addChild(_tileMap, 1);

	//**************************************************************
	const rapidjson::Value& pathtArray = document["path"];
	for (int i = 0; i < pathtArray.Size(); i++) {
		// 1. ��ȡÿ�����ﾭ����·��x,y
		int x = pathtArray[i]["x"].GetInt();
		int y = pathtArray[i]["y"].GetInt();
		// ������ͼ����
		Vec2 tilePoint = Vec2(x, y);
		// ����ͼ����ת������Ļ����
		Vec2 locationPoint = TMXPosToLocation(tilePoint);

		setCarrotTag(1000 * x + y);

		// Point���ܼ̳�Ref��Vector���ܴ洢
		auto pointDelegate = PointDelegate::create(locationPoint.x, locationPoint.y);

		// ��ÿ������浽·��������
		_pathPoints.pushBack(pointDelegate);
	}

}


// *******************************************���������
void GameScene::onMouseDown(EventMouse* event)
{
	// ��ȡ�����������
	Vec2 clickPos = event->getLocation();
	//��OpenGL����ϵת��Ϊ��Ļ����ϵ
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// ע����������λ��
	// ���������OpenGL����ϵ�����Ͻ�0��0����Ļ�������½�0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// ת����TMX��ͼ����
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);

	//��ȡ��ǰ��������Ƿ񼤻�
	int hasBuild = getHasBuild();
	//��ȡ��ǰ���������Ƿ񼤻�
	int hasUpgrade = getHasUpgrade();

	// �жϵ���ܲ��¼�
	int currentTag = 1000 * mapX + mapY;
	CCLOG("currenttTag: %d", 1000 * mapX + mapY);
	CCLOG("carrotTag: %d", getCarrotTag());
	if (currentTag == getCarrotTag() && _goldValue >= _carrotCost && carrotHealth < 5) {
		carrotHealth++;
		_goldValue -= _carrotCost;
		HurtCarrot(0);
	}


	// ��ͼ�Ͽ��Խ���ʱ
	if (isTurretAble[mapX][mapY] == 0) {
		if (!hasUpgrade)//������ֹ����������ʱ�����ԱߵĿյ�
			TouchLand(event);
	}
	else if (isTurretAble[mapX][mapY] != 0 && isTurretAble[mapX][mapY] != 1) {
		if (!hasBuild)//������ֹ�ڽ�������ʱ�����Աߵ���
			TouchTower(event);
	}


}

void GameScene::TouchLand(EventMouse* event) {

	// ��ȡ�����������
	Vec2 clickPos = event->getLocation();
	//��OpenGL����ϵת��Ϊ��Ļ����ϵ
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// ע����������λ��
	// ���������OpenGL����ϵ�����Ͻ�0��0����Ļ�������½�0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// ת����TMX��ͼ����
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);
	// ��ͼ�Ͽ��Խ���ʱ


	//��ȡ��ǰ��������Ƿ񼤻�
	int hasBuild = getHasBuild();

	//���û�м�������
	if (!hasBuild) {
		//�����µĴ�����ͼ�����
		createTouchLayer();
		createTouchListener();
	}
	//��ȡ�µĴ�����ͼ�����
	auto touch_layer = getTouchLayer();
	auto touch_listener = getTouchListener();

	//���µĴ��������ӵ���������
	if (!hasBuild)
		this->addChild(touch_layer, 10);


	//��������ͼ�꾫��
	Sprite* TB_Can = Sprite::create("CarrotGuardRes/Towers/TBottle/CanBuy.png");
	Sprite* TFan_Can = Sprite::create("CarrotGuardRes/Towers/TFan/TFan_CanBuy.png");
	Sprite* TSun_Can = Sprite::create("CarrotGuardRes/Towers/TSun/TSun_CanBuy.png");
	Sprite* TB_Not = Sprite::create("CarrotGuardRes/Towers/TBottle/NotBuy.png");
	Sprite* TFan_Not = Sprite::create("CarrotGuardRes/Towers/TFan/TFan_NotBuy.png");
	Sprite* TSun_Not = Sprite::create("CarrotGuardRes/Towers/TSun/TSun_NotBuy.png");

	//�������ַ������Ľ���ɱ�
	int TB_Cost{}, TFan_Cost{}, TSun_Cost{};

	if (!hasBuild) {
		//�ɵ�ͼ������ת��Ϊ��Ļ���꣬��֤ͬһ��ͼ���꽨��ʱ��Ļ������ͬ
		screenPos = TMXPosToLocation(mapPos);
		//�����ڵ������еļ�����ǩ
		int count = 1;
		//�������ÿһ���������Ľ���ɱ�
		for (const auto& turretData : _turretDatas) {
			switch (count) {
			case 1:
				TB_Cost = turretData->getCost1();
				break;
			case 2:
				TFan_Cost = turretData->getCost1();
				break;
			case 3:
				TSun_Cost = turretData->getCost1();
				break;
			default:
				break;
			}
			count++;
		}

		//���ݵ�ǰ�Ľ����������ý����������ͼ��
		if (_goldValue >= TB_Cost) {
			TB_Can->setPosition(screenPos.x - _screenWidth * 0.05, screenPos.y);
			TB_Can->setVisible(true);
			touch_layer->addChild(TB_Can);
			TB_Can->setName("TB_Can");
		}
		else {
			TB_Not->setPosition(screenPos.x - _screenWidth * 0.05, screenPos.y);
			TB_Not->setVisible(true);
			touch_layer->addChild(TB_Not);
			TB_Not->setName("TB_Not");
		}
		if (_goldValue >= TFan_Cost) {
			TFan_Can->setPosition(screenPos.x, screenPos.y);
			TFan_Can->setVisible(true);
			touch_layer->addChild(TFan_Can);
			TFan_Can->setName("TFan_Can");
		}
		else {
			TFan_Not->setPosition(screenPos.x, screenPos.y);
			TFan_Not->setVisible(true);
			touch_layer->addChild(TFan_Not);
			TFan_Not->setName("TFan_Not");
		}
		if (_goldValue >= TFan_Cost) {
			TSun_Can->setPosition(screenPos.x + _screenWidth * 0.06, screenPos.y);
			TSun_Can->setVisible(true);
			touch_layer->addChild(TSun_Can);
			TSun_Can->setName("TSun_Can");
		}
		else {
			TSun_Not->setPosition(screenPos.x + _screenWidth * 0.05, screenPos.y);
			TSun_Not->setVisible(true);
			touch_layer->addChild(TSun_Not);
			TSun_Not->setName("TSun_Not");
		}

	}

	//���������������ʾͼ��ľ���
	Sprite* TB, * TFan, * TSun;
	//ͨ��name��ȡ�����Խ��족��ͼ�꣬������ܻ�ȡ����˵��"���ɽ��족
	TB = (Sprite*)touch_layer->getChildByName("TB_Can");
	TFan = (Sprite*)touch_layer->getChildByName("TFan_Can");
	TSun = (Sprite*)touch_layer->getChildByName("TSun_Can");

	//��¼��ǰ����¼����������ں�����÷����������λ��
	if (!hasBuild) {
		EventMouse* temp = new EventMouse(*event);
		setBuildEvent(temp);
	}
	//���½���״̬��˵����ǰ�Ѽ�������
	setHasBuild(1);
	//��ȡ����������λ�ö�Ӧ�ĵ���¼�
	EventMouse* buildTower = getBuildEvent();
	//�������괥���ĵ���¼�
	touch_listener->onMouseDown = [this, TB, TFan, TSun, &screenPos, mapPos, touch_layer, touch_listener, buildTower](EventMouse* event) {

		//��¼���������
		Vec2 clickPos = event->getLocation();
		//�����������ת��ΪUI����
		Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);

		//������TBָ�������Խ���ͼ�ꡱ��������ڣ���˵�������Խ��죬Ϊ�١�������ڣ��ҵ������ͼ��ķ�Χ����Ϊ�档
		if ((TB) && TB->getBoundingBox().containsPoint(screenPos)) {
			//�����Ӧ�ķ�����
			BuildTower(buildTower, 1);
			//�Ƴ�������õ�lisenner����ֹ���lisenner�໥����
			_eventDispatcher->removeEventListener(touch_listener);
			//�ӳ������Ƴ�������
			this->removeChild(touch_layer);
			//�����Ƴ�������
			removeTouchLayer();
			//�Ƴ�����������
			removeTouchListener();
		}
		else if ((TSun) && TSun->getBoundingBox().containsPoint(screenPos)) {
			BuildTower(buildTower, 2);
			_eventDispatcher->removeEventListener(touch_listener);
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		else if ((TFan) && TFan->getBoundingBox().containsPoint(screenPos)) {
			BuildTower(buildTower, 3);
			_eventDispatcher->removeEventListener(touch_listener);
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		//�����������������Խ��족ͼ��֮��ĵط��������ٽ������
		else {
			_eventDispatcher->removeEventListener(touch_listener);
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		//��ʾ�������������
		setHasBuild(0);
		};
	if (!hasBuild)
		_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

}

void GameScene::BuildTower(EventMouse* event, int numTower) {
	MusicManager::getInstance()->buildSound();
	// ��ȡ�����������
	Vec2 clickPos = event->getLocation();
	//��OpenGL����ϵת��Ϊ��Ļ����ϵ
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// ע����������λ��
	// ���������OpenGL����ϵ�����Ͻ�0��0����Ļ�������½�0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// ת����TMX��ͼ����
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);

	//������Ҫ�Ľ��
	int buildCost{};

	if (isTurretAble[mapX][mapY] == 0) {
		// ������ǰλ����������
		isTurretAble[mapX][mapY] = 1 + numTower * 10;

		//������õ�ǰ���������name
		TurretData* turretData = nullptr;
		std::string name;
		int count = 1;
		for (const auto& temp : _turretDatas) {
			if (count >= numTower) {
				name = (temp->getName());
				buildCost = (temp->getCost1());
				turretData = temp;
				break;
			}
			count++;
		}

		//���ݵ�ǰ�������ͽ��ж�Ӧ�Ľ���
		Turret* turret;
		if (numTower == 1) {
			turret = Turret_TB::createWithSpriteFrameName(name, 1);
		}
		else if (numTower == 2) {
			turret = Turret_TSun::createWithSpriteFrameName(name, 1);
		}
		else {
			turret = Turret_TFan::createWithSpriteFrameName(name, 1);
		}
		
		_goldValue -= buildCost;
		turret->setCost1(turretData->getCost1());
		turret->setCost2(turretData->getCost2());
		turret->setCost3(turretData->getCost3());
		turret->setDamage(turretData->getDamage());
		turret->setRange(turretData->getRange());
		//���½��������뵱ǰ���������ļ�����
		_currentTurrets.pushBack(turret);
		turret->setName(name);
		screenPos = TMXPosToLocation(mapPos);
		turret->setPosition(screenPos);
		
		turret->setTag(mapX * 1000 + mapY);
		turret->init();
		this->addChild(turret, 10);
	}
}

void GameScene::TouchTower(EventMouse* event) {
	// ��ȡ�����������
	Vec2 clickPos = event->getLocation();
	//��OpenGL����ϵת��Ϊ��Ļ����ϵ
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// ע����������λ��
	// ���������OpenGL����ϵ�����Ͻ�0��0����Ļ�������½�0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// ת����TMX��ͼ����
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);
	// ��ͼ�Ͽ��Խ���ʱ

	//��ȡ��ǰ�����¼��Ƿ񼤻�
	int hasUpgrade = getHasUpgrade();

	if (!hasUpgrade) {
		//�����µĴ�����ʹ���������
		createTouchLayer();
		createTouchListener();
	}
	auto touch_layer = getTouchLayer();
	auto touch_listener = getTouchListener();

	if (!hasUpgrade)
		this->addChild(touch_layer, 10);

	//���ӹ����������۵ľ���
	Sprite* circle = Sprite::create("CarrotGuardRes/UI/RangeBackground.png");
	Sprite* upgrade_can = Sprite::create("CarrotGuardRes/Towers/upgrade_able.png");
	Sprite* upgrade_not = Sprite::create("CarrotGuardRes/Towers/upgrade_unable.png");
	Sprite* upgrade_max = Sprite::create("CarrotGuardRes/Towers/upgrade_top.png");
	Sprite* sale = Sprite::create("CarrotGuardRes/Towers/sale.png");

	//��õ�ǰ׼���������۵����������Ϣ
	Turret* currentTower;
	float currentRange;
	int currentGrade, currentCost;

	//currentGrade = 3;
	std::string currentName;
	currentTower = (Turret*)this->getChildByTag(mapX * 1000 + mapY);
	currentRange = currentTower->getRange();
	currentName = currentTower->getName();
	currentGrade = isTurretAble[mapX][mapY] % 10;

	//��ö�Ӧ������������
	switch (currentGrade) {
	case 1:
		currentCost = currentTower->getCost2();
		break;
	case 2:
		currentCost = currentTower->getCost3();
		break;
	default:
		currentCost = -1;
		break;
	}


	if (!hasUpgrade) {
		//��������ʱ��Բ�η�Χ
		screenPos = TMXPosToLocation(mapPos);
		circle->setVisible(true);
		circle->setPosition(screenPos);
		circle->setScale(2 * currentRange / circle->getContentSize().width);
		circle->setName("circle");
		touch_layer->addChild(circle);
		//���ö�Ӧ����ͼ��
		if (currentGrade < 3) {
			if (_goldValue >= currentCost) {
				upgrade_can->setVisible(true);
				upgrade_can->setPosition(screenPos.x + _screenWidth * 0.004, screenPos.y + _screenHeight * 0.1);
				upgrade_can->setName("upgrade_can");
				touch_layer->addChild(upgrade_can);
			}
			else {
				upgrade_not->setVisible(true);
				upgrade_not->setPosition(screenPos.x + _screenWidth * 0.004, screenPos.y + _screenHeight * 0.1);
				upgrade_not->setName("upgrade_not");
				touch_layer->addChild(upgrade_not);
			}
		}
		//��ǰ����������
		else {
			upgrade_max->setVisible(true);
			upgrade_max->setPosition(screenPos.x + _screenWidth * 0.004, screenPos.y + _screenHeight * 0.1);
			upgrade_max->setName("upgrade_max");
			touch_layer->addChild(upgrade_max);
		}

		sale->setVisible(true);
		sale->setPosition(screenPos.x + _screenWidth * 0.004, screenPos.y - _screenHeight * 0.1);
		sale->setName("sale");
		touch_layer->addChild(sale);
	}

	//��ȡ��ǰ��������������ص�Ԫ��
	Sprite* Circle, * UpgradeCan, * Sale;
	Circle = (Sprite*)touch_layer->getChildByName("circle");
	UpgradeCan = (Sprite*)touch_layer->getChildByName("upgrade_can");
	Sale = (Sprite*)touch_layer->getChildByName("sale");

	if (!hasUpgrade) {
		EventMouse* temp = new EventMouse(*event);
		setBuildEvent(temp);
	}
	setHasUpgrade(1);
	EventMouse* buildTower = getBuildEvent();
	touch_listener->onMouseDown = [this, Circle, UpgradeCan, Sale, &screenPos, mapPos, touch_layer, touch_listener, buildTower, currentCost](EventMouse* event) {

		Vec2 clickPos = event->getLocation();
		Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);

		//���������ť
		if ((UpgradeCan) && UpgradeCan->getBoundingBox().containsPoint(screenPos)) {//��·��ֵ���ж��Ƿ�Ϊ���Ե����ť
			_eventDispatcher->removeEventListener(touch_listener);
			UpgradeTower(buildTower);
			_goldValue -= currentCost;
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		//������۰�ť
		else if ((Sale) && Sale->getBoundingBox().containsPoint(screenPos)) {
			_eventDispatcher->removeEventListener(touch_listener);
			SaleTower(buildTower);
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		//�����������
		else {
			_eventDispatcher->removeEventListener(touch_listener);//TB�������Ƴ�lisenner����ֹ���lisenner�������bug
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		setHasUpgrade(0);
		};
	if (!hasUpgrade)
		_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

}

void GameScene::UpgradeTower(EventMouse* event) {
	MusicManager::getInstance()->upgradeSound();
	// ��ȡ�����������
	Vec2 clickPos = event->getLocation();
	//��OpenGL����ϵת��Ϊ��Ļ����ϵ
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// ע����������λ��
	// ���������OpenGL����ϵ�����Ͻ�0��0����Ļ�������½�0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// ת����TMX��ͼ����
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);
	//ͨ��tag��ȡ�����ķ���������������������
	Turret* currentTower;
	currentTower = (Turret*)this->getChildByTag(mapX * 1000 + mapY);
	int currentGrade, currentBase;
	currentGrade = isTurretAble[mapX][mapY] % 10;
	currentBase = isTurretAble[mapX][mapY] / 10;
	currentTower->upgrade();
	isTurretAble[mapX][mapY] += 1;

}

void GameScene::SaleTower(EventMouse* event) {
	MusicManager::getInstance()->sellSound();
	// ��ȡ�����������
	Vec2 clickPos = event->getLocation();
	//��OpenGL����ϵת��Ϊ��Ļ����ϵ
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// ע����������λ��
	// ���������OpenGL����ϵ�����Ͻ�0��0����Ļ�������½�0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// ת����TMX��ͼ����
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);

	//ͨ��tag��ȡ���۵ķ����������г���
	Turret* currentTower;
	currentTower = (Turret*)this->getChildByTag(mapX * 1000 + mapY);
	int currentValue;
	currentValue = currentTower->getCost1();
	_goldValue += currentValue / 2;
	isTurretAble[mapX][mapY] = 0;
	this->removeChildByTag(mapX * 1000 + mapY);
}


void GameScene::initLevel()
{
	// ���ؾ���֡�������ӵ�������ܲ�  =========����Ӧ��Ϊ��Թؿ����ݼ���
	auto spriteFrameCache = SpriteFrameCache::getInstance();
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TBList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TFList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TSunList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Monsters.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Health.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/BaseFunc.plist");



	// ��ȡ�ϰ��㣬�����ϰ�������
	_collidable = _tileMap->getLayer("collidable");
	_collidable->setVisible(true);
	// ���йؿ���ͼ15*10������������
	// ��ʼ���ɽ�����������
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10; j++) {
			isTurretAble[i][j] = 0;
		}
	}
	// ���ϰ���λ����Ϊ1,��ʾ���ܽ�������
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10; j++) {
			if (_collidable->getTileGIDAt(Vec2(i, j)) != 0) {
				isTurretAble[i][j] = 1;
			}
		}
	}



	// ���Carrot ����
	auto carrotObject = _tileMap->getObjectGroup("carrotObject");

	// ���carrot��ͼ���󣬼�carrotλ��
	ValueMap carrotValueMap = carrotObject->getObject("carrot");
	int carrotX = carrotValueMap.at("x").asInt();
	int carrotY = carrotValueMap.at("y").asInt();
	// �����ܲ�
	_carrot = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", carrotHealth));
	_carrot->setScale(0.5);
	_carrot->setPosition(carrotX, carrotY);
	_carrot->setName("carrot"); // �������
	_tileMap->addChild(_carrot, 2);


}


void GameScene::TopLabel()

{
	// ע����Ļ���ݵĸ��ڵ�Ӧ����scece ����������Ƭ��ͼ����Ϊ��Ƭ��ͼ���������ţ�
	// �������Ƭ��ͼ���ӽڵ������Ļ��setposition ��������ţ���������Ļ��������
	// 1. ��ʾ�����˶��ٲ�����
	_curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(_currNum, _monsterWave)), "Arial", 32);
	_curNumberLabel->setColor(Color3B::YELLOW);
	_curNumberLabel->setPosition(_screenWidth * 0.45, _screenHeight * 0.95);
	//_tileMap->addChild(_curNumberLabel);
	this->addChild(_curNumberLabel, 2);
	// 2. һ���ж��ٲ�����
	_numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _monsterWave), "Arial", 32);
	_curNumberLabel->setColor(Color3B::YELLOW);
	_numberLabel->setPosition(_screenWidth * 0.53, _screenHeight * 0.95);
	this->addChild(_numberLabel, 2);
	// 3. ���Ͻǽ������
	_goldLabel = Label::createWithSystemFont(StringUtils::format("%d", _goldValue), "Arial-BoldMT", 32);
	_goldLabel->setColor(Color3B::WHITE);
	_goldLabel->setPosition(_screenWidth * 0.125f, _screenHeight * 0.95);
	//_goldLabel->enableOutline(Color4B::WHITE, 2);
	this->addChild(_goldLabel, 2);
	//������Ϸ�����ϲ���ui
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto topImage = Sprite::create("CarrotGuardRes/UI/GameTop.png");
	//topImage->setPosition(Vec2(_screenWidth / 2 + origin.x, _screenHeight + origin.y - topImage->getContentSize().height + _screenHeight * 0.01f));
	topImage->setPosition(Vec2(_screenWidth / 2 + origin.x, _screenHeight + origin.y - _screenHeight * 0.065f));
	topImage->setScale(_screenWidth / topImage->getContentSize().width);
	this->addChild(topImage, 1);

	// ��ͣ��ť
	auto pauseButton = Button::create("CarrotGuardRes/UI/pauseButton.png", "CarrotGuardRes/UI/continueButton.png");
	pauseButton->setPosition(Vec2(_screenWidth / 2 + origin.x + _screenWidth * 0.33, _screenHeight + origin.y - _screenHeight * 0.055f));
	this->addChild(pauseButton, 2);
	pauseButton->addClickEventListener(CC_CALLBACK_1(GameScene::onPauseButton, this));

	// �˵���ť
	auto menuButton = Button::create("CarrotGuardRes/UI/gameMenuNormal.png", "CarrotGuardRes/UI/gameMenuSelected.png");
	menuButton->setPosition(Vec2(_screenWidth / 2 + origin.x + _screenWidth * 0.43, _screenHeight + origin.y - _screenHeight * 0.055f));
	this->addChild(menuButton, 2);
	menuButton->addTouchEventListener([this](Ref* psender, Button::TouchEventType type) {
		switch (type) {
		case Button::TouchEventType::BEGAN:
			break;
		case Button::TouchEventType::MOVED:
			break;
		case Button::TouchEventType::CANCELED:
			break;
		case Button::TouchEventType::ENDED:
			onMenuButton();
			break;
		}
		});


}


void GameScene::CountDown()
{
	auto countBackground = Sprite::create("CarrotGuardRes/UI/countBackground.png");
	auto count1 = Sprite::create("CarrotGuardRes/UI/countOne.png");
	auto count2 = Sprite::create("CarrotGuardRes/UI/countTwo.png");
	auto count3 = Sprite::create("CarrotGuardRes/UI/countThree.png");
	Label* count0 = Label::createWithSystemFont("GO", "Arial-BoldMT", 100);

	countBackground->setPosition(_screenWidth / 2, _screenHeight / 2);
	count1->setPosition(_screenWidth / 2, _screenHeight / 2);
	count2->setPosition(_screenWidth / 2, _screenHeight / 2);
	count3->setPosition(_screenWidth / 2, _screenHeight / 2);
	count0->setPosition(_screenWidth / 2, _screenHeight / 2);

	countBackground->setVisible(false);
	count1->setVisible(false);
	count2->setVisible(false);
	count3->setVisible(false);
	count0->setVisible(false);

	this->addChild(countBackground, 2);
	this->addChild(count1, 2);
	this->addChild(count2, 2);
	this->addChild(count3, 2);
	this->addChild(count0, 2);

	//�浵����Ļص�����
	auto baginSaveGame = CallFunc::create([=] {
		beganSaveGame();
		});

	// ���õ���sequence����
	auto countdown = Sequence::create(CallFunc::create([=] {
		countBackground->setVisible(true);
		count3->setVisible(true);
		MusicManager::getInstance()->countSound();
		}), DelayTime::create(1), CallFunc::create([=] {
			this->removeChild(count3);
			}), CallFunc::create([=] {
				count2->setVisible(true);
				MusicManager::getInstance()->countSound();
				}), DelayTime::create(1), CallFunc::create([=] {
					this->removeChild(count2);
					}), CallFunc::create([=] {
						count1->setVisible(true);
						MusicManager::getInstance()->countSound();
						}), DelayTime::create(1), CallFunc::create([=] {
							this->removeChild(count1);
							count0->setVisible(true);
							MusicManager::getInstance()->downSound();
							}), DelayTime::create(1), CallFunc::create([=] {
								this->removeChild(count0);
								this->removeChild(countBackground);
								// ��Ϸ��ѭ��
								scheduleUpdate();
								// ���ش浵
								if (IS_LOAD_SAVE_GAME) {
									beganSaveGame();
								}
								// ���ɹ���
								generateMonsters();

								}), NULL);


	this->runAction(countdown);
}

void GameScene::onPauseButton(Ref* pSender) {
	MusicManager::getInstance()->buttonSound();
	// ��ȡ��ͣ��ť
	auto button = static_cast<ui::Button*>(pSender);
	int isPaused = getIsPaused();
	// ���ʱ����ͣ
	if (isPaused) {
		button->loadTextures("CarrotGuardRes/UI/pauseButton.png", "CarrotGuardRes/UI/pauseButton.png");
		// �ָ���Ϸ����
		this->scheduleUpdate();
		Director::getInstance()->resume();
		//�Ƴ���ͣ��ʶ
		this->removeChildByName("pauseTop");
		setIsPaused(0);
	}
	// ���ʱ������
	else {
		button->loadTextures("CarrotGuardRes/UI/continueButton.png", "CarrotGuardRes/UI/continueButton.png");

		// ���Ӷ�����ͣ��ʶ
		auto pauseTop = Sprite::create("CarrotGuardRes/UI/pausing.png");
		pauseTop->setName("pauseTop");
		pauseTop->setPosition(Vec2(_screenWidth / 2, _screenHeight * 0.945));
		pauseTop->setScale(2.0f);
		this->addChild(pauseTop, 10);

		//ֹͣ��Ϸ����
		this->unscheduleUpdate();
		Director::getInstance()->pause();
		setIsPaused(1);
	}

}

void GameScene::onMenuButton() {
	MusicManager::getInstance()->buttonSound();

	//ֹͣ��Ϸ����
	this->unscheduleUpdate();
	//ֹͣ���нڵ�Ķ���
	this->pause();//ֹͣ����¼�
	Director::getInstance()->pause();// ֹͣ�����¼�

	//������ɫ���ֲ�
	auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
	menuLayer->setPosition(Vec2::ZERO);
	this->addChild(menuLayer, 10);

	//������ͣ�˵�����
	auto menuBackground = Sprite::create("CarrotGuardRes/UI/gameMenu.png");
	menuBackground->setPosition(Vec2(_screenWidth / 2, _screenHeight / 2));
	menuBackground->setScale(1.5f);
	menuLayer->addChild(menuBackground, 0);

	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	menuLayer->addChild(menu, 1);
	//������ͣ�˵�����ع��ܰ�ť
	auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
	continueButton->setPosition(Vec2(_screenWidth * 0.495, _screenHeight * 0.649));
	continueButton->setScale(1.5);
	auto restartButton = MenuItemImage::create("CarrotGuardRes/UI/restartNormal.png", "CarrotGuardRes/UI/restartSelected.png");
	restartButton->setPosition(Vec2(_screenWidth * 0.495, _screenHeight * 0.51));
	restartButton->setScale(1.5);
	auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
	chooseButton->setPosition(Vec2(_screenWidth * 0.495, _screenHeight * 0.375));
	chooseButton->setScale(1.5);

	// ������Ϸѡ��
	continueButton->setCallback([this, menuLayer](Ref* psender) {
		MusicManager::getInstance()->buttonSound();
		this->removeChild(menuLayer);
		// �ж��ڵ���˵���ť֮ǰ�Ƿ�������ͣ��ť����ֹ����bug
		if (getIsPaused() == 0) {
			// �ָ���Ϸ����
			this->scheduleUpdate();
			// �ָ����нڵ�Ķ���
			this->resume();
			Director::getInstance()->resume();
		}
		});

	//���¿�ʼ��Ϸѡ��
	restartButton->setCallback([this, menuLayer](Ref* psender) {
		MusicManager::getInstance()->buttonSound();
		auto gameScene = GameScene::createSceneWithLevel(1, 0);
		Director::getInstance()->replaceScene(gameScene);
		this->removeChild(menuLayer);
		auto runningScene = Director::getInstance()->getRunningScene();
		auto gameLayer = runningScene->getChildByName("layer");
		Director::getInstance()->resume();
		});

	//ѡ��ؿ�ѡ��
	chooseButton->setCallback([this, menuLayer](Ref* psender) {
		MusicManager::getInstance()->buttonSound();
		auto skylineScene = SkyLineSelection::createScene();
		Director::getInstance()->replaceScene(skylineScene);
		auto runningScene = Director::getInstance()->getRunningScene();
		auto gameLayer = runningScene->getChildByName("layer");
		this->removeChild(menuLayer);
		Director::getInstance()->resume();
		});


	menu->addChild(continueButton, 1);
	menu->addChild(chooseButton, 1);
	menu->addChild(restartButton, 1);

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [menuLayer](Touch* touch, Event* event) {
		return true;
		};


	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, menuLayer);

}


void GameScene::gameOver(int isWin) {
	// ��մ浵
	cocos2d::FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
	std::string path = "Level_" + std::to_string(currentLevel) + "_save.json";
	if (fileUtils->isFileExist(fileUtils->getWritablePath() + path)) {
		fileUtils->writeStringToFile(" ", fileUtils->getWritablePath() + path);
	}
	else {
		CCLOG("File does not exist: %s", (fileUtils->getWritablePath() + path).c_str());
	}

	//ֹͣ��Ϸ����
	this->unscheduleUpdate();
	//ֹͣ���нڵ�Ķ���
	this->pause();

	// ���û�ɫ���ֲ�
	auto menuLayer = LayerColor::create(Color4B(0, 0, 0, 150));
	menuLayer->setPosition(Vec2::ZERO);
	this->addChild(menuLayer, 10);

	// �����˵�
	auto menu = Menu::create();
	menu->setPosition(Vec2::ZERO);
	menuLayer->addChild(menu, 1);

	// ��Ϸʤ��
	if (isWin) {
		//������һ��
		std::string content;

		// ʹ��FileUtils��ȡ�ļ�����
		FileUtils* fileUtils = FileUtils::getInstance();
		std::string filePath = fileUtils->getWritablePath() + "AllGameSave.json";
		if (fileUtils->isFileExist(filePath)) {
			// ��ȡ�ļ�����
			content = fileUtils->getStringFromFile(filePath);
		}
		else {
			CCLOG("File not found: %s", filePath.c_str());
		}
		content[currentLevel] = '1';
		fileUtils->writeStringToFile(content, filePath);
		//������Ϸ��ʤ����
		auto gameWinBackground = Sprite::create("CarrotGuardRes/UI/WinGame.png");
		gameWinBackground->setPosition(Vec2(_screenWidth / 2, _screenHeight / 2));
		gameWinBackground->setScale(1.5f);
		menuLayer->addChild(gameWinBackground, 0);
		//���ӻ�ʤ�Ľ��ܲ���ʶ
		auto goldenCarrot = Sprite::create("CarrotGuardRes/UI/goldenCarrot.png");
		goldenCarrot->setPosition(Vec2(_screenWidth * 0.493, _screenHeight * 0.7));
		menuLayer->addChild(goldenCarrot, 0);

		// ʤ���������ʾ��
		_curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum > _monsterWave ? _monsterWave : _currNum), "Arial", 32);
		_curNumberLabel->setColor(Color3B::YELLOW);
		_curNumberLabel->setPosition(_screenWidth * 0.51, _screenHeight * 0.54);
		Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
		loseWordLeft->setPosition(_screenWidth * 0.36, _screenHeight * 0.54);
		Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
		loseWordRight->setPosition(_screenWidth * 0.60, _screenHeight * 0.545);

		this->addChild(_curNumberLabel, 10);
		this->addChild(loseWordLeft, 10);
		this->addChild(loseWordRight, 10);
		//������Ϸ��ť
		auto continueButton = MenuItemImage::create("CarrotGuardRes/UI/continueNormal.png", "CarrotGuardRes/UI/continueSelected.png");
		continueButton->setPosition(Vec2(_screenWidth * 0.613, _screenHeight * 0.375));
		continueButton->setScale(1.38);

		continueButton->setCallback([this, menuLayer](Ref* psender) {
			MusicManager::getInstance()->buttonSound();
			//����ǰδ�����ŵ����һ�أ��������һ��
			if (currentLevel < 2) {
				auto gameScene = GameScene::createSceneWithLevel(currentLevel + 1, 0);
				Director::getInstance()->replaceScene(gameScene);
			}
			//����ǰ�Ѿ��ǿ��ŵ����һ�أ��򷵻�ѡ��ؿ�����
			else {
				auto skylineScene = SkyLineSelection::createScene();
				Director::getInstance()->replaceScene(skylineScene);
			}
			});
		menu->addChild(continueButton, 1);
	}
	// ��Ϸʧ��
	else {
		auto gameLoseBackground = Sprite::create("CarrotGuardRes/UI/LoseGame.png");
		gameLoseBackground->setPosition(Vec2(_screenWidth / 2 + _screenWidth * 0.01, _screenHeight / 2 + _screenHeight * 0.015));
		gameLoseBackground->setScale(1.5f);
		menuLayer->addChild(gameLoseBackground, 0);

		// ��Ϸʧ�ܵ������ʾ��
		_curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum - 1), "Arial", 32);// ��ʱû��currnumΪʲô���1��������-1
		_curNumberLabel->setColor(Color3B::YELLOW);
		_curNumberLabel->setPosition(_screenWidth * 0.51, _screenHeight * 0.54);
		Label* loseWordLeft = Label::createWithSystemFont("fought off", "Arial", 30);
		loseWordLeft->setPosition(_screenWidth * 0.36, _screenHeight * 0.54);
		Label* loseWordRight = Label::createWithSystemFont("waves", "Arial", 30);
		loseWordRight->setPosition(_screenWidth * 0.60, _screenHeight * 0.54);

		this->addChild(_curNumberLabel, 10);
		this->addChild(loseWordLeft, 10);
		this->addChild(loseWordRight, 10);
		//������Ϸ��ť
		auto againButton = MenuItemImage::create("CarrotGuardRes/UI/AgainNormal.png", "CarrotGuardRes/UI/AgainSelected.png");
		againButton->setPosition(Vec2(_screenWidth * 0.61, _screenHeight * 0.37));
		againButton->setScale(0.9);

		// ���¿�ʼ��ť��ѡ��
		againButton->setCallback([this, menuLayer](Ref* psender) {
			MusicManager::getInstance()->buttonSound();
			auto gameScene = GameScene::createSceneWithLevel(1, 0);
			Director::getInstance()->replaceScene(gameScene);
			this->removeChild(menuLayer);
			auto runningScene = Director::getInstance()->getRunningScene();
			auto gameLayer = runningScene->getChildByName("layer");
			});
		menu->addChild(againButton, 1);

	}


	// ѡ����Ϸ�ؿ���ť
	auto chooseButton = MenuItemImage::create("CarrotGuardRes/UI/chooseLevelNormal.png", "CarrotGuardRes/UI/chooseLevelSelected.png");
	chooseButton->setPosition(Vec2(_screenWidth * 0.38, _screenHeight * 0.37));
	chooseButton->setScale(1.4);



	chooseButton->setCallback([this, menuLayer](Ref* psender) {
		MusicManager::getInstance()->buttonSound();
		auto skylineScene = SkyLineSelection::createScene();
		Director::getInstance()->replaceScene(skylineScene);
		auto runningScene = Director::getInstance()->getRunningScene();
		auto gameLayer = runningScene->getChildByName("layer");
		this->removeChild(menuLayer);
		});



	menu->addChild(chooseButton, 1);

}



// TMX point ->Screen
// ��ͼ��������ת������Ļ����
Vec2 GameScene::TMXPosToLocation(Vec2 pos)
{   // ע�� * _tileMap->getScale() ������
	int x = (int)(pos.x * (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()));
	float pointHeight = _tileMap->getTileSize().height * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR();
	int y = (int)((_tileMap->getMapSize().height * pointHeight) - (pos.y * pointHeight));
	// �����������ת����ת�����Ͻ�eg: (0,0)->(0,640)   // window��Ϸ��Ļ����(960,640)
	// �����ٽ���ת���ɸ��ӵ�����
	x += (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
	y -= (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
#ifdef DEBUG
	CCLOG("x: %lf , y: %lf ", pos.x, pos.y);
	CCLOG("Screen.x: %d, Screen.y: %d", x, y);
#endif // DEBUG



	return Vec2(x, y);

}



// Screen ->TMX point
// ��Ļ����ת���ɵ�ͼ��������
Vec2 GameScene::LocationToTMXPos(Vec2 pos)
{
	int x = (int)(pos.x) / (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR());
	float pointHeight = _tileMap->getTileSize().height * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR();

	//int y = (int)((_tileMap->getMapSize().height*_tileMap->getScale() * pointHeight-pos.y )/ pointHeight);
	int y = (int)((screenSize.height - pos.y) / pointHeight);

#ifdef DEBUG
	CCLOG("x: %lf , y: %lf ", pos.x, pos.y);
	CCLOG("TMX.x: %d, TMX.y: %d", x, y);
#endif // DEBUG
	return Vec2(x, y);
}



// ���ɹ���
// ÿ������5.0s��ÿ�������ɼ��0.5s
void GameScene::generateMonsters() {
	float interval = 5.0f;
	this->schedule([=](float dt) {
		_currNum++;
		if (_currNum <= _monsterWave) {
			generateMonsterWave();
			if (_currNum == _monsterWave) {
				setIsFinalWave(1);

			}

		}
		else {
			unschedule("generateMonsters");
		}

		}, interval, "generateMonsters");

}

// ������hhh,ע��mutable��lambda��ʹ�ã���from++��ע��ÿ�ε��õĻ�lambda�е�from��������һ�λ�����++�������ǹ̶���from+1��
// ͬʱ�ⲿ��from���䣬ע������﷨�㣬
void GameScene::generateMonsterWave() {

	//_monsterNum;
	if (_currNum > _monsterWave) {

		return;
	}
	int end = _everyWave[_currNum];
	this->schedule([=](float dt)mutable {
		int i = 0;
		for (auto monsterData : _monsterDatas) {
			if (i >= _monsterNum) {
				if (_monsterNum < end) {

					auto monster = Monster::createWithSpriteFrameName(monsterData->getName());
					_currentMonsters.pushBack(monster);
					// ê����Ϊ����
					monster->setAnchorPoint(Vec2(0.5f, 0.5f));
					monster->setMaxLifeValue(monsterData->getLifeValue());
					monster->setGold(monsterData->getGold());
					monster->setSpeed(monsterData->getSpeed());
					monster->setPointPath(_pathPoints); // ����·��������
					monster->setName(monsterData->getName());
					this->addChild(monster, 8);
					monster->startMoving();
					_monsterNum++;
					break;
				}
				else {
					unschedule("generateMonsterWave");
				}
			}
			i++;
		}
		}, 0.5f, "generateMonsterWave");
}

// �й��ﵽ���յ㣬���ܲ�����˺�
void GameScene::HurtCarrot(int isHurt) {
	MusicManager::getInstance()->carrotSound();
	if (isHurt)
		carrotHealth--;;
	// �ж���Ϸ�Ƿ����
	if (carrotHealth <= 0) {
		CCLOG("Game Over!");
	}
	else {
		if (_carrot != nullptr) {
			_carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotHealth));
		}

	}
}

// ��������ɾ�����Ϲ���
void GameScene::removeMonster(Monster* monster) {
	_currentMonsters.eraseObject(monster);
}

// ����ִ����
Vector<Monster*>& GameScene::getMonsters() {
	return _currentMonsters;
}


void GameScene::update(float dt)
{
	// **************�����˳��***************
	// ���¹���
	updateMonster();
	// ���´浵
	SaveGame();
	// ����gamemassagebuffer
	udpserver.setGameMassageBuffer(gameMassageBuffer);
	// ������Ϸ����
	updateGameState();
}

void GameScene::updateMonster()
{
	Vector<Monster*> monstersToRemove;
	for (auto monster : _currentMonsters) {
		// �жϹ����Ƿ�����,���ӽ��

		if (monster->getLifeValue() <= 0) {
			MusicManager::getInstance()->normalSound();
			_goldValue += monster->getGold();
			monster->removeHP();//�Ƴ�Ѫ��
			monstersToRemove.pushBack(monster);
			continue;
		}
		else {
			monster->setHP();//����Ѫ��
		}
		// �жϹ����Ƿ񵽴��յ㣬���ܲ�����˺�
		if (monster->getisLastPoint()) {
			HurtCarrot(1);
			monstersToRemove.pushBack(monster);
		}
	}
	// �ڵ��������Ĺ�����ɾ��Ԫ���ǲ���ȫ�ģ���Ϊ�ᵼ�µ�����ʧЧ
	// �Ƴ�����
	for (auto monster : monstersToRemove) {
		monster->removeFromParent();
		_currentMonsters.eraseObject(monster);
		_monsterDeath++;
	}
}

void GameScene::updateGameState()
{
	// ���½�ұ�ǩ
	_goldLabel->setString(StringUtils::format("%d", _goldValue));
	// ���²�����ǩ
	_curNumberLabel->setString(StringUtils::format("%d", _currNum > _monsterWave ? _monsterWave : _currNum));

	// �������һ�����ﶯ��
	if (_currNum == _monsterWave && getIsFinalWave() == 1) {
		auto finalWave = Sprite::create("CarrotGuardRes/UI/finalWave.png");
		finalWave->setName("finalWave");
		finalWave->setPosition(Vec2(_screenWidth / 2, _screenHeight / 2));
		finalWave->setScale(2.0f);
		this->addChild(finalWave, 10);

		// �����ӳٶ������Ƴ��ڵ㶯������϶���
		auto delay = DelayTime::create(1.0f);
		auto remove = RemoveSelf::create();
		auto sequence = Sequence::create(delay, remove, nullptr);

		// ִ����϶���
		finalWave->runAction(sequence);
		setIsFinalWave(0);
	}
	// ʧ��
	if (getCarrotHealth() <= 0) {
		gameOver(0);
		CCLOG("****************GAME OVER***************");
		return;

	}
	// �ɹ�
	if (_monsterDeath >= _monsterAll) {
		gameOver(1);
		CCLOG("***************YOU WIN*******************");
	}

}
// Ҫ�浵����
//int _currNum = 1;            // ��ǰ���ﲨ��
//int _goldValue = 2000;          // ��ҵ�ǰ�������
//int carrotHealth = 5;     // ֱ������Ӱɣ��ܲ�������ֵ
//int isTurretAble[15][10];               // �ɽ�����̨��λ�õ�ͼ�����Ҽ�¼λ������������ȼ�
//Vector<Monster*> _currentMonsters;       // �����ִ�Ĺ���
//int _monsterDeath = 0;                   // ���ݻٵĹ�������������ĺ͵��յ��
//int _monsterNum = 0;             // �Ѿ����ɵĹ�������
void GameScene::SaveGame()
{
	rapidjson::Document document;
	document.SetObject();
	document.AddMember("currentLevel", currentLevel, document.GetAllocator());
	document.AddMember("currNum", _currNum, document.GetAllocator());
	document.AddMember("goldValue", _goldValue, document.GetAllocator());
	document.AddMember("carrotHealth", carrotHealth, document.GetAllocator());
	document.AddMember("monsterDeath", _monsterDeath, document.GetAllocator());
	document.AddMember("monsterNum", _monsterNum, document.GetAllocator());
	rapidjson::Value monsters(rapidjson::kArrayType);
	for (const auto& monster : _currentMonsters) {
		if (monster == nullptr) {
			continue;
		}
		rapidjson::Value monsterObject(rapidjson::kObjectType);
		// string д��
		std::string monsterName = monster->getName();
		rapidjson::Value monsterNameValue(rapidjson::kStringType);
		monsterNameValue.SetString(monsterName.c_str(), monsterName.length(), document.GetAllocator());
		monsterObject.AddMember("name", monsterNameValue, document.GetAllocator());
		monsterObject.AddMember("lifeValue", monster->getLifeValue(), document.GetAllocator());
		monsterObject.AddMember("MaxLifeValue", monster->getMaxLifeValue(), document.GetAllocator());
		monsterObject.AddMember("gold", monster->getGold(), document.GetAllocator());
		monsterObject.AddMember("step", monster->getStep(), document.GetAllocator());
		monsterObject.AddMember("speed", monster->getSpeed(), document.GetAllocator());
		monsterObject.AddMember("screen.x", monster->getPosition().x, document.GetAllocator());
		monsterObject.AddMember("screen.y", monster->getPosition().y, document.GetAllocator());
		monsters.PushBack(monsterObject, document.GetAllocator());
	}
	document.AddMember("monsters", monsters, document.GetAllocator());
	rapidjson::Value TurretMap(rapidjson::kArrayType);
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10; j++) {
			rapidjson::Value isTurretAbleObject(rapidjson::kObjectType);
			isTurretAbleObject.AddMember("x", i, document.GetAllocator());
			isTurretAbleObject.AddMember("y", j, document.GetAllocator());
			isTurretAbleObject.AddMember("value", isTurretAble[i][j], document.GetAllocator());
			TurretMap.PushBack(isTurretAbleObject, document.GetAllocator());
		}
	}
	document.AddMember("TurretMap", TurretMap, document.GetAllocator());
	//****************************
	// ��bug �ڴ���ʴ��󣬵��Ǹ��ֲ�������
	// 
	//****************************

	rapidjson::Value bullets(rapidjson::kArrayType);
	// ��ȡ��ǰ�����е������ӽڵ�
	auto children = this->getChildren();
	// �������ʽ
	std::regex pattern(".*bullet.*");
	for (const auto& child : children) {
		auto sprite = dynamic_cast<Sprite*>(child);// ��childת��ΪSprite����
		// �ж��Ƿ����ӵ�
		if (sprite && std::regex_match(sprite->getName(), pattern)) {//
			rapidjson::Value bulletObject(rapidjson::kObjectType);
			// string д��
			std::string spriteName = sprite->getName();
			rapidjson::Value spriteNameValue(rapidjson::kStringType);
			spriteNameValue.SetString(spriteName.c_str(), spriteName.length(), document.GetAllocator());
			bulletObject.AddMember("name", spriteNameValue, document.GetAllocator());
			bulletObject.AddMember("screen.x", sprite->getPosition().x, document.GetAllocator());
			bulletObject.AddMember("screen.y", sprite->getPosition().y, document.GetAllocator());
			bulletObject.AddMember("contentSize.x", sprite->getContentSize().width, document.GetAllocator());
			bulletObject.AddMember("contentSize.y", sprite->getContentSize().height, document.GetAllocator());
			bullets.PushBack(bulletObject, document.GetAllocator());
		}
	}
	document.AddMember("bullets", bullets, document.GetAllocator());




	// ����һ�� rapidjson::StringBuffer �������ڴ洢 JSON �ַ���
	rapidjson::StringBuffer buffer;// StringBuffer ��һ���ɱ���ַ����У������� std::string һ��ʹ��
	// ����һ�� rapidjson::Writer �������ڽ� JSON �ĵ�д�뵽 StringBuffer ��
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	// �� JSON �ĵ�д�� StringBuffer ��
	document.Accept(writer);// Accept() ����һ�� Document ���󣬽����� JSON ��ʽд�뵽 StringBuffer ��
	gameMassageBuffer = buffer.GetString(); // �� StringBuffer �е��ַ������ std::string ��
	cocos2d::FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
	std::string path = "Level_" + std::to_string(currentLevel) + "_save.json";
	fileUtils->writeStringToFile(buffer.GetString(), fileUtils->getWritablePath() + path);
}

void GameScene::LoadSaveGame()
{
	// rapidjson ����
	rapidjson::Document document;

	// ���ݴ��ݵĹؿ�ֵselectLevel��ö�Ӧ�Ĺؿ������ļ�
	cocos2d::FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
	std::string path = "Level_" + std::to_string(currentLevel) + "_save.json";
	std::string filePath = fileUtils->getWritablePath() + path;

	// ��ȡ�ļ�����
	std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);
	// �ж��ļ��Ƿ�Ϊ��
	if (contentStr.empty() || contentStr == " ") {
		// cocos ����������������ַ�����д��һ���ո񣬴�����մ浵
		// fileUtils->writeStringToFile(" ", fileUtils->getWritablePath() + path); 
		CCLOG("file is empty");
		return;
	}
	// ����contentStr��json���ݣ����浽document��
	document.Parse<0>(contentStr.c_str());

	// ��ȡ�浵����
	_currNum = document["currNum"].GetInt();
	_goldValue = document["goldValue"].GetInt();
	carrotHealth = document["carrotHealth"].GetInt();
	_monsterDeath = document["monsterDeath"].GetInt();
	_monsterNum = document["monsterNum"].GetInt();
	//  ��ùؿ��浵�Ĺ���
	const rapidjson::Value& monsterArray = document["monsters"];
	for (int i = 0; i < monsterArray.Size(); i++) {
		// ���ÿһ����������
		std::string name = monsterArray[i]["name"].GetString();
		float lifeValue = monsterArray[i]["lifeValue"].GetFloat();
		int MaxLifeValue = monsterArray[i]["MaxLifeValue"].GetFloat();
		int gold = monsterArray[i]["gold"].GetInt();
		int step = monsterArray[i]["step"].GetInt();
		float speed = monsterArray[i]["speed"].GetFloat();
		float screenX = monsterArray[i]["screen.x"].GetFloat();
		float screenY = monsterArray[i]["screen.y"].GetFloat();
		// �����ݴ���MonsterData������
		auto monsterData = MonsterData::create();
		monsterData->setName(name);
		monsterData->setCurLifeValue(lifeValue);
		monsterData->setLifeValue(MaxLifeValue);
		monsterData->setGold(gold);
		monsterData->setStep(step);
		monsterData->setSpeed(speed);
		monsterData->setposition(Vec2(screenX, screenY));
		// ���䴫���ؿ����Ｏ����
		_monsterSaveDatas.pushBack(monsterData);
	}
	// ����isTurretAble����
	const rapidjson::Value& TurretMapArray = document["TurretMap"];
	for (int i = 0; i < TurretMapArray.Size(); i++) {
		int x = TurretMapArray[i]["x"].GetInt();
		int y = TurretMapArray[i]["y"].GetInt();
		int value = TurretMapArray[i]["value"].GetInt();
		isTurretAble[x][y] = value;
	}
}

void GameScene::initSaveGame()
{
	// ����carrot
	if (_carrot != nullptr && carrotHealth > 0) {
		_carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotHealth));
	}
	if (_carrot != nullptr && carrotHealth <= 0) {
		_carrot->removeFromParent();
	}
	// ����turret
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10; j++) {
			// �жϵ�ǰλ���Ƿ�������
			if (isTurretAble[i][j] / 10 != 0) {
				// ��ȡ��ǰ��������
				TurretData* turretData = nullptr;
				int count = 1;
				for (const auto& temp : _turretDatas) {
					if (count >= isTurretAble[i][j] / 10) {
						turretData = temp;
						break;
					}
					count++;
				}
				Turret* turret = nullptr;
				if (isTurretAble[i][j] / 10 == 1) {
					turret = Turret_TB::createWithSpriteFrameName("TB", isTurretAble[i][j] % 10);
					turret->setName("TB");
					turret->setLevel(isTurretAble[i][j] % 10);

				}
				else if (isTurretAble[i][j] / 10 == 2) {
					turret = Turret_TSun::createWithSpriteFrameName("TSun", isTurretAble[i][j] % 10);
					turret->setName("TSun");
					turret->setLevel(isTurretAble[i][j] % 10);

				}
				else if (isTurretAble[i][j] / 10 == 3) {
					turret = Turret_TFan::createWithSpriteFrameName("TFan", isTurretAble[i][j] % 10);
					turret->setName("TFan");
					turret->setLevel(isTurretAble[i][j] % 10);

				}
				turret->setTag(i * 1000 + j);
				turret->setPosition(TMXPosToLocation(Vec2(i, j)));
				turret->setCost1(turretData->getCost1());
				turret->setCost2(turretData->getCost2());
				turret->setCost3(turretData->getCost3());
				turret->setDamage(turretData->getDamage());
				turret->setRange(turretData->getRange());
				// �����˺���Χ
				for (int i = 1; i < turret->getLevel(); i++) {
					turret->setDamage(turret->getDamage() * 2.0);
					turret->setRange(turret->getRange() * 2.0);
				}
				turret->setLevel(isTurretAble[i][j] % 10);
				//turret->init();
				this->addChild(turret, 10);
				_currentTurrets.pushBack(turret);
			}
		}
	}
	// ���ع���
	for (const auto& monsterData : _monsterSaveDatas) {
		auto monster = Monster::createWithSpriteFrameName(monsterData->getName());
		_currentMonsters.pushBack(monster);
		// ê����Ϊ����
		monster->setAnchorPoint(Vec2(0.5f, 0.5f));
		monster->setMaxLifeValue(monsterData->getLifeValue());
		monster->setLifeValue(monsterData->getCurLifeValue());
		monster->setGold(monsterData->getGold());
		monster->setSpeed(monsterData->getSpeed());
		monster->setPointPath(_pathPoints); // ����·��������
		monster->setName(monsterData->getName());
		monster->setPosition(monsterData->getposition());
		monster->setStep(monsterData->getStep());
		monster->setHP();
		this->addChild(monster, 8);
		//monster->startMoving();

	}
}

void GameScene::beganSaveGame()
{
	for (auto turret : _currentTurrets)
	{
		turret->init();
	}
	for (auto monster : _currentMonsters) {
		monster->startMoving();
	}
	// �����Ⲩû������Ĺ���
	generateMonsterWave();
}
