


#include"GameScene.h"
#include"ui/CocosGUI.h"
#include"Turret_TB.h"
#include"Turret_TFan.h"
#include"Turret_TSun.h"

#define IS_LOAD_SAVE_GAME 1


using namespace ui;
USING_NS_CC;

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
	// 读取存档
	if (IS_LOAD_SAVE_GAME) {
		LoadSaveGame();
	}
	// 设置屏幕数据
	TopLabel();
	// 开始游戏时，倒计时
	CountDown();
	// 加载存档
	if (IS_LOAD_SAVE_GAME) {
		initSaveGame();
	}

	// 创造鼠标点击时间，用于建塔
	auto listener = EventListenerMouse::create();
	//listener->onMouseDown = CC_CALLBACK_1(GameScene::onClicked, this);
	listener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	// 生成怪物
	generateMonsters();

	//初始化建造塔和升级塔的点击事件
	setBuildEvent(NULL);
	setHasBuild(0);
	setHasUpgrade(0);
	createTouchLayer();
	setTouchLayer(NULL);
	createTouchListener();
	setTouchListener(NULL);

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

	//获取当前建造界面是否激活
	int b = getHasBuild();
	//获取当前升级界面是否激活
	int u = getHasUpgrade();

	// 地图上可以建造时
	if (isTurretAble[mapX][mapY] == 0) {
		if (!u)//这样防止在升级塔的时候误触旁边的空地
			TouchLand(event);
	}
	else if (isTurretAble[mapX][mapY] != 0 && isTurretAble[mapX][mapY] != 1) {
		if (!b)//这样防止在建造塔的时候误触旁边的塔
			TouchTower(event);
	}


}

void GameScene::TouchLand(EventMouse* event) {
	//***************建两个塔，先点左边再点右边，再点右边的空地，不要见他，再点右边的空地，会出现touchlayer空(最近没有发现


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


	//获取当前建造界面是否激活
	int b = getHasBuild();

	//如果没有激活建造界面
	if (!b) {
		//创建新的触摸层和监听器
		createTouchLayer();
		createTouchListener();
	}
	//获取新的触摸层和监听器
	auto touch_layer = getTouchLayer();
	auto touch_listener = getTouchListener();

	//将新的触摸层添加到场景当中
	if (!b)
		this->addChild(touch_layer, 10);


	//创建建造图标精灵
	Sprite* TB_Can = Sprite::create("CarrotGuardRes/Towers/TBottle/CanBuy.png");
	Sprite* TFan_Can = Sprite::create("CarrotGuardRes/Towers/TFan/TFan_CanBuy.png");
	Sprite* TSun_Can = Sprite::create("CarrotGuardRes/Towers/TSun/TSun_CanBuy.png");
	Sprite* TB_Not = Sprite::create("CarrotGuardRes/Towers/TBottle/NotBuy.png");
	Sprite* TFan_Not = Sprite::create("CarrotGuardRes/Towers/TFan/TFan_NotBuy.png");
	Sprite* TSun_Not = Sprite::create("CarrotGuardRes/Towers/TSun/TSun_NotBuy.png");

	//定义三种防御塔的建造成本
	int TB_Cost{}, TFan_Cost{}, TSun_Cost{};

	if (!b) {
		//由地图坐标再转化为屏幕坐标，保证同一地图坐标建造时屏幕坐标相同
		screenPos = TMXPosToLocation(mapPos);
		//定义在迭代器中的计数标签
		int count = 1;
		//遍历获得每一个防御塔的建造成本
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

		//根据当前的金币情况来设置建造防御塔的图标
		if (_goldValue >= TB_Cost) {//******************时不时会说没有初始化内存
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

	//定义防御塔最终显示图标的精灵
	Sprite* TB, * TFan, * TSun;
	//通过name获取“可以建造”的图标，如果不能获取，则说明"不可建造”
	TB = (Sprite*)touch_layer->getChildByName("TB_Can");
	TFan = (Sprite*)touch_layer->getChildByName("TFan_Can");
	TSun = (Sprite*)touch_layer->getChildByName("TSun_Can");

	//记录当前点击事件，可以用于后续获得防御塔建造的位置
	if (!b) {
		EventMouse* temp = new EventMouse(*event);
		setBuildEvent(temp);
	}
	//更新建造状态，说明当前已激活建造界面
	setHasBuild(1);
	//获取防御塔建造位置对应的点击事件
	EventMouse* buildTower = getBuildEvent();
	//当点击鼠标触发的点击事件
	touch_listener->onMouseDown = [this, TB, TFan, TSun, &screenPos, mapPos, touch_layer, touch_listener, buildTower](EventMouse* event) {

		//记录点击的坐标
		Vec2 clickPos = event->getLocation();
		//将点击的坐标转化为UI坐标
		Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);

		//在这里TB指代“可以建造图标”如果不存在，就说明不可以建造，为假。如果存在，且点击到该图标的范围内则为真。
		if ((TB) && TB->getBoundingBox().containsPoint(screenPos)) {
			//建造对应的防御塔
			BuildTower(buildTower, 1);
			//移除最后设置的lisenner，防止多个lisenner相互干扰
			_eventDispatcher->removeEventListener(touch_listener);
			//从场景中移除触摸层
			this->removeChild(touch_layer);
			//彻底移除触摸层
			removeTouchLayer();
			//移除触摸监听器
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
		//如果点击到三个“可以建造”图标之外的地方，则销毁建造界面
		else {
			_eventDispatcher->removeEventListener(touch_listener);
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		//表示建造界面已销毁
		setHasBuild(0);
		};
	if (!b)
		_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

}

void GameScene::BuildTower(EventMouse* event, int numTower) {
	// 获取鼠标点击的坐标
	Vec2 clickPos = event->getLocation();
	//将OpenGL坐标系转换为屏幕坐标系
	Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);
	// 注意两个坐标位置
	// 鼠标点击的是OpenGL坐标系，左上角0，0，屏幕坐标左下角0,0
	Vec2 mapPos = LocationToTMXPos(screenPos);
	// 转化成TMX地图坐标
	int mapX = (int)(mapPos.x), mapY = (int)(mapPos.y);

	//建造需要的金币
	int buildCost{};

	if (isTurretAble[mapX][mapY] == 0) {
		// 代表当前位置上是炮塔
		isTurretAble[mapX][mapY] = 1 + numTower * 10;

		//遍历获得当前炮塔种类的name
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
		_goldValue -= buildCost;//****************************************************时不时说没有初始化内存
		turret->setCost1(turretData->getCost1());
		turret->setCost2(turretData->getCost2());
		turret->setCost3(turretData->getCost3());
		turret->setDamage(turretData->getDamage());
		turret->setRange(turretData->getRange());

		_currentTurrets.pushBack(turret);
		turret->setName(name);
		screenPos = TMXPosToLocation(mapPos);
		turret->setPosition(screenPos);
		//
		turret->setTag(mapX * 1000 + mapY);
		turret->init();
		this->addChild(turret, 10);
	}
}

void GameScene::TouchTower(EventMouse* event) {//*******************从右往左生成三个塔，第三个塔升级的时候会出bug，哪份distance是空的（应该是怪刚刚死的时候点升级

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

	//获取当前升级事件是否激活
	int u = getHasUpgrade();

	if (!u) {
		//创建新的触摸层和触摸监听器
		createTouchLayer();
		createTouchListener();
	}
	auto touch_layer = getTouchLayer();
	auto touch_listener = getTouchListener();

	if (!u)
		this->addChild(touch_layer, 10);

	//添加关于升级出售的精灵
	Sprite* circle = Sprite::create("CarrotGuardRes/UI/RangeBackground.png");
	Sprite* upgrade_can = Sprite::create("CarrotGuardRes/Towers/upgrade_able.png");
	Sprite* upgrade_not = Sprite::create("CarrotGuardRes/Towers/upgrade_unable.png");
	Sprite* upgrade_max = Sprite::create("CarrotGuardRes/Towers/upgrade_top.png");
	Sprite* sale = Sprite::create("CarrotGuardRes/Towers/sale.png");

	//获得当前准备升级出售的塔的相关信息
	Turret* currentTower;
	float currentRange;
	int currentGrade, currentCost;//*************************思考下扣钱放在点击塔还是升级塔里面

	//currentGrade = 3;
	std::string currentName;
	currentTower = (Turret*)this->getChildByTag(mapX * 1000 + mapY);
	currentRange = currentTower->getRange();
	currentName = currentTower->getName();
	CCLOG("currentname: %s", currentName.c_str());//******************************************log里测试
	currentGrade = isTurretAble[mapX][mapY] % 10;

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


	if (!u) {
		screenPos = TMXPosToLocation(mapPos);
		circle->setVisible(true);
		circle->setPosition(screenPos);
		circle->setScale(2 * currentRange / circle->getContentSize().width);
		circle->setName("circle");
		touch_layer->addChild(circle);

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


	Sprite* Circle, * UpgradeCan, * Sale;
	Circle = (Sprite*)touch_layer->getChildByName("circle");
	UpgradeCan = (Sprite*)touch_layer->getChildByName("upgrade_can");
	Sale = (Sprite*)touch_layer->getChildByName("sale");

	if (!u) {
		EventMouse* temp = new EventMouse(*event);
		setBuildEvent(temp);
	}
	setHasUpgrade(1);
	EventMouse* buildTower = getBuildEvent();
	touch_listener->onMouseDown = [this, Circle, UpgradeCan, Sale, &screenPos, mapPos, touch_layer, touch_listener, buildTower, currentCost](EventMouse* event) {
		// 若按下位置在第一个炮塔图标内

		Vec2 clickPos = event->getLocation();
		Vec2 screenPos = Director::getInstance()->convertToUI(clickPos);

		if ((UpgradeCan) && UpgradeCan->getBoundingBox().containsPoint(screenPos)) {//短路求值来判断是否为可以点击按钮
			_eventDispatcher->removeEventListener(touch_listener);
			UpgradeTower(buildTower);
			_goldValue -= currentCost;
			//update(0);
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		else if ((Sale) && Sale->getBoundingBox().containsPoint(screenPos)) {
			_eventDispatcher->removeEventListener(touch_listener);
			SaleTower(buildTower);
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		else {
			_eventDispatcher->removeEventListener(touch_listener);//TB消除后移除lisenner，防止多个lisenner存在造成bug
			this->removeChild(touch_layer);
			removeTouchLayer();
			removeTouchListener();
		}
		setHasUpgrade(0);
		};
	if (!u)
		_eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);

}

void GameScene::UpgradeTower(EventMouse* event) {
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
	Turret* currentTower;
	currentTower = (Turret*)this->getChildByTag(mapX * 1000 + mapY);
	int currentGrade, currentBase;
	currentGrade = isTurretAble[mapX][mapY] % 10;
	currentBase = isTurretAble[mapX][mapY] / 10;
	currentTower->upgrade();
	isTurretAble[mapX][mapY] += 1;

}

void GameScene::SaleTower(EventMouse* event) {
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

	Turret* currentTower;
	currentTower = (Turret*)this->getChildByTag(mapX * 1000 + mapY);
	int currentValue;
	currentValue = currentTower->getCost1();//************************************没找到出售金币，先拿建造的一半来顶替
	_goldValue += currentValue / 2;
	isTurretAble[mapX][mapY] = 0;
	this->removeChildByTag(mapX * 1000 + mapY);
}


void GameScene::initLevel()
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
	_carrot->setScale(0.5);
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

	this->addChild(label1, 2);
	this->addChild(label2, 2);
	this->addChild(label3, 2);

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
							}), NULL);


	this->runAction(countdown);
}


// TMX point ->Screen
// 地图格子坐标转化成屏幕坐标
Vec2 GameScene::TMXPosToLocation(Vec2 pos)
{   // 注意 * _tileMap->getScale() ！！！
	int x = (int)(pos.x * (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()));
	float pointHeight = _tileMap->getTileSize().height * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR();
	int y = (int)((_tileMap->getMapSize().height * pointHeight) - (pos.y * pointHeight));
	// 现在这个坐标转化是转到左上角eg: (0,0)->(0,640)   // window游戏屏幕设置(960,640)
	// 我们再将其转化成格子的中心
	x += (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
	y -= (_tileMap->getTileSize().width * _tileMap->getScale() / CC_CONTENT_SCALE_FACTOR()) / 2.0;
#ifdef DEBUG
	CCLOG("x: %lf , y: %lf ", pos.x, pos.y);
	CCLOG("Screen.x: %d, Screen.y: %d", x, y);
#endif // DEBUG



	return Vec2(x, y);

}



// Screen ->TMX point
// 屏幕坐标转化成地图格子坐标
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



// 生成怪物
// 每波怪物5.0s，每个怪生成间隔0.5s
void GameScene::generateMonsters() {
	_currNum = 0;

	this->schedule([=](float dt) {
		_currNum++;
		if (_currNum <= _monsterWave) {
			_curNumberLabel->setString(StringUtils::format("%d", _currNum));
			generateMonsterWave();

		}
		else {
			unschedule("generateMonsters");
		}

		}, 5.0f, "generateMonsters");
}

// 调好了hhh,注意mutable在lambda的使用，及from++，注意每次调用的话lambda中的from都会在上一次基础上++，而不是固定的from+1，
// 同时外部的from不变，注意这个语法点，
void GameScene::generateMonsterWave() {

	//_monsterNum;
	int end = _everyWave[_currNum];
	this->schedule([=](float dt)mutable {
		int i = 0;
		for (auto monsterData : _monsterDatas) {
			if (i >= _monsterNum) {
				if (_monsterNum < end) {

					auto monster = Monster::createWithSpriteFrameName(monsterData->getName());
					_currentMonsters.pushBack(monster);
					// 锚点设为中心
					monster->setAnchorPoint(Vec2(0.5f, 0.5f));
					monster->setMaxLifeValue(monsterData->getLifeValue());
					monster->setGold(monsterData->getGold());
					monster->setSpeed(monsterData->getSpeed());
					monster->setPointPath(_pathPoints); // 传递路径给怪物
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
	// 更新怪物
	updateMonster();
	// 更新游戏界面
	updateGameState();
	// 更新存档
	SaveGame();
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
		else {
			monster->setHP();//更新血条
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
	_goldLabel->setString(StringUtils::format("%d", _goldValue));
	// 判断游戏是否结束：成功或失败
	//==========待完善============
	// 失败
	if (getCarrotHealth() <= 0) {
		CCLOG("****************GAME OVER***************");
		return;

	}
	// 成功
	if (_monsterDeath >= _monsterAll) {
		CCLOG("***************YOU WIN*******************");
	}
}
// 要存档数据
//int _currNum = 1;            // 当前怪物波数
//int _goldValue = 2000;          // 玩家当前金币数量
//int carrotHealth = 5;     // 直接在这加吧，萝卜的生命值
//int isTurretAble[15][10];               // 可建造炮台的位置地图，并且记录位置上炮塔种类等级
//Vector<Monster*> _currentMonsters;       // 场上现存的怪物
//int _monsterDeath = 0;                   // 被摧毁的怪物，包括被打死的和到终点的
//int _monsterNum = 0;             // 已经生成的怪物数量
void GameScene::SaveGame()
{
	rapidjson::Document document;
	document.SetObject();
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
		//monsterObject.AddMember("name", rapidjson::StringRef(monster->getName().c_str()), document.GetAllocator());
		std::string monsterName = monster->getName();  // 假设 monster->getName() 返回一个 std::string

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
	// 创建一个 rapidjson::StringBuffer 对象，用于存储 JSON 字符串
	rapidjson::StringBuffer buffer;
	// 创建一个 rapidjson::Writer 对象，用于将 JSON 文档写入到 StringBuffer 中
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	// 将 JSON 文档写入 StringBuffer 中
	document.Accept(writer);
	cocos2d::FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
	std::string path = "Level_" + std::to_string(currentLevel) + "_save.json";
	fileUtils->writeStringToFile(buffer.GetString(), fileUtils->getWritablePath() + path);
}

void GameScene::LoadSaveGame()
{
	// rapidjson 对象
	rapidjson::Document document;

	// 根据传递的关卡值selectLevel获得对应的关卡数据文件
	cocos2d::FileUtils* fileUtils = cocos2d::FileUtils::getInstance();
	std::string path = "Level_" + std::to_string(currentLevel) + "_save.json";
	std::string filePath = fileUtils->getWritablePath() + path;

	// 读取文件内容
	std::string contentStr = FileUtils::getInstance()->getStringFromFile(filePath);
	// 判断文件是否为空
	if (contentStr.empty()) {
		CCLOG("file is empty");
		return;
	}
	// 解析contentStr中json数据，并存到document中
	document.Parse<0>(contentStr.c_str());

	// 读取存档数据
	_currNum = document["currNum"].GetInt();
	_goldValue = document["goldValue"].GetInt();
	carrotHealth = document["carrotHealth"].GetInt();
	_monsterDeath = document["monsterDeath"].GetInt();
	_monsterNum = document["monsterNum"].GetInt();
	//  获得关卡存档的怪物
	const rapidjson::Value& monsterArray = document["monsters"];
	for (int i = 0; i < monsterArray.Size(); i++) {
		// 获得每一个怪物数据
		std::string name = monsterArray[i]["name"].GetString();
		int lifeValue = monsterArray[i]["lifeValue"].GetInt();
		int MaxLifeValue = monsterArray[i]["MaxLifeValue"].GetFloat();
		int gold = monsterArray[i]["gold"].GetInt();
		float speed = monsterArray[i]["speed"].GetFloat();
		float screenX = monsterArray[i]["screen.x"].GetFloat();
		float screenY = monsterArray[i]["screen.y"].GetFloat();
		// 将数据传到MonsterData对象中
		auto monsterData = MonsterData::create();
		monsterData->setName(name);
		monsterData->setCurLifeValue(lifeValue);
		monsterData->setLifeValue(MaxLifeValue);
		monsterData->setGold(gold);
		monsterData->setSpeed(speed);
		monsterData->setposition(Vec2(screenX, screenY));
		// 将其传到关卡怪物集合中
		_monsterSaveDatas.pushBack(monsterData);
	}
	// 更新isTurretAble数组
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
	// 更新carrot
	if (_carrot != nullptr) {
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

				}
				else if (isTurretAble[i][j] / 10 == 2) {
					turret = Turret_TSun::createWithSpriteFrameName("TSun", isTurretAble[i][j] % 10);
					turret->setName("TSun");
				}
				else if (isTurretAble[i][j] / 10 == 3) {
					turret = Turret_TFan::createWithSpriteFrameName("TFan", isTurretAble[i][j] % 10);
					turret->setName("TFan");
				}
				turret->setTag(i * 1000 + j);
				turret->setPosition(TMXPosToLocation(Vec2(i, j)));
				turret->setCost1(turretData->getCost1());
				turret->setCost2(turretData->getCost2());
				turret->setCost3(turretData->getCost3());
				turret->setDamage(turretData->getDamage());
				turret->setRange(turretData->getRange());
				turret->setLevel(isTurretAble[i][j] % 10);
				turret->init();
				this->addChild(turret, 10);
				_currentTurrets.pushBack(turret);
			}
		}
	}
}

