#include"GameScene.h"
#include"ui/CocosGUI.h"
#include"cocos2d.h"


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
	Size size = Director::getInstance()->getVisibleSize();
	_screenWidth = size.width;
	_screenHeight = size.height;
#ifdef DEBUG
	CCLOG("screenWidth:  %lf, screenHeight:  %lf", _screenWidth, _screenHeight);
#endif // DEBUG

	//****************** 读取关卡数据 ***************
	

	// rapidjson 对象
	rapidjson::Document document;

	// 根据传递的关卡值selectLevel获得对应的关卡数据文件
	std::string filePath = FileUtils::getInstance()->
		fullPathForFilename(StringUtils::format("CarrotGuardRes/level_%d.data", currentLevel));

#ifdef DEBUG
	CCLOG("File Path: %s", filePath.c_str());
#endif // DEBUG

	// 读取文件内容
	std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);

#ifdef DEBUG
	if (contentStr.empty()) {
		CCLOG("Error: File content is empty.");
	}
#endif // DEBUG

	// 解析contentStr中json数据，并存到document中
	document.Parse<0>(contentStr.c_str());

#ifdef DEBUG
	if (document.Parse<0>(contentStr.c_str()).HasParseError()) {
		CCLOG("Error: JSON parse error. Code: %u",
			   document.GetParseError());
	}
#endif // DEBUG


// 检查是否读到 tilefile数据
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
	//***************进行关卡初始化******************
	//
	// 1. 读取地图文件
	_tileFile = document["tileFile"].GetString();
	// 2. 读取怪物波数
	_number = document["number"].GetInt();
	// 3. 当前怪物出现的数量
	_currentCount = 0;
	// 4. 当前怪物波数
	_currNum = 1;
	// 5. 初始化金币数量
	_goldValue = 200;
	// 加载精灵帧，塔与子弹，怪物，萝卜  =========后续应改为针对关卡数据加载
	auto spriteFrameCache = SpriteFrameCache::getInstance();
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/TBList.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Monsters.plist");
	spriteFrameCache->addSpriteFramesWithFile("CarrotGuardRes/Carrots.plist");

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
	_tileMap->setScaleX(size.width / _tileMap->getContentSize().width);
    _tileMap->setScaleY(size.height / _tileMap->getContentSize().height);
	// 把地图锚点和位置都设置为原点，使地图左下角与屏幕左下角对齐
	_tileMap->setAnchorPoint(Vec2::ZERO);
	_tileMap->setPosition(Vec2::ZERO);

	this->addChild(_tileMap, 1);

#ifdef DEBUG
	// 检查是否成功加载地图
	if (_tileMap) {
		// 获取 "collidable" 层
		_collidable = _tileMap->getLayer("collidable");

		// 检查是否成功获取 "collidable" 层
		if (_collidable) {
			// 正常处理 _collidable
		} else {
			CCLOG("Error: 'collidable' layer not found in the TMX map.");
		}
	} else {
		CCLOG("Error: Failed to load TMX map.");
	}
#endif // DEBUG

	// 获取障碍层，设置障碍层隐藏
	_collidable = _tileMap->getLayer("collidable");
	_collidable->setVisible(true);  // ========================= 回来改成false

	// 获得关卡设定的怪物
	const rapidjson::Value& monsterArray = document["monsters"];
	for (int i = 0; i < monsterArray.Size(); i++) {
		// 获得每一种怪物name，lifeValue,gold
		std::string name = monsterArray[i]["name"].GetString();
		int lifeValue = monsterArray[i]["lifeValue"].GetInt();
		int gold = monsterArray[i]["gold"].GetInt();
		// 将数据传到MonsterData对象中
		auto monsterData = MonsterData::create();
		monsterData->setName(name);
		monsterData->setLifeValue(lifeValue);
		monsterData->setGold(gold);
		// 将其传到关卡怪物集合中
		_monsterDatas.pushBack(monsterData);
	}
	// 获取关卡设定的炮台
	const rapidjson::Value& turretArray = document["turrets"];
	for (int i = 0; i < turretArray.Size(); i++) {
		// 获取炮台数据
		std::string name = turretArray[i]["name"].GetString();
		int gold = turretArray[i]["gold"].GetInt();
		std::string bulletName = turretArray[i]["bulletName"].GetString();
		// 将数据传到TurretData对象中
		auto turretData = TurretData::create();
		turretData->setName(name);
		turretData->setGold(gold);
		turretData->setBulletName(bulletName);
		// 将其传到关卡炮台数据集合中
		_turretDatas.pushBack(turretData);

	}
	// 怪物经过的路径
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

	// 获得Carrot 对象
	auto carrotObject = _tileMap->getObjectGroup("carrotObject");

	// 获得carrot地图对象，即carrot位置
	ValueMap carrotValueMap = carrotObject->getObject("carrot");
	int carrotX = carrotValueMap.at("x").asInt();
	int carrotY = carrotValueMap.at("y").asInt();

#ifdef DEBUG
	CCLOG("carrotX: %d,  carrotY: %d",carrotX,carrotY );
#endif // DEBUG


	// 创建萝卜
	_carrot = Sprite::createWithSpriteFrameName("Carrot_2.png");
	//_carrot = Sprite::create("carrot.png");
	_carrot ->setScale(0.4);
	_carrot->setPosition(carrotX, carrotY);
	_tileMap->addChild(_carrot, 2);


	// =========设置屏幕数据
	// 注意屏幕数据的父节点应该是scece ，而不是瓦片地图，因为瓦片地图进行了缩放，
	// 如果是瓦片地图的子节点基于屏幕的setposition 会进行缩放，被挤出屏幕！！！！
	// 1. 显示出现了多少波怪物
	_curNumberLabel = Label::createWithSystemFont(StringUtils::format("% d", _currNum), "Arial", 32);
	_curNumberLabel->setColor(Color3B::RED);
	_curNumberLabel->setPosition(_screenWidth * 0.45, _screenHeight * 0.95);
	//_tileMap->addChild(_curNumberLabel);
	this->addChild(_curNumberLabel,2);
	// 2. 一共有多少波怪物
	_numberLabel = Label::createWithSystemFont(StringUtils::format("/%dtimes", _number), "Arial", 32);
	_numberLabel->setColor(Color3B::BLUE);
	_numberLabel->setPosition(_screenWidth * 0.55, _screenHeight * 0.95);
	this->addChild(_numberLabel,2);
	// 3. 右上角金币数量
	//auto gold = Sprite::create("");// ===============
	//gold->setPosition(50, _screenHeight * 0.96);
	//_tileMap->addChild(gold, 2);

	_goldLabel = Label::createWithSystemFont(StringUtils::format("gold: %d", _goldValue), "Arial-BoldMT", 32);
	_goldLabel->setColor(Color3B::BLUE);
	_goldLabel->setPosition(100, _screenHeight * 0.95);
	_goldLabel->enableOutline(Color4B::WHITE, 2);
	this->addChild(_goldLabel,2);


	// 开始游戏时，倒计时
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


#ifdef DEBUG
	auto listener = EventListenerMouse::create();
	listener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif // DEBUG

	// 生成怪物
	generateMonsters();



	return true;
}

void GameScene::onMouseDown(EventMouse* event)
{
#ifdef DEBUG
    // 获取鼠标点击的坐标
    Vec2 clickPos = event->getLocation();
	//将OpenGL坐标系转换为屏幕坐标系
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
    // 注意两个坐标位置
	// OpenGL坐标系，左上角0，0
	// 输出鼠标点击的坐标
    CCLOG(" clickPos.x = %f, clickPos.y = %f", clickPos.x, clickPos.y);
	// 输出鼠标点击的屏幕坐标
	//屏幕坐标系 ，左下角 0，0
    CCLOG("screenPos.x = %f, screenPos.y = %f", screenPos.x, screenPos.y);
#endif // DEBUG


}



// TMX point ->Screen
// 地图格子坐标转化成屏幕坐标
Vec2 GameScene::TMXPosToLocation(Vec2 pos)
{   // 注意 * _tileMap->getScale() ！！！
	int x = (int)(pos.x * (_tileMap->getTileSize().width / CC_CONTENT_SCALE_FACTOR())*_tileMap->getScale());
	float pointHeight = _tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR()*_tileMap->getScale();
	int y = (int)((_tileMap->getMapSize().height * pointHeight) - (pos.y * pointHeight));
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
	int x = (int)(pos.x) / (_tileMap->getTileSize().width / CC_CONTENT_SCALE_FACTOR());
	float pointHeight = _tileMap->getTileSize().height / CC_CONTENT_SCALE_FACTOR(); 

	int y = (int)((_tileMap->getMapSize().height * pointHeight-pos.y )/ pointHeight);


#ifdef DEBUG
	CCLOG("x: %lf , y: %lf ",pos.x,pos.y);
	CCLOG("Screen.x: %d, Screen.y: %d", x, y);
#endif // DEBUG
	return Vec2(x, y);
}
/* 生成怪物
 static int monsterCount = 5;   // 每一波出现多少怪物
int _number;             // 关卡怪物波数
int _currentCount;       // 当前怪兽数量,当前波次已出现多少怪物
int _currNum;            // 当前怪物波数
Vector<MonsterData*> _monsterDatas;   // 当前关卡怪物信息
Vector<PointDelegate*> _pathPoints;   // 记录有效路径点
bool _isFinish = false;               // 所有怪物是否全部出现
*/
void GameScene::generateMonsters() {
    _currNum = 1;

    this->schedule([=](float dt) {
        if (_currNum <=_number ) {
            generateMonsterWave();
            _currNum++;
        } else {
            unschedule("generateMonsters");
        }
    }, 3.0f, "generateMonsters");
}

void GameScene::generateMonsterWave() {
    _currentCount = 0;

    this->schedule([=](float dt) {
        if (_currentCount < monsterCount) {
			// ===========使用 point to point，回来会改的
            auto monster = Monster::createWithSpriteFrameName((*(_monsterDatas.begin()))->getName());
			monster->setPosition(-100, -100);// 随便放到一个看不见的地方
            monster->setPointPath(_pathPoints); // 传递路径给怪物
            this->addChild(monster,10);
			monster->startMoving();

            _currentCount++;
        } else {
            unschedule("generateMonsterWave");
        }
    }, 0.5f, monsterCount, 0, "generateMonsterWave");
}
