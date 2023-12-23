

#include"GameScene.h"
#include"ui/CocosGUI.h"



using namespace ui;
USING_NS_CC;

static int monsterCount = 5;   // ÿһ�����ֶ��ٹ���
static int currentLevel = 1;  // ��ǰ�ؿ�


#define DEBUG
// ���ݹؿ���Ŵ�����Ϸ�ؿ�����
Scene* GameScene::createSceneWithLevel(int selectLevel)
{   // ��ùؿ����
	currentLevel = selectLevel;

	auto scene = Scene::create();
	
	auto layer = GameScene::create();
	layer->setName("layer"); // �������
	scene->addChild(layer);
	return scene;
}
// �ؿ�������ʼ��
bool GameScene::init()
{
	if (!Layer::init()) {
		return false;
	}
	// ��ȡ��Ļ���
	screenSize = Director::getInstance()->getVisibleSize();
	_screenWidth = screenSize.width;
	_screenHeight = screenSize.height;
#ifdef DEBUG
	CCLOG("screenWidth:  %lf, screenHeight:  %lf", _screenWidth, _screenHeight);
#endif // DEBUG

	// ��ȡ�ؿ����� 
	LoadLevelData();
	// ���йؿ���ʼ��
	initLevel();
	// ������Ļ����
	TopLabel();
	// ��ʼ��Ϸʱ������ʱ
	CountDown();
    // ���������ʱ�䣬���ڽ���
	auto listener = EventListenerMouse::create();
	//listener->onMouseDown = CC_CALLBACK_1(GameScene::onClicked, this);
	listener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// ���ɹ���
	generateMonsters();
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
	// ��ͼ�Ͽ��Խ���ʱ


	if(isTurretAble[mapX][mapY] == 0)
		TouchLand(event);
	if (isTurretAble[mapX][mapY] == 2)
		TouchTower(event);

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


	Sprite* TB = Sprite::create("CarrotGuardRes/Towers/TBottle/CanBuy.png");
	TB->setName("TB");
	screenPos = TMXPosToLocation(mapPos);
	TB->setPosition(screenPos);
	this->addChild(TB,11);


	auto listener = EventListenerMouse::create();
listener->onMouseDown = [this, TB, &screenPos, &mapPos](EventMouse* event) {
    // ������λ���ڵ�һ������ͼ����
    Vec2 touchPos = event->getLocationInView();
    touchPos = Director::getInstance()->convertToGL(touchPos);
    if (1) {
        std::string name = (*(_turretDatas.begin()))->getName();
        auto turret = Turret::createWithSpriteFrameName(name);
        _currentTurrets.pushBack(turret);
        turret->setName(name);
        // �ɵ�ͼ������ת��Ϊ��Ļ���꣬��֤ͬһ��ͼ���꽨��ʱ��Ļ������ͬ
        screenPos = TMXPosToLocation(mapPos);
        turret->setPosition(screenPos);
        turret->init();
        this->addChild(turret, 10);
    }
};

_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//if (isTurretAble[mapX][mapY] == 0) {
	//	isTurretAble[mapX][mapY] = 2; // ��������������
	//	// �ȹ̶���ƿ�ӣ������ٸ�
	//	std::string name = (*(_turretDatas.begin()))->getName();
	//	auto turret = Turret::createWithSpriteFrameName(name);
	//	_currentTurrets.pushBack(turret);
	//	turret->setName(name);
	//	//�ɵ�ͼ������ת��Ϊ��Ļ���꣬��֤ͬһ��ͼ���꽨��ʱ��Ļ������ͬ
	//	screenPos = TMXPosToLocation(mapPos);
	//	turret->setPosition(screenPos);
	//	turret->init();
	//	this->addChild(turret, 10);
	//}
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


	Sprite* circle = getChildByName<Sprite*>("circle");
	if (circle) {

		Vec2 circlePos = circle->getPosition();
		float circleRadius = circle->getContentSize().width * 0.1;

		circle->setVisible(false);
		removeChildByName("circle");

		if (isTurretAble[mapX][mapY] == 2) {
			Sprite* circle = Sprite::create("CarrotGuardRes/UI/RangeBackground.png");
			circle->setName("circle");

			//�ɵ�ͼ������ת��Ϊ��Ļ���꣬��֤ͬһ��ͼ���꽨��ʱ��Ļ������ͬ
			screenPos = TMXPosToLocation(mapPos);
			circle->setPosition(screenPos);
			this->addChild(circle, 10);
		}

	}
	else {
		if (isTurretAble[mapX][mapY] == 2) {
			Sprite* circle = Sprite::create("CarrotGuardRes/UI/RangeBackground.png");
			circle->setName("circle");

			//�ɵ�ͼ������ת��Ϊ��Ļ���꣬��֤ͬһ��ͼ���꽨��ʱ��Ļ������ͬ
			screenPos = TMXPosToLocation(mapPos);
			circle->setPosition(screenPos);
			this->addChild(circle, 10);

		}
	}
}



void GameScene::initLevel()
{
	// ���ؾ���֡�������ӵ�������ܲ�  =========����Ӧ��Ϊ��Թؿ����ݼ���
	auto spriteFrameCache = SpriteFrameCache::getInstance();
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TBList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Monsters.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist");



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
	_carrot ->setScale(0.5);
	_carrot->setPosition(carrotX, carrotY);
	_carrot->setName("carrot"); // �������
	_tileMap->addChild(_carrot, 2);

}


void GameScene::TopLabel()
{
	// ע����Ļ���ݵĸ��ڵ�Ӧ����scece ����������Ƭ��ͼ����Ϊ��Ƭ��ͼ���������ţ�
	// �������Ƭ��ͼ���ӽڵ������Ļ��setposition ��������ţ���������Ļ��������
	// 1. ��ʾ�����˶��ٲ�����
	_curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum), "Arial", 32);
	_curNumberLabel->setColor(Color3B::RED);
	_curNumberLabel->setPosition(_screenWidth * 0.45, _screenHeight * 0.95);
	//_tileMap->addChild(_curNumberLabel);
	this->addChild(_curNumberLabel,2);
	// 2. һ���ж��ٲ�����
	_numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _monsterWave), "Arial", 32);
	_numberLabel->setColor(Color3B::BLUE);
	_numberLabel->setPosition(_screenWidth * 0.55, _screenHeight * 0.95);
	this->addChild(_numberLabel,2);
	// 3. ���Ͻǽ������
	_goldLabel = Label::createWithSystemFont(StringUtils::format("%d", _goldValue), "Arial-BoldMT", 32);
	_goldLabel->setColor(Color3B::WHITE);
	_goldLabel->setPosition(_screenWidth * 0.125f, _screenHeight * 0.95);
	//_goldLabel->enableOutline(Color4B::WHITE, 2);
	this->addChild(_goldLabel,2);

	


	//�����Ϸ�����ϲ���ui
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto topImage = Sprite::create("CarrotGuardRes/UI/GameTop.png");

	//topImage->setPosition(Vec2(_screenWidth / 2 + origin.x, _screenHeight + origin.y - topImage->getContentSize().height + _screenHeight * 0.01f));
	topImage->setPosition(Vec2(_screenWidth / 2 + origin.x, _screenHeight + origin.y - _screenHeight * 0.065f));
	topImage->setScale(_screenWidth / topImage->getContentSize().width);
	this->addChild(topImage, 1);



}


void GameScene::CountDown()
{
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
	
	this->addChild(label1,2);
	this->addChild(label2,2);
	this->addChild(label3,2);

	// ���õ���sequence����
	auto countdown = Sequence::create(CallFunc::create([=] {
		label3->setVisible(true);
		}), DelayTime::create(1), CallFunc::create([=] {
			this->removeChild(label3);
			}), CallFunc::create([=] {
				label2->setVisible(true);
				}), DelayTime::create(1), CallFunc::create([=] {
					this->removeChild(label2);
					}), CallFunc::create([=] {
						label1->setVisible(true);
						}), DelayTime::create(1), CallFunc::create([=] {
							this->removeChild(label1);
							// ��Ϸ��ѭ��
							scheduleUpdate();
							}) ,NULL);


	this->runAction(countdown);
}


// TMX point ->Screen
// ��ͼ��������ת������Ļ����
Vec2 GameScene::TMXPosToLocation(Vec2 pos)
{   // ע�� * _tileMap->getScale() ������
	int x = (int)(pos.x * (_tileMap->getTileSize().width*_tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()));
	float pointHeight = _tileMap->getTileSize().height*_tileMap->getScale() / CC_CONTENT_SCALE_FACTOR();
	int y = (int)((_tileMap->getMapSize().height * pointHeight) - (pos.y * pointHeight));
	// �����������ת����ת�����Ͻ�eg: (0,0)->(0,640)   // window��Ϸ��Ļ����(960,640)
	// �����ٽ���ת���ɸ��ӵ�����
	x += (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
	y-=(_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
#ifdef DEBUG
	CCLOG("x: %lf , y: %lf ",pos.x,pos.y);
	CCLOG("Screen.x: %d, Screen.y: %d", x, y);
#endif // DEBUG



	return Vec2(x, y);

}



// Screen ->TMX point
// ��Ļ����ת���ɵ�ͼ��������
Vec2 GameScene::LocationToTMXPos(Vec2 pos)
{
	int x = (int)(pos.x) / (_tileMap->getTileSize().width*_tileMap->getScale() / CC_CONTENT_SCALE_FACTOR());
	float pointHeight = _tileMap->getTileSize().height*_tileMap->getScale() / CC_CONTENT_SCALE_FACTOR(); 

	//int y = (int)((_tileMap->getMapSize().height*_tileMap->getScale() * pointHeight-pos.y )/ pointHeight);
	int y = (int)((screenSize.height - pos.y) / pointHeight);

#ifdef DEBUG
	CCLOG("x: %lf , y: %lf ",pos.x,pos.y);
	CCLOG("TMX.x: %d, TMX.y: %d", x, y);
#endif // DEBUG
	return Vec2(x, y);
}



// ���ɹ���
// ÿ������5.0s��ÿ�������ɼ��0.5s
void GameScene::generateMonsters() {
    _currNum = 0;

    this->schedule([=](float dt) {
		_currNum++;
        if (_currNum <=_monsterWave ) {
			_curNumberLabel->setString(StringUtils::format("%d", _currNum));
            generateMonsterWave();
            
        } else {
            unschedule("generateMonsters");
        }
		
    }, 5.0f, "generateMonsters");
}

// ������hhh,ע��mutable��lambda��ʹ�ã���from++��ע��ÿ�ε��õĻ�lambda�е�from��������һ�λ�����++�������ǹ̶���from+1��
// ͬʱ�ⲿ��from���䣬ע������﷨�㣬
void GameScene::generateMonsterWave() {
    
	int from = _everyWave[_currNum - 1];
	int end = _everyWave[_currNum];
    this->schedule([=](float dt)mutable {
		int i = 0;
		for (auto monsterData : _monsterDatas) {
			if (i >= from) {
				if (from  < end) {

					 auto monster = Monster::createWithSpriteFrameName(monsterData->getName());
					_currentMonsters.pushBack(monster);
					// ê����Ϊ����
					monster->setAnchorPoint(Vec2(0.5f, 0.5f));
					monster->setLifeValue(monsterData->getLifeValue());
					monster->setGold(monsterData->getGold());
					monster->setSpeed(monsterData->getSpeed());
					monster->setPointPath(_pathPoints); // ����·��������
					this->addChild(monster,8);
					monster->startMoving();
					from++;
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
void GameScene::HurtCarrot() {
	carrotHealth--;
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
	
	updateMonster();
	// ��ע�͵�
	updateGameState();
}

void GameScene::updateMonster()
{
	Vector<Monster*> monstersToRemove;
	for (auto monster : _currentMonsters) {
		// �жϹ����Ƿ�����,���ӽ��
		if (monster->getLifeValue() <= 0) {
			
			_goldValue += monster->getGold();
			monster->removeHP();//�Ƴ�Ѫ��
			monstersToRemove.pushBack(monster);
			continue;
		}
		// �жϹ����Ƿ񵽴��յ㣬���ܲ�����˺�
		if (monster->getisLastPoint()) {
			HurtCarrot();
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
	_goldLabel ->setString(StringUtils::format("%d", _goldValue));
	// �ж���Ϸ�Ƿ�������ɹ���ʧ��
	//==========������============
	// ʧ��
	if (getCarrotHealth() <= 0) {
		CCLOG("****************GAME OVER***************");
		return ;

	}
	// �ɹ�
	if (_monsterDeath >= _monsterAll) {
		CCLOG("***************YOU WIN*******************");
	}
}

