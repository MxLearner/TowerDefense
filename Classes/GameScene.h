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
	Size screenSize;           // 获取屏幕size 
	TMXTiledMap* _tileMap;   // 地图
	TMXLayer* _collidable;   // 障碍层
	std::string _tileFile;   // 关卡地图名称
	int _number;             // 关卡怪物波数
	int _currentCount;       // 当前怪兽数量
	int _currNum;            // 当前怪物波数
	int _goldValue;          // 玩家当前金币数量
	Sprite* _carrot;          // 萝卜
	int carrotHealth = 5;     // 直接在这加吧，萝卜的生命值
	float _screenWidth, _screenHeight;  //屏幕宽高

	Vector<MonsterData*> _monsterDatas;   // 当前关卡怪物信息
	Vector<TurretData*> _turretDatas;     // 当前关卡炮台信息
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点

	Label* _numberLabel;                  // 显示怪物波数
	Label* _curNumberLabel;               // 显示当前怪物波数
	Label* _goldLabel;                    // 显示当前玩家金币

	Vector<Monster*>_monsterVector;       // 存储出现怪物集合
	bool _isFinish = false;               // 所有怪物是否全部出现

	int isTurretAble[15][10];               // 可建造炮台的位置地图
	Vector<Monster*> _currentMonsters;       // 场上现存的怪物
	Vector<Turret*> _currentTurrets;         // 场上现存的怪物

public:

	// 根据关卡编号创建游戏关卡场景
	static Scene* createSceneWithLevel(int selectLevel);
	// 关卡场景初始化
	virtual bool init();
	// 鼠标点击事件,用于创建炮台
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
	void HurtCarrot() {
		carrotHealth--;
        // 判断游戏是否结束
        if (carrotHealth<=0) {// 先<=0 这样写
            // 游戏结束逻辑，例如显示游戏结束画面、重置游戏等
            // 这里只是一个示例，你需要根据实际情况实现
            CCLOG("Game Over!");
        }
        else {
			if (_carrot != nullptr) {
				_carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", carrotHealth));
			}
    
        }
	}
	// 从数组中删除场上怪物
	void removeMonster(Monster* monster) {
		_currentMonsters.eraseObject(monster);
	}
	// 获得现存怪物
	const Vector<Monster*>& getMonsters()const {
		return _currentMonsters;
	}

	virtual void update(float dt) override;

	CREATE_FUNC(GameScene);
};

