#pragma once

#include"cocos2d.h"
#include"PointDelegate.h"
#include"TurretData.h"
#include"MonsterData.h"
#include"json/document.h"
#include"Monster.h"
#include"Turret.h"
#include"Bullet.h"


USING_NS_CC;

// 游戏逻辑
class GameScene :public Layer
{
protected:
	TMXTiledMap* _tileMap;   // 地图
	TMXLayer* _collidable;   // 障碍层
	std::string _tileFile;   // 关卡地图名称
	int _number;             // 关卡共有多少波怪
	int _currentCount;       // 当前怪兽数量
	int _currNum;            // 当前怪物波数
	int _goldValue;          // 玩家当前金币数量
	Sprite* _carrot;          // 萝卜
	float _screenWidth, _screenHeight;  //屏幕宽高
	int _count;              // 游戏计数器;
	int _delivery;            // 出现怪物取模系数 ？？？

	Vector<MonsterData*> _monsterDatas;   // 当前关卡怪物信息
	Vector<TurretData*> _turretDatas;     // 当前关卡炮台信息
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点

	Label* _numberLabel;                  // 显示怪物波数
	Label* _curNumberLabel;               // 显示当前怪物波数
	Label* _goldLabel;                    // 显示当前玩家金币

	Vector<Monster*>_monsterVector;       // 存储出现怪物集合
	bool _isFinish = false;               // 所有怪物是否全部出现

public:

	// 选择当前关卡，并创建，通过关卡数
	static Scene* createSceneWithLevel(int selectLevel);

	virtual bool init();

	void onMouseDown(EventMouse* event);


	// TMP ->OPEN GL
	Vec2 locationForTilePos(Vec2 pos);
	// OPEN ->tmp
	Vec2 titleCoordForPosition(Vec2 position);

	CREATE_FUNC(GameScene);
};

