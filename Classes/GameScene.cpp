#include"GameScene.h"
#include"ui/CocosGUI.h"

using namespace ui;

static int monsterCount = 5;   // ÿһ�����ֶ��ٹ���
static int _currentLevel = 1;  // ��ǰ�ؿ�


#define DEBUG

Scene* GameScene::createSceneWithLevel(int selectLevel)
{   // ��ùؿ����
	_currentLevel = selectLevel;

	auto scene = Scene::create();
	
	auto layer = GameScene::create();

	scene->addChild(layer);
	return scene;
}

bool GameScene::init()
{
	if (!Layer::init()) {
		return false;
	}
	Size size = Director::getInstance()->getVisibleSize();
	_screenWidth = size.width;
	_screenHeight = size.height;

	//****************** ��ȡ�ؿ����� ***************
	
	// rapidjson ����
	rapidjson::Document document;

	// ���ݴ��ݵĹؿ�ֵ������Ӧ�Ĺؿ�����
	std::string filePath = FileUtils::getInstance()->
		fullPathForFilename(StringUtils::format("%d.data", _currentLevel));

#ifdef DEBUG
	CCLOG("File Path: %s", filePath.c_str());
#endif // DEBUG


	std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);

#ifdef DEBUG
	if (contentStr.empty()) {
		CCLOG("Error: File content is empty.");
	}
#endif // DEBUG

	
	document.Parse<0>(contentStr.c_str());

#ifdef DEBUG
	if (document.Parse<0>(contentStr.c_str()).HasParseError()) {
		CCLOG("Error: JSON parse error.");
	}
	else {
		CCLOG("okkkkkkkkkk.");
	}
	if (document.Parse<0>(contentStr.c_str()).HasParseError()) {
		CCLOG("Error: JSON parse error. Code: %u",
			   document.GetParseError());
	}
#endif // DEBUG



#ifdef DEBUG
	if (document.HasMember("tileFile") && document["tileFile"].IsString()) {
		const char* tileFileValue = document["tileFile"].GetString();
		if (tileFileValue) {
			_tileFile = tileFileValue;
		} else {
			CCLOG("Error: empty string.");
		}
	} else {
		CCLOG("Error: not string.");
	}
#endif // DEBUG

	// 1. ��ȡ��ͼ�ļ�
	_tileFile = document["tileFile"].GetString();
	// 2. ��ȡ���ﲨ��
	_number = document["number"].GetInt();
	// 3. ���ֹ���ȡģϵ��
	_delivery = 55;
	// 4. ��ǰ������ֵ�����
	_currentCount = 0;
	// 5. ��ǰ���ﲨ��
	_currNum = 1;
	// 6. ��ʼ���������
	_goldValue = 200;


	// ����TMXTiledMap ��ȡ��Ƭ��ͼ
	_tileMap = TMXTiledMap::create(_tileFile);
	//********************************************************************
	//Fix incorrect rendering of tilemaps with csv data on windows
	// https://github.com/cocos2d/cocos2d-x/pull/20483/files
	// ���windows�¼��ص�ͼ���⣡��������������
	//********************************************************************

	 // ��ȡ���ڴ�С
    Size winSize = Director::getInstance()->getWinSize();

    // ���ó��������Ĵ�СΪ���ڴ�С
    //this->setContentSize(winSize);
	// ������Ƭ��ͼ��ʹ������������Ļ
	_tileMap->setScaleX(winSize.width / _tileMap->getContentSize().width);
    _tileMap->setScaleY(winSize.height / _tileMap->getContentSize().height);
	// �ѵ�ͼê���λ�ö�����Ϊԭ�㣬ʹ��ͼ���½�����Ļ���½Ƕ���
	_tileMap->setAnchorPoint(Vec2::ZERO);
	_tileMap->setPosition(Vec2(-1.0f*0,0.0f));

	this->addChild(_tileMap, 1);

#ifdef DEBUG
	// ����Ƿ�ɹ����ص�ͼ
	if (_tileMap) {
		// ��ȡ "collidable" ��
		_collidable = _tileMap->getLayer("collidable");

		// ����Ƿ�ɹ���ȡ "collidable" ��
		if (_collidable) {
			// �������� _collidable
		} else {
			CCLOG("Error: 'collidable' layer not found in the TMX map.");
		}
	} else {
		CCLOG("Error: Failed to load TMX map.");
	}
#endif // DEBUG

	// ��ȡ�ϰ��㣬�����ϰ�������
	_collidable = _tileMap->getLayer("collidable");
	_collidable->setVisible(true);  // ========================= �����ĳ�false

	// ��ùؿ��趨�Ĺ���
	const rapidjson::Value& monsterArray = document["monsters"];
	for (int i = 0; i < monsterArray.Size(); i++) {
		// ���ÿһ�ֹ���name��lifeValue,gold
		std::string name = monsterArray[i]["name"].GetString();
		int lifeValue = monsterArray[i]["lifeValue"].GetInt();
		int gold = monsterArray[i]["gold"].GetInt();
		// �����ݴ���MonsterData������
		auto monsterData = MonsterData::create();
		monsterData->setName(name);
		monsterData->setLifeValue(lifeValue);
		monsterData->setGold(gold);
		// ���䴫���ؿ����Ｏ����
		_monsterDatas.pushBack(monsterData);
	}
	// ��ȡ�ؿ��趨����̨
	const rapidjson::Value& turretArray = document["turrets"];
	for (int i = 0; i < turretArray.Size(); i++) {
		// ��ȡ��̨����
		std::string name = turretArray[i]["name"].GetString();
		int gold = turretArray[i]["gold"].GetInt();
		std::string bulletName = turretArray[i]["bulletName"].GetString();
		// �����ݴ���TurretData������
		auto turretData = TurretData::create();
		turretData->setName(name);
		turretData->setGold(gold);
		turretData->setBulletName(bulletName);
		// ���䴫���ؿ���̨���ݼ�����
		_turretDatas.pushBack(turretData);

	}
	// ���ﾭ����·��
	const rapidjson::Value& pathtArray = document["path"];
	for (int i = 0; i < pathtArray.Size(); i++) {
		// 1. ��ȡÿ�����ﾭ����·��x,y
		int x = pathtArray[i]["x"].GetInt();
		int y = pathtArray[i]["y"].GetInt();
		// ������ͼ����
		Vec2 tilePoint = Vec2(x, y);
		// ����ͼ����ת������Ļ����
		Vec2 locationPoint = locationForTilePos(tilePoint);

		// Point���ܼ̳�Ref��Vector���ܴ洢
		auto pointDelegate = PointDelegate::create(locationPoint.x, locationPoint.y);

		// ��ÿ������浽·��������
		_pathPoints.pushBack(pointDelegate);
	}

	// ���Carrot ����
	auto carrotObject = _tileMap->getObjectGroup("carrotObject");

	// ���carrot��ͼ���󣬼�carrotλ��
	ValueMap carrotValueMap = carrotObject->getObject("carrot");
	int carrotX = carrotValueMap.at("x").asInt();
	int carrotY = carrotValueMap.at("y").asInt();

#ifdef DEBUG
	CCLOG("carrotX: %d,  carrotY: %d",carrotX,carrotY );
#endif // DEBUG


	// �����ܲ�
	//_carrot = Sprite::createWithSpriteFrameName("carrot.png"); // ================
	_carrot = Sprite::create("carrot.png");
	_carrot ->setScale(0.2);
	_carrot->setPosition(carrotX, carrotY);
	_tileMap->addChild(_carrot, 2);


	// =========������Ļ����
	// 1. ��ʾ�����˶��ٲ�����
	_curNumberLabel = Label::createWithSystemFont("0", "Arial", 32);
	_curNumberLabel->setColor(Color3B::RED);
	_curNumberLabel->setPosition(_screenWidth * 0.45, _screenHeight * 0.7);
	_tileMap->addChild(_curNumberLabel);
	// 2. һ���ж��ٲ�����
	_numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _number), "Arial", 32);
	_numberLabel->setColor(Color3B::BLUE);
	_numberLabel->setPosition(_screenWidth * 0.55, _screenHeight * 0.7);
	_tileMap->addChild(_numberLabel);
	// 3. ���Ͻǽ������
	//auto gold = Sprite::create("");// ===============
	//gold->setPosition(50, _screenHeight * 0.96);
	//_tileMap->addChild(gold, 2);

	_goldLabel = Label::createWithSystemFont("200", "Arial-BoldMT", 32);
	_goldLabel->setColor(Color3B::RED);
	_goldLabel->setPosition(100, _screenHeight * 0.7);
	_tileMap->addChild(_goldLabel);


	// ��ʼ��Ϸʱ������ʱ
	Label* label1 = Label::createWithSystemFont("1", "Arial-BoldMT", 150);
	Label* label2 = Label::createWithSystemFont("2", "Arial-BoldMT", 150);
	Label* label3 = Label::createWithSystemFont("3", "Arial-BoldMT", 150);
	label1->setColor(Color3B::BLUE);
	label2->setColor(Color3B::BLUE);
	label3->setColor(Color3B::BLUE);
	label1->setPosition(_screenWidth / 2, _screenHeight / 2);
	label2->setPosition(_screenWidth / 2, _screenHeight / 2);
	label3->setPosition(_screenWidth / 2, _screenHeight / 2);
	
	// ����label����ʾ
	label1->setVisible(false);
	label2->setVisible(false);
	label3->setVisible(false);
	
	_tileMap->addChild(label1,2);
	_tileMap->addChild(label2,2);
	_tileMap->addChild(label3,2);

	// ���õ���sequence����
	auto countdown = Sequence::create(CallFunc::create([=] {
		label3->setVisible(true);
		}), DelayTime::create(1), CallFunc::create([=] {
			_tileMap->removeChild(label3);
			}), CallFunc::create([=] {
				label2->setVisible(true);
				}), DelayTime::create(1), CallFunc::create([=] {
					_tileMap->removeChild(label2);
					}), CallFunc::create([=] {
						label1->setVisible(true);
						}), DelayTime::create(1), CallFunc::create([=] {
							_tileMap->removeChild(label1);
							// ��Ϸ��ѭ��
							scheduleUpdate();
							}) ,NULL);


	this->runAction(countdown);

	// ��������  =================

	
	return true;
}

// TMP ->OPEN GL
Vec2 GameScene::locationForTilePos(Vec2 pos)
{
	int x = (int)(pos.x * (_tileMap->getTileSize().width / CC_CONTENT_SCALE_FACTOR()));
	float pointHeight = _tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR();
	int y = (int)((_tileMap->getMapSize().height * pointHeight) - (pos.y * pointHeight));
#ifdef DEBUG
	CCLOG("x: %d , y: %d ", x, y);
#endif // DEBUG



	return Vec2(x, y);

}

// OPEN ->TMP
Vec2 GameScene::titleCoordForPosition(Vec2 position)
{
	int x = (int)(position.x) / (_tileMap->getTileSize().width / CC_CONTENT_SCALE_FACTOR());
	float pointHeight = _tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR(); 

	int y = (int)((_tileMap->getMapSize().height * pointHeight-position.y )/ pointHeight);


#ifdef DEBUG
	CCLOG("x: %d , y: %d ", x, y);
#endif // DEBUG
	return Vec2(x, y);
}