#pragma once

#include "GameScene.h"

USING_NS_CC;

class ClientGameScene :public GameScene
{
private:
	UDPClient udpclient;


public:
	// ���ݹؿ���Ŵ�����Ϸ�ؿ�����
	static Scene* createScene();
	virtual bool init()override; //init the scene
	virtual void LoadLevelData()override;
	virtual void initLevel()override;//init the level
	virtual void update(float dt)override;//update the scene
	bool getCurrentLevel();// ��ȡ������������еĹؿ�
	// ��Ļ������ǩ
	virtual void TopLabel()override;
	void startClient();//start the client

	void clearAll();//clear all the data


	void onExit() override {
		udpclient.stop();
		Layer::onExit();
	}
	CREATE_FUNC(ClientGameScene);
};