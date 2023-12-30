#include"Monster.h"
#include"GameScene.h"

#define DEBUG
bool Monster::init()
{
	return true;
}
/*
	int _lifeValue;        // ����ֵ
	LoadingBar* _HP;       // Ѫ��
	float _HPInterval;     //  Ѫ��������
	Vector<PointDelegate*> _pathPoints;   // ��¼��Ч·����
	int _gold;         // ��������õĽ��
	float _speed;        // �ƶ��ٶ�
*/
void Monster::startMoving()
{
	Vector<FiniteTimeAction*> moveActions;
	int i = 0;
	for (auto pathPoint : _pathPoints) {
		// ��Ŀ������ת����vec2
		Vec2 targetPos;
		targetPos.x = pathPoint->getX();
		targetPos.y = pathPoint->getY();
		// �ڵ�һ��·��λ�ã������ó�ʼλ��
		if (_step == 0) {
			this->setPosition(targetPos);
			_step++;
			i++;
			continue;
		}
		if (i < _step) {
			i++;
			continue;
		}
		// ����һ�� MoveTo ������ʹ�����ƶ���Ŀ��λ��
		// ���ӿɸı���ٶ�
		auto moveAction = MoveTo::create(0.5 / _speed, targetPos);
		moveActions.pushBack(moveAction);

	}
	// ʹ�� Sequence ������ MoveTo ������������
	auto sequence = Sequence::create(moveActions);
	// �ص���������ǹ��ﵽ���յ�
	auto callback = CallFunc::create([this]() {
		isLastPoint = true;
		});
	// �ڶ���������ɺ�ִ�лص����Ƴ�������߼��Ƶ�gamescene��
	// ʹ�� s ͬʱִ�ж������кͻص�
	auto s = Sequence::create(sequence, callback, nullptr);

	runAction(s);
}
