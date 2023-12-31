#include "clientGameScene.h"
#include"ui/CocosGUI.h"
#include"Turret_TB.h"
#include"Turret_TFan.h"
#include"Turret_TSun.h"
#include"Bullet.h"
using namespace ui;
USING_NS_CC;

static int currentLevel = -1;  // ��ǰ�ؿ�
static int IS_GET_CURRENT_LEVEL = 0; // �Ƿ��ȡ����ǰ�ؿ�
Scene* ClientGameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ClientGameScene::create();
	layer->setName("layer"); // �������
	scene->addChild(layer);
	if (IS_GET_CURRENT_LEVEL) {
		return  scene;
	}
	else {
		return nullptr;

	}
}

bool ClientGameScene::init()
{
	if (!Layer::init()) {
		return false;
	}
	// ��ȡ��Ļ���
	screenSize = Director::getInstance()->getVisibleSize();
	_screenWidth = screenSize.width;
	_screenHeight = screenSize.height;
	// �����ͻ���
	std::thread serverThread(&ClientGameScene::startClient, this);
	serverThread.detach();
	// ��ȡ�ؿ�
	if (!getCurrentLevel()) {
		return true;
	}
	// ��ȡ�ؿ����� 
	LoadLevelData();
	// ������Ļ����
	TopLabel();
	// ��Ϸ��ѭ��
	scheduleUpdate();

	return true;
}

void ClientGameScene::LoadLevelData()
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

	// rapidjson ����
	rapidjson::Document document;

	// ���ݴ��ݵĹؿ�ֵselectLevel��ö�Ӧ�Ĺؿ������ļ�
	while (currentLevel == -1) {
		;
	}
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
	// ����TMXTiledMap ��ȡ��Ƭ��ͼ
	_tileMap = TMXTiledMap::create(_tileFile);
	// ������Ƭ��ͼ��ʹ������������Ļ
	_tileMap->setScaleX(screenSize.width / _tileMap->getContentSize().width);
	_tileMap->setScaleY(screenSize.height / _tileMap->getContentSize().height);
	// �ѵ�ͼê���λ�ö�����Ϊԭ�㣬ʹ��ͼ���½�����Ļ���½Ƕ���
	_tileMap->setAnchorPoint(Vec2::ZERO);
	_tileMap->setPosition(Vec2::ZERO);
	_tileMap->setName("_tileMap"); // �������
	this->addChild(_tileMap, 1);
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

void ClientGameScene::initLevel()
{

	// rapidjson ����
	rapidjson::Document document;
	std::string contentStr = udpclient.getMessage();

	// �ж��ļ��Ƿ�Ϊ��
	if (contentStr.empty()) {
		CCLOG("File is empty");
		return;
	}

	// ����contentStr��json���ݣ����浽document��
	if (document.Parse<0>(contentStr.c_str()).HasParseError()) {
		CCLOG("JSON parse error");
		return;
	}

	try {
		_currNum = document["currNum"].GetInt();
		_goldValue = document["goldValue"].GetInt();
		carrotHealth = document["carrotHealth"].GetInt();
		_monsterDeath = document["monsterDeath"].GetInt();
		_monsterNum = document["monsterNum"].GetInt();

		// ��ùؿ��浵�Ĺ���
		if (document.HasMember("monsters")) {
			const rapidjson::Value& monsterArray = document["monsters"];
			if (!monsterArray.IsArray()) {
				CCLOG("Monsters data is not an array");
				return;
			}

			for (rapidjson::SizeType i = 0; i < monsterArray.Size(); i++) {
				// ���ÿһ����������
				if (!monsterArray[i].IsObject()) {
					CCLOG("Monster data is not an object");
					continue;
				}
				std::string name = monsterArray[i]["name"].GetString();
				float lifeValue = monsterArray[i]["lifeValue"].GetFloat();
				if (!monsterArray[i].HasMember("MaxLifeValue")) {
					continue;
				}
				int maxLifeValue = monsterArray[i]["MaxLifeValue"].GetFloat();
				int gold = monsterArray[i]["gold"].GetInt();
				int step = monsterArray[i]["step"].GetInt();
				float speed = monsterArray[i]["speed"].GetFloat();
				float screenX = monsterArray[i]["screen.x"].GetFloat();
				float screenY = monsterArray[i]["screen.y"].GetFloat();

				// �����ݴ���MonsterData������
				auto monsterData = MonsterData::create();
				monsterData->setName(name);
				monsterData->setCurLifeValue(lifeValue);
				monsterData->setLifeValue(maxLifeValue);
				monsterData->setGold(gold);
				monsterData->setStep(step);
				monsterData->setSpeed(speed);
				monsterData->setposition(Vec2(screenX, screenY));

				// ���䴫���ؿ����Ｏ����
				_monsterSaveDatas.pushBack(monsterData);
			}
		}
	}
	catch (const std::exception& e) {
		CCLOG("Exception while processing monster data: %s", e.what());
	}

	// ����isTurretAble����
	const rapidjson::Value& TurretMapArray = document["TurretMap"];
	for (int i = 0; i < TurretMapArray.Size(); i++) {
		int x = TurretMapArray[i]["x"].GetInt();
		int y = TurretMapArray[i]["y"].GetInt();
		int value = TurretMapArray[i]["value"].GetInt();
		isTurretAble[x][y] = value;
	}

	// ����carrot
	if (_carrot != nullptr && carrotHealth > 0) {
		_carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotHealth));
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
	// �����ӵ�
	const rapidjson::Value& bulletArray = document["bullets"];
	for (rapidjson::SizeType i = 0; i < bulletArray.Size(); i++) {
		std::string name = bulletArray[i]["name"].GetString();
		float screenX = bulletArray[i]["screen.x"].GetFloat();
		float screenY = bulletArray[i]["screen.y"].GetFloat();
		float contentsizeX = bulletArray[i]["contentSize.x"].GetFloat();
		float contentsizeY = bulletArray[i]["contentSize.y"].GetFloat();
		Bullet* bullet = Bullet::createWithSpriteFrameName(name);
		bullet->setPosition(screenX, screenY);
		bullet->setContentSize(Size(contentsizeX, contentsizeY));
		this->addChild(bullet, 9);
	}
}

void ClientGameScene::update(float dt)
{
	udpclient.Send("update");
	clearAll();
	initLevel();
	updateGameState();
}

bool ClientGameScene::getCurrentLevel()
{
	udpclient.Send("update");
	// rapidjson ����
	rapidjson::Document document;
	std::string contentStr = udpclient.getMessage();


	const int tmp = 1e5;
	int i = 1;
	// �ȴ���ȡ����Ϣ��ʱ
	while (i < tmp) {
		i++;
		if (!contentStr.empty() && !document.Parse<0>(contentStr.c_str()).HasParseError()) {
			currentLevel = document["currentLevel"].GetInt();
			IS_GET_CURRENT_LEVEL = 1;
			return true;
		}
		// ������������
		udpclient.Send("update");
		// ������Ϣ����
		contentStr = udpclient.getMessage();
	}
	udpclient.stop();
	return false;
}


void ClientGameScene::TopLabel()
{
	// ע����Ļ���ݵĸ��ڵ�Ӧ����scece ����������Ƭ��ͼ����Ϊ��Ƭ��ͼ���������ţ�
	// �������Ƭ��ͼ���ӽڵ������Ļ��setposition ��������ţ���������Ļ��������
	// 1. ��ʾ�����˶��ٲ�����
	_curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(_currNum, _monsterWave)), "Arial", 32);
	_curNumberLabel->setColor(Color3B::RED);
	_curNumberLabel->setPosition(_screenWidth * 0.45, _screenHeight * 0.95);
	//_tileMap->addChild(_curNumberLabel);
	this->addChild(_curNumberLabel, 2);
	// 2. һ���ж��ٲ�����
	_numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _monsterWave), "Arial", 32);
	_numberLabel->setColor(Color3B::BLUE);
	_numberLabel->setPosition(_screenWidth * 0.55, _screenHeight * 0.95);
	this->addChild(_numberLabel, 2);
	// 3. ���Ͻǽ������
	_goldLabel = Label::createWithSystemFont(StringUtils::format("%d", _goldValue), "Arial-BoldMT", 32);
	_goldLabel->setColor(Color3B::WHITE);
	_goldLabel->setPosition(_screenWidth * 0.125f, _screenHeight * 0.95);
	//_goldLabel->enableOutline(Color4B::WHITE, 2);
	this->addChild(_goldLabel, 2);
	//�����Ϸ�����ϲ���ui
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto topImage = Sprite::create("CarrotGuardRes/UI/GameTop.png");
	//topImage->setPosition(Vec2(_screenWidth / 2 + origin.x, _screenHeight + origin.y - topImage->getContentSize().height + _screenHeight * 0.01f));
	topImage->setPosition(Vec2(_screenWidth / 2 + origin.x, _screenHeight + origin.y - _screenHeight * 0.065f));
	topImage->setScale(_screenWidth / topImage->getContentSize().width);
	topImage->setName("topImage");
	this->addChild(topImage, 1);
}

void ClientGameScene::startClient()
{
	std::lock_guard<std::mutex> lock(serverMutex);//����
	udpclient.Receive();

}

void ClientGameScene::clearAll()
{
	_monsterSaveDatas.clear();
	auto children = this->getChildren();
	for (const auto& child : children) {
		auto sprite = dynamic_cast<Sprite*>(child);// ��childת��ΪSprite����
		if (sprite != nullptr && sprite->getName() != "topImage") {
			sprite->removeFromParent();
		}
	}
}
