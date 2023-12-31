#include "clientGameScene.h"
#include"ui/CocosGUI.h"
#include"Turret_TB.h"
#include"Turret_TFan.h"
#include"Turret_TSun.h"
#include"Bullet.h"
using namespace ui;
USING_NS_CC;

static int currentLevel = -1;  // 当前关卡
static int IS_GET_CURRENT_LEVEL = 0; // 是否获取到当前关卡
Scene* ClientGameScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ClientGameScene::create();
	layer->setName("layer"); // 设个名字
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
	// 获取屏幕宽高
	screenSize = Director::getInstance()->getVisibleSize();
	_screenWidth = screenSize.width;
	_screenHeight = screenSize.height;
	// 开启客户端
	std::thread serverThread(&ClientGameScene::startClient, this);
	serverThread.detach();
	// 获取关卡
	if (!getCurrentLevel()) {
		return true;
	}
	// 读取关卡数据 
	LoadLevelData();
	// 设置屏幕数据
	TopLabel();
	// 游戏主循环
	scheduleUpdate();

	return true;
}

void ClientGameScene::LoadLevelData()
{
	// 加载精灵帧，塔与子弹，怪物，萝卜  =========后续应改为针对关卡数据加载
	auto spriteFrameCache = SpriteFrameCache::getInstance();
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TBList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TFList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TSunList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Monsters.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Health.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/BaseFunc.plist");

	// rapidjson 对象
	rapidjson::Document document;

	// 根据传递的关卡值selectLevel获得对应的关卡数据文件
	while (currentLevel == -1) {
		;
	}
	std::string filePath = FileUtils::getInstance()->
		fullPathForFilename(StringUtils::format("CarrotGuardRes/LeveL_%d.data", currentLevel));

	// 读取文件内容
	std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);

	// 解析contentStr中json数据，并存到document中
	document.Parse<0>(contentStr.c_str());

	//***************获取文件内容********************
	// 1. 读取地图文件
	_tileFile = document["tileFile"].GetString();
	// 2. 怪物总数
	_monsterAll = document["monsterAll"].GetInt();
	// 3. 怪物波数
	_monsterWave = document["monsterWave"].GetInt();
	// 4.每波怪物数量
	const rapidjson::Value& waveArray = document["everyWave"];
	for (int i = 0; i < waveArray.Size(); i++) {
		_everyWave.push_back(waveArray[i].GetInt());
	}
	// 调用TMXTiledMap 读取瓦片地图
	_tileMap = TMXTiledMap::create(_tileFile);
	// 缩放瓦片地图，使其填满整个屏幕
	_tileMap->setScaleX(screenSize.width / _tileMap->getContentSize().width);
	_tileMap->setScaleY(screenSize.height / _tileMap->getContentSize().height);
	// 把地图锚点和位置都设置为原点，使地图左下角与屏幕左下角对齐
	_tileMap->setAnchorPoint(Vec2::ZERO);
	_tileMap->setPosition(Vec2::ZERO);
	_tileMap->setName("_tileMap"); // 设个名字
	this->addChild(_tileMap, 1);
	// 获取障碍层，设置障碍层隐藏
	_collidable = _tileMap->getLayer("collidable");
	_collidable->setVisible(true);
	// 所有关卡地图15*10！！！！！！
	// 初始化可建造炮塔数组
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10; j++) {
			isTurretAble[i][j] = 0;
		}
	}
	// 将障碍物位置设为1,表示不能建炮塔了
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10; j++) {
			if (_collidable->getTileGIDAt(Vec2(i, j)) != 0) {
				isTurretAble[i][j] = 1;
			}
		}
	}
	// 6. 获得关卡设定的炮塔
	const rapidjson::Value& turretArray = document["turrets"];
	for (int i = 0; i < turretArray.Size(); i++) {
		// 获取炮塔数据
		std::string name = turretArray[i]["name"].GetString();
		int cost1 = turretArray[i]["Cost1"].GetInt();
		int cost2 = turretArray[i]["Cost2"].GetInt();
		int cost3 = turretArray[i]["Cost3"].GetInt();
		float range = turretArray[i]["range"].GetFloat();
		int damage = turretArray[i]["damage"].GetInt();
		// 传到turretdata
		auto turretData = TurretData::create();
		turretData->setName(name);
		turretData->setCost1(cost1);
		turretData->setCost2(cost2);
		turretData->setCost3(cost3);
		turretData->setRange(range);
		turretData->setDamage(damage);
		// 传到关卡塔集
		_turretDatas.pushBack(turretData);
	}
	// 获得Carrot 对象
	auto carrotObject = _tileMap->getObjectGroup("carrotObject");

	// 获得carrot地图对象，即carrot位置
	ValueMap carrotValueMap = carrotObject->getObject("carrot");
	int carrotX = carrotValueMap.at("x").asInt();
	int carrotY = carrotValueMap.at("y").asInt();
	// 创建萝卜
	_carrot = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", carrotHealth));
	_carrot->setScale(0.5);
	_carrot->setPosition(carrotX, carrotY);
	_carrot->setName("carrot"); // 设个名字
	_tileMap->addChild(_carrot, 2);
}

void ClientGameScene::initLevel()
{

	// rapidjson 对象
	rapidjson::Document document;
	std::string contentStr = udpclient.getMessage();

	// 判断文件是否为空
	if (contentStr.empty()) {
		CCLOG("File is empty");
		return;
	}

	// 解析contentStr中json数据，并存到document中
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

		// 获得关卡存档的怪物
		if (document.HasMember("monsters")) {
			const rapidjson::Value& monsterArray = document["monsters"];
			if (!monsterArray.IsArray()) {
				CCLOG("Monsters data is not an array");
				return;
			}

			for (rapidjson::SizeType i = 0; i < monsterArray.Size(); i++) {
				// 获得每一个怪物数据
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

				// 将数据传到MonsterData对象中
				auto monsterData = MonsterData::create();
				monsterData->setName(name);
				monsterData->setCurLifeValue(lifeValue);
				monsterData->setLifeValue(maxLifeValue);
				monsterData->setGold(gold);
				monsterData->setStep(step);
				monsterData->setSpeed(speed);
				monsterData->setposition(Vec2(screenX, screenY));

				// 将其传到关卡怪物集合中
				_monsterSaveDatas.pushBack(monsterData);
			}
		}
	}
	catch (const std::exception& e) {
		CCLOG("Exception while processing monster data: %s", e.what());
	}

	// 更新isTurretAble数组
	const rapidjson::Value& TurretMapArray = document["TurretMap"];
	for (int i = 0; i < TurretMapArray.Size(); i++) {
		int x = TurretMapArray[i]["x"].GetInt();
		int y = TurretMapArray[i]["y"].GetInt();
		int value = TurretMapArray[i]["value"].GetInt();
		isTurretAble[x][y] = value;
	}

	// 更新carrot
	if (_carrot != nullptr && carrotHealth > 0) {
		_carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotHealth));
	}
	// 加载turret
	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 10; j++) {
			// 判断当前位置是否有炮塔
			if (isTurretAble[i][j] / 10 != 0) {
				// 获取当前炮塔种类
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
				// 更改伤害范围
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
	// 加载怪物
	for (const auto& monsterData : _monsterSaveDatas) {
		auto monster = Monster::createWithSpriteFrameName(monsterData->getName());
		_currentMonsters.pushBack(monster);
		// 锚点设为中心
		monster->setAnchorPoint(Vec2(0.5f, 0.5f));
		monster->setMaxLifeValue(monsterData->getLifeValue());
		monster->setLifeValue(monsterData->getCurLifeValue());
		monster->setGold(monsterData->getGold());
		monster->setSpeed(monsterData->getSpeed());
		monster->setPointPath(_pathPoints); // 传递路径给怪物
		monster->setName(monsterData->getName());
		monster->setPosition(monsterData->getposition());
		monster->setStep(monsterData->getStep());
		monster->setHP();
		this->addChild(monster, 8);
		//monster->startMoving();
	}
	// 加载子弹
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
	// rapidjson 对象
	rapidjson::Document document;
	std::string contentStr = udpclient.getMessage();


	const int tmp = 1e5;
	int i = 1;
	// 等待获取到消息或超时
	while (i < tmp) {
		i++;
		if (!contentStr.empty() && !document.Parse<0>(contentStr.c_str()).HasParseError()) {
			currentLevel = document["currentLevel"].GetInt();
			IS_GET_CURRENT_LEVEL = 1;
			return true;
		}
		// 继续发送请求
		udpclient.Send("update");
		// 更新消息内容
		contentStr = udpclient.getMessage();
	}
	udpclient.stop();
	return false;
}


void ClientGameScene::TopLabel()
{
	// 注意屏幕数据的父节点应该是scece ，而不是瓦片地图，因为瓦片地图进行了缩放，
	// 如果是瓦片地图的子节点基于屏幕的setposition 会进行缩放，被挤出屏幕！！！！
	// 1. 显示出现了多少波怪物
	_curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", std::min(_currNum, _monsterWave)), "Arial", 32);
	_curNumberLabel->setColor(Color3B::RED);
	_curNumberLabel->setPosition(_screenWidth * 0.45, _screenHeight * 0.95);
	//_tileMap->addChild(_curNumberLabel);
	this->addChild(_curNumberLabel, 2);
	// 2. 一共有多少波怪物
	_numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _monsterWave), "Arial", 32);
	_numberLabel->setColor(Color3B::BLUE);
	_numberLabel->setPosition(_screenWidth * 0.55, _screenHeight * 0.95);
	this->addChild(_numberLabel, 2);
	// 3. 右上角金币数量
	_goldLabel = Label::createWithSystemFont(StringUtils::format("%d", _goldValue), "Arial-BoldMT", 32);
	_goldLabel->setColor(Color3B::WHITE);
	_goldLabel->setPosition(_screenWidth * 0.125f, _screenHeight * 0.95);
	//_goldLabel->enableOutline(Color4B::WHITE, 2);
	this->addChild(_goldLabel, 2);
	//添加游戏界面上部的ui
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
	std::lock_guard<std::mutex> lock(serverMutex);//加锁
	udpclient.Receive();

}

void ClientGameScene::clearAll()
{
	_monsterSaveDatas.clear();
	auto children = this->getChildren();
	for (const auto& child : children) {
		auto sprite = dynamic_cast<Sprite*>(child);// 将child转化为Sprite类型
		if (sprite != nullptr && sprite->getName() != "topImage") {
			sprite->removeFromParent();
		}
	}
}
