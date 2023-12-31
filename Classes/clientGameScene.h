#pragma once

#include "GameScene.h"

USING_NS_CC;

class ClientGameScene :public GameScene
{
private:
	UDPClient udpclient;


public:
	// 根据关卡编号创建游戏关卡场景
	static Scene* createScene();
	virtual bool init()override; //init the scene
	virtual void LoadLevelData()override;
	virtual void initLevel()override;//init the level
	virtual void update(float dt)override;//update the scene
	bool getCurrentLevel();// 获取服务端正在运行的关卡
	// 屏幕顶部标签
	virtual void TopLabel()override;
	void startClient();//start the client

	void clearAll();//clear all the data


	void onExit() override {
		udpclient.stop();
		Layer::onExit();
	}
	CREATE_FUNC(ClientGameScene);
};