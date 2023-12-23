#pragma once

#include"cocos2d.h"

USING_NS_CC;

// �洢�ؿ�������Ϣ
class LevelData:public Ref
{
public:
	// �����Ϊ˽�б��������ù��нӿ�,�����Ҿ�ֱ����Ϊ���У������ٸ�
	int _number;// ���ﲨ��
	std::string _bg; //�ؿ�����
	std::string _data;//�ؿ������ļ�

	static LevelData* create(int number, std::string bg, std::string data) {
		LevelData* Sprite = new LevelData();
		if (Sprite) {
			Sprite->autorelease();
			Sprite->_number = number;
			Sprite->_bg = bg;
			Sprite->_data = data;
			return Sprite;
		}
		CC_SAFE_DELETE(Sprite);
		return nullptr;
	}
};

