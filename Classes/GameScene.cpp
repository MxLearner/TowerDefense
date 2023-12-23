

#include"GameScene.h"
#include"ui/CocosGUI.h"



using namespace ui;
USING_NS_CC;

static int monsterCount = 5;   // 每一波出现多少怪物
static int currentLevel = 1;  // 当前关卡


#define DEBUG
// 根据关卡编号创建游戏关卡场景
Scene* GameScene::createSceneWithLevel(int selectLevel)
{   // 获得关卡编号
	currentLevel = selectLevel;

	auto scene = Scene::create();
	
	auto layer = GameScene::create();
	layer->setName("layer"); // 设个名字
	scene->addChild(layer);
	return scene;
}
// 关卡场景初始化
bool GameScene::init()
{
	if (!Layer::init()) {
		return false;
	}
	// 获取屏幕宽高
	screenSize = Director::getInstance()->getVisibleSize();
	_screenWidth = screenSize.width;
	_screenHeight = screenSize.height;
#ifdef DEBUG
	CCLOG("screenWidth:  %lf, screenHeight:  %lf", _screenWidth, _screenHeight);
#endif // DEBUG

	// 读取关卡数据 
	LoadLevelData();
	// 进行关卡初始化
	initLevel();
	// 设置屏幕数据
	TopLabel();
	// 开始游戏时，倒计时
	CountDown();
    // 创造鼠标点击时间，用于建塔
	auto listener = EventListenerMouse::create();
	//listener->onMouseDown = CC_CALLBACK_1(GameScene::onClicked, this);
	listener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// 生成怪物
	generateMonsters();
	return true;
}
void GameScene::LoadLevelData()
{
		
	// rapidjson 对象
	rapidjson::Document document;

	// 根据传递的关卡值selectLevel获得对应的关卡数据文件
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
	// 5. 获得关卡设定的怪物
	const rapidjson::Value& monsterArray = document["monsters"];
	for (int i = 0; i < monsterArray.Size(); i++) {
		// 获得每一个怪物数据
		std::string name = monsterArray[i]["name"].GetString();
		int lifeValue = monsterArray[i]["lifeValue"].GetInt();
		int gold = monsterArray[i]["gold"].GetInt();
		float speed = monsterArray[i]["speed"].GetFloat();
		// 将数据传到MonsterData对象中
		auto monsterData = MonsterData::create();
		monsterData->setName(name);
		monsterData->setLifeValue(lifeValue);
		monsterData->setGold(gold);
		monsterData->setSpeed(speed);
		// 将其传到关卡怪物集合中
		_monsterDatas.pushBack(monsterData);
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
	// 7. 怪物经过的路径
	// 
	// 因为路径坐标计算需要先加载地图，只能把它放前边了
	// 调用TMXTiledMap 读取瓦片地图
	_tileMap = TMXTiledMap::create(_tileFile);
	//********************************************************************
	//Fix incorrect rendering of tilemaps with csv data on windows
	// https://github.com/cocos2d/cocos2d-x/pull/20483/files
	// 解决windows下加载地图问题！！！！！！！！
	//********************************************************************
	
    // 设置场景容器的大小为窗口大小
    //this->setContentSize(size);
	// 缩放瓦片地图，使其填满整个屏幕
	_tileMap->setScaleX(screenSize.width / _tileMap->getContentSize().width);
    _tileMap->setScaleY(screenSize.height / _tileMap->getContentSize().height);
	// 把地图锚点和位置都设置为原点，使地图左下角与屏幕左下角对齐
	_tileMap->setAnchorPoint(Vec2::ZERO);
	_tileMap->setPosition(Vec2::ZERO);
	_tileMap->setName("_tileMap"); // 设个名字
	this->addChild(_tileMap, 1);

	//**************************************************************
	const rapidjson::Value& pathtArray = document["path"];
	for (int i = 0; i < pathtArray.Size(); i++) {
		// 1. 获取每个怪物经过的路径x,y
		int x = pathtArray[i]["x"].GetInt();
		int y = pathtArray[i]["y"].GetInt();
		// 创建地图坐标
		Vec2 tilePoint = Vec2(x, y);
		// 将地图坐标转化成屏幕坐标
		Vec2 locationPoint = TMXPosToLocation(tilePoint);

		// Point不能继承Ref，Vector不能存储
		auto pointDelegate = PointDelegate::create(locationPoint.x, locationPoint.y);

		// 将每个坐标存到路径集合中
		_pathPoints.pushBack(pointDelegate);
	}

}


// *******************************************鼠标点击建塔
void GameScene::onMouseDown(EventMouse* event)
{
	// 获取鼠标点击的坐标
	Vec2 clickPos = event->getLocation();
	//将OpenGL坐标系转换为屏幕坐标系
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// 注意两个坐标位置
	// 鼠标点击的是OpenGL坐标系，左上角0，0，屏幕坐标左下角0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// 转化成TMX地图坐标
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);
	// 地图上可以建造时


	if(isTurretAble[mapX][mapY] == 0)
		TouchLand(event);
	if (isTurretAble[mapX][mapY] == 2)
		TouchTower(event);

}

void GameScene::TouchLand(EventMouse* event) {
	// 获取鼠标点击的坐标
	Vec2 clickPos = event->getLocation();
	//将OpenGL坐标系转换为屏幕坐标系
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// 注意两个坐标位置
	// 鼠标点击的是OpenGL坐标系，左上角0，0，屏幕坐标左下角0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// 转化成TMX地图坐标
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);
	// 地图上可以建造时


	Sprite* TB = Sprite::create("CarrotGuardRes/Towers/TBottle/CanBuy.png");
	TB->setName("TB");
	screenPos = TMXPosToLocation(mapPos);
	TB->setPosition(screenPos);
	this->addChild(TB,11);


	auto listener = EventListenerMouse::create();
listener->onMouseDown = [this, TB, &screenPos, &mapPos](EventMouse* event) {
    // 若按下位置在第一个炮塔图标内
    Vec2 touchPos = event->getLocationInView();
    touchPos = Director::getInstance()->convertToGL(touchPos);
    if (1) {
        std::string name = (*(_turretDatas.begin()))->getName();
        auto turret = Turret::createWithSpriteFrameName(name);
        _currentTurrets.pushBack(turret);
        turret->setName(name);
        // 由地图坐标再转化为屏幕坐标，保证同一地图坐标建造时屏幕坐标相同
        screenPos = TMXPosToLocation(mapPos);
        turret->setPosition(screenPos);
        turret->init();
        this->addChild(turret, 10);
    }
};

_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	//if (isTurretAble[mapX][mapY] == 0) {
	//	isTurretAble[mapX][mapY] = 2; // 代表上面是炮塔
	//	// 先固定建瓶子，回来再改
	//	std::string name = (*(_turretDatas.begin()))->getName();
	//	auto turret = Turret::createWithSpriteFrameName(name);
	//	_currentTurrets.pushBack(turret);
	//	turret->setName(name);
	//	//由地图坐标再转化为屏幕坐标，保证同一地图坐标建造时屏幕坐标相同
	//	screenPos = TMXPosToLocation(mapPos);
	//	turret->setPosition(screenPos);
	//	turret->init();
	//	this->addChild(turret, 10);
	//}
}


void GameScene::TouchTower(EventMouse* event) {

	// 获取鼠标点击的坐标
	Vec2 clickPos = event->getLocation();
	//将OpenGL坐标系转换为屏幕坐标系
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// 注意两个坐标位置
	// 鼠标点击的是OpenGL坐标系，左上角0，0，屏幕坐标左下角0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// 转化成TMX地图坐标
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);
	// 地图上可以建造时


	Sprite* circle = getChildByName<Sprite*>("circle");
	if (circle) {

		Vec2 circlePos = circle->getPosition();
		float circleRadius = circle->getContentSize().width * 0.1;

		circle->setVisible(false);
		removeChildByName("circle");

		if (isTurretAble[mapX][mapY] == 2) {
			Sprite* circle = Sprite::create("CarrotGuardRes/UI/RangeBackground.png");
			circle->setName("circle");

			//由地图坐标再转化为屏幕坐标，保证同一地图坐标建造时屏幕坐标相同
			screenPos = TMXPosToLocation(mapPos);
			circle->setPosition(screenPos);
			this->addChild(circle, 10);
		}

	}
	else {
		if (isTurretAble[mapX][mapY] == 2) {
			Sprite* circle = Sprite::create("CarrotGuardRes/UI/RangeBackground.png");
			circle->setName("circle");

			//由地图坐标再转化为屏幕坐标，保证同一地图坐标建造时屏幕坐标相同
			screenPos = TMXPosToLocation(mapPos);
			circle->setPosition(screenPos);
			this->addChild(circle, 10);

		}
	}
}



void GameScene::initLevel()
{
	// 加载精灵帧，塔与子弹，怪物，萝卜  =========后续应改为针对关卡数据加载
	auto spriteFrameCache = SpriteFrameCache::getInstance();
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TBList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Monsters.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist");



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

	// 获得Carrot 对象
	auto carrotObject = _tileMap->getObjectGroup("carrotObject");

	// 获得carrot地图对象，即carrot位置
	ValueMap carrotValueMap = carrotObject->getObject("carrot");
	int carrotX = carrotValueMap.at("x").asInt();
	int carrotY = carrotValueMap.at("y").asInt();
	// 创建萝卜
	_carrot = Sprite::createWithSpriteFrameName(StringUtils::format("Carrot_%d.png", carrotHealth));
	_carrot ->setScale(0.5);
	_carrot->setPosition(carrotX, carrotY);
	_carrot->setName("carrot"); // 设个名字
	_tileMap->addChild(_carrot, 2);

}


void GameScene::TopLabel()
{
	// 注意屏幕数据的父节点应该是scece ，而不是瓦片地图，因为瓦片地图进行了缩放，
	// 如果是瓦片地图的子节点基于屏幕的setposition 会进行缩放，被挤出屏幕！！！！
	// 1. 显示出现了多少波怪物
	_curNumberLabel = Label::createWithSystemFont(StringUtils::format("%d", _currNum), "Arial", 32);
	_curNumberLabel->setColor(Color3B::RED);
	_curNumberLabel->setPosition(_screenWidth * 0.45, _screenHeight * 0.95);
	//_tileMap->addChild(_curNumberLabel);
	this->addChild(_curNumberLabel,2);
	// 2. 一共有多少波怪物
	_numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _monsterWave), "Arial", 32);
	_numberLabel->setColor(Color3B::BLUE);
	_numberLabel->setPosition(_screenWidth * 0.55, _screenHeight * 0.95);
	this->addChild(_numberLabel,2);
	// 3. 右上角金币数量
	_goldLabel = Label::createWithSystemFont(StringUtils::format("%d", _goldValue), "Arial-BoldMT", 32);
	_goldLabel->setColor(Color3B::WHITE);
	_goldLabel->setPosition(_screenWidth * 0.125f, _screenHeight * 0.95);
	//_goldLabel->enableOutline(Color4B::WHITE, 2);
	this->addChild(_goldLabel,2);

	


	//添加游戏界面上部的ui
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
	
	// 设置label不显示
	label1->setVisible(false);
	label2->setVisible(false);
	label3->setVisible(false);
	
	this->addChild(label1,2);
	this->addChild(label2,2);
	this->addChild(label3,2);

	// 设置倒数sequence动作
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
							// 游戏主循环
							scheduleUpdate();
							}) ,NULL);


	this->runAction(countdown);
}


// TMX point ->Screen
// 地图格子坐标转化成屏幕坐标
Vec2 GameScene::TMXPosToLocation(Vec2 pos)
{   // 注意 * _tileMap->getScale() ！！！
	int x = (int)(pos.x * (_tileMap->getTileSize().width*_tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()));
	float pointHeight = _tileMap->getTileSize().height*_tileMap->getScale() / CC_CONTENT_SCALE_FACTOR();
	int y = (int)((_tileMap->getMapSize().height * pointHeight) - (pos.y * pointHeight));
	// 现在这个坐标转化是转到左上角eg: (0,0)->(0,640)   // window游戏屏幕设置(960,640)
	// 我们再将其转化成格子的中心
	x += (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
	y-=(_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
#ifdef DEBUG
	CCLOG("x: %lf , y: %lf ",pos.x,pos.y);
	CCLOG("Screen.x: %d, Screen.y: %d", x, y);
#endif // DEBUG



	return Vec2(x, y);

}



// Screen ->TMX point
// 屏幕坐标转化成地图格子坐标
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



// 生成怪物
// 每波怪物5.0s，每个怪生成间隔0.5s
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

// 调好了hhh,注意mutable在lambda的使用，及from++，注意每次调用的话lambda中的from都会在上一次基础上++，而不是固定的from+1，
// 同时外部的from不变，注意这个语法点，
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
					// 锚点设为中心
					monster->setAnchorPoint(Vec2(0.5f, 0.5f));
					monster->setLifeValue(monsterData->getLifeValue());
					monster->setGold(monsterData->getGold());
					monster->setSpeed(monsterData->getSpeed());
					monster->setPointPath(_pathPoints); // 传递路径给怪物
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

// 有怪物到达终点，对萝卜造成伤害
void GameScene::HurtCarrot() {
	carrotHealth--;
	// 判断游戏是否结束
	if (carrotHealth <= 0) {
		CCLOG("Game Over!");
	}
	else {
		if (_carrot != nullptr) {
			_carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotHealth));
		}

	}
}

// 从数组中删除场上怪物
void GameScene::removeMonster(Monster* monster) {
	_currentMonsters.eraseObject(monster);
}

// 获得现存怪物
Vector<Monster*>& GameScene::getMonsters() {
	return _currentMonsters;
}


void GameScene::update(float dt)
{
	
	updateMonster();
	// 先注释掉
	updateGameState();
}

void GameScene::updateMonster()
{
	Vector<Monster*> monstersToRemove;
	for (auto monster : _currentMonsters) {
		// 判断怪物是否被消灭,增加金币
		if (monster->getLifeValue() <= 0) {
			
			_goldValue += monster->getGold();
			monster->removeHP();//移除血条
			monstersToRemove.pushBack(monster);
			continue;
		}
		// 判断怪物是否到达终点，对萝卜造成伤害
		if (monster->getisLastPoint()) {
			HurtCarrot();
			monstersToRemove.pushBack(monster);
		}
	}
	// 在迭代容器的过程中删除元素是不安全的，因为会导致迭代器失效
	// 移除怪物
	for (auto monster : monstersToRemove) {
		monster->removeFromParent();
		_currentMonsters.eraseObject(monster);
		_monsterDeath++;
	}
}

void GameScene::updateGameState()
{
	// 更新金币标签
	_goldLabel ->setString(StringUtils::format("%d", _goldValue));
	// 判断游戏是否结束：成功或失败
	//==========待完善============
	// 失败
	if (getCarrotHealth() <= 0) {
		CCLOG("****************GAME OVER***************");
		return ;

	}
	// 成功
	if (_monsterDeath >= _monsterAll) {
		CCLOG("***************YOU WIN*******************");
	}
}

