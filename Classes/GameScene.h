#ifndef _Game_Scene_H_
#define _Game_Scene_H_



#include"cocos2d.h"
#include"Monster.h"
#include"Turret.h"
#include"Bullet.h"
#include"PointDelegate.h"
#include"TurretData.h"
#include"MonsterData.h"
#include"json/document.h"

USING_NS_CC;

// 游戏逻辑
class GameScene :public Layer
{
protected:
	Size screenSize;           // 获取屏幕size 
	float _screenWidth, _screenHeight;  //屏幕宽高

	// 初始化参数
	int _currNum=1;            // 当前怪物波数
	int _goldValue=200;          // 玩家当前金币数量
	Sprite* _carrot;          // 萝卜
	int carrotHealth = 5;     // 直接在这加吧，萝卜的生命值

	// 从json中读取
	std::string _tileFile;   // 关卡地图名称，文件路径
	TMXTiledMap* _tileMap;   // 地图
	TMXLayer* _collidable;   // 障碍层，不可建造的位置
	int _monsterAll ;                    // 关卡怪物总数量
	int _monsterWave ;                     // 怪物波数
	std::vector<int> _everyWave;                  // 每波怪物数量
	Vector<MonsterData*> _monsterDatas;   // 当前关卡所有怪物信息
	Vector<TurretData*> _turretDatas;     // 当前关卡炮台信息
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点

	// 游戏信息记录
	int isTurretAble[15][10];               // 可建造炮台的位置地图，并且记录位置上炮塔种类等级
	Vector<Monster*> _currentMonsters;       // 场上现存的怪物
	Vector<Turret*> _currentTurrets;         // 场上现存的炮塔
	int _monsterDeath = 0;                   // 被摧毁的怪物，包括被打死的和到终点的

	// 顶部标签
	Label* _numberLabel;                  // 显示怪物波数
	Label* _curNumberLabel;               // 显示当前怪物波数
	Label* _goldLabel;                    // 显示当前玩家金币
public:

	// 根据关卡编号创建游戏关卡场景
	static Scene* createSceneWithLevel(int selectLevel);
	// 关卡场景初始化
	virtual bool init();
	// 获取json关卡数据
	void LoadLevelData();
	// 关卡初始化，加载地图，萝卜 ，初始化可见炮塔数组
	void initLevel();
	// 屏幕顶部标签
	void TopLabel();

	// 开始时倒计时，也可用于暂停后重新开始
	void CountDown();
	
	// 鼠标点击事件,用于选择创建炮台
	void onMouseDown(EventMouse* event);
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
	void HurtCarrot();

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

	CREATE_FUNC(GameScene);
};

#endif // !_Game_Scene_H
