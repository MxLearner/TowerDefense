#ifndef _Game_Scene_H_
#define _Game_Scene_H_



#include"cocos2d.h"
#include"Monster.h"
#include"Turret/Turret.h"
#include"Turret/Bullet.h"
#include"Data/PointDelegate.h"
#include"Data/TurretData.h"
#include"Data/MonsterData.h"
#include"json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include"Network/server.h"
#include"Network/client.h"


USING_NS_CC;

// 游戏逻辑
class GameScene :public Layer
{
protected:
	Size screenSize;           // 获取屏幕size 
	float _screenWidth, _screenHeight;  //屏幕宽高

	// 初始化参数
	int _currNum = 0;            // 当前怪物波数
	int _goldValue = 200;          // 玩家当前金币数量
	Sprite* _carrot;          // 萝卜
	int carrotHealth = 5;     // 直接在这加吧，萝卜的生命值

	// 从json中读取
	std::string _tileFile;   // 关卡地图名称，文件路径
	TMXTiledMap* _tileMap;   // 地图
	TMXLayer* _collidable;   // 障碍层，不可建造的位置
	int _monsterAll;                    // 关卡怪物总数量
	int _monsterWave;                     // 怪物波数
	std::vector<int> _everyWave;                  // 每波怪物数量
	Vector<MonsterData*> _monsterDatas;   // 当前关卡所有怪物信息
	Vector<TurretData*> _turretDatas;     // 当前关卡炮台信息
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点

	// 游戏信息记录
	int isTurretAble[15][10];               // 可建造炮台的位置地图，并且记录位置上炮塔种类等级
	Vector<Monster*> _currentMonsters;       // 场上现存的怪物
	Vector<Turret*> _currentTurrets;         // 场上现存的炮塔
	int _monsterDeath = 0;                   // 被摧毁的怪物，包括被打死的和到终点的
	int _isPaused = 0;                       // 标记当前游戏是否暂停
	int _isFinalWave = 0;                   // 标记当前游戏是否是最后一波怪物
	int _carrotTag = 0;                     // 标记萝卜位置的tag
	int _carrotCost = 150;                  //标记点击萝卜回血的花费


	// 顶部标签
	Label* _numberLabel;                  // 显示怪物波数
	Label* _curNumberLabel;               // 显示当前怪物波数
	Label* _goldLabel;                    // 显示当前玩家金币

	//游戏内功能实现需要
	EventMouse* buildEvent;           //记录防御塔建造位置的点击事件
	int hasBuild = 0;                 //记录是否已经调出建造界面
	int hasUpgrade = 0;               //记录是否已经调出升级出售界面
	Layer* touchLayer;				//用于建造升级出售界面的层
	EventListenerMouse* touchListener;//用于建造升级出售界面的监听器

	// 用于保存游戏进度
	int _monsterNum = 0;             // 已经生成的怪物数量
	Vector<MonsterData*> _monsterSaveDatas;   // 当前关卡存档怪物信息
	std::string gameMassageBuffer; // 用于存档的json
	// 联机服务端
	UDPServer udpserver;
	std::mutex serverMutex; // 互斥锁


public:
	//
	//~GameScene();
	// 根据关卡编号创建游戏关卡场景
	static Scene* createSceneWithLevel(int selectLevel, int isSave);
	// 关卡场景初始化
	virtual bool init();
	// 获取json关卡数据
	virtual void LoadLevelData();
	// 关卡初始化，加载地图，萝卜 ，初始化可见炮塔数组
	virtual void initLevel();
	// 屏幕顶部标签
	virtual void TopLabel();
	// 游戏中的菜单选项
	void onMenuButton();
	// 游戏中的暂停按钮选项
	void onPauseButton(Ref* pSender);
	// 游戏结束
	void gameOver(int isWin);

	// 开始时倒计时，也可用于暂停后重新开始
	void CountDown();

	// 鼠标点击事件,用于选择创建炮台
	void onMouseDown(EventMouse* event);

	//点击空地事件
	void TouchLand(EventMouse* event);
	//建造塔事件
	void BuildTower(EventMouse* event, int numTower);
	//点击塔的事件
	void TouchTower(EventMouse* event);
	//升级塔的事件
	void UpgradeTower(EventMouse* event);
	//出售塔的事件
	void SaleTower(EventMouse* event);

	// TMX point ->Screen
	// 地图格子坐标转化成屏幕坐标

	Vec2 TMXPosToLocation(Vec2 pos);
	// Screen ->TMX point
	// 屏幕坐标转化成地图格子坐标	
	Vec2 LocationToTMXPos(Vec2 pos);
	// 生成怪物
	void generateMonsters();
	void generateMonsterWave();

	//getCarrotHealth
	int getCarrotHealth() {
		return carrotHealth;
	}

	// 有怪物到达终点，对萝卜造成伤害，并判断游戏是否失败
	void HurtCarrot(int isHurt);

	// 从数组中删除场上怪物
	void removeMonster(Monster* monster);

	// 获得现存怪物
	Vector<Monster*>& getMonsters();

	// 游戏场景每帧更新
	virtual void update(float dt) override;

	// 更新怪物，判断是否死亡，增加金币，增加标记或到达终点，对萝卜造成伤害
	void updateMonster();
	// 更新游戏状态，更新当前玩家金币标签，判断游戏是否结束：成功或失败
	void updateGameState();

	void setBuildEvent(EventMouse* event) {
		buildEvent = event;
	}

	EventMouse* getBuildEvent() {
		return buildEvent;
	}

	void setHasBuild(int b) {
		hasBuild = b;
	}

	int getHasBuild() {
		return hasBuild;
	}

	void setHasUpgrade(int b) {
		hasUpgrade = b;
	}

	int getHasUpgrade() {
		return hasUpgrade;
	}


	void createTouchLayer() {
		touchLayer = Layer::create();
	}

	void removeTouchLayer() {
		touchLayer = NULL;
	}

	void setTouchLayer(Layer* ptr) {
		touchLayer = ptr;
	}

	Layer* getTouchLayer() {
		return touchLayer;
	}

	void createTouchListener() {
		touchListener = EventListenerMouse::create();
	}

	void removeTouchListener() {
		touchListener = NULL;
	}

	void setTouchListener(EventListenerMouse* ptr) {
		touchListener = ptr;
	}

	EventListenerMouse* getTouchListener() {
		return touchListener;
	}
	// 暂停
	void setIsPaused(int option) {
		_isPaused = option;
	}
	int getIsPaused() {
		return _isPaused;
	}
	// 最后一波
	void setIsFinalWave(int option) {
		_isFinalWave = option;
	}
	int getIsFinalWave() {
		return _isFinalWave;
	}
	// 获取萝卜的tag
	void setCarrotTag(int option) {
		_carrotTag = option;
	}
	int getCarrotTag() {
		return _carrotTag;
	}
	// 存档
	void SaveGame();
	// 读取存档
	void LoadSaveGame();
	// 读取存档后的初始化
	void initSaveGame();
	// 开始存档游戏
	void beganSaveGame();

	// 服务器端
	void startServer();
	//
	void onExit() {
		udpserver.Stop();
		Layer::onExit();
	}

	CREATE_FUNC(GameScene);
};

#endif // !_Game_Scene_H
