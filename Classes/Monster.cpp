#include"Monster.h"


/*
	int _lifeValue;        // 生命值
	LoadingBar* _HP;       // 血条
	float _HPInterval;     //  血条更新量
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点
	int _gold=10;         // 消灭怪物获得的金币
	float _speed=100;        // 移动速度
*/
void Monster::startMoving()
{   Vector<FiniteTimeAction*> moveActions;
	for (auto pathPoint : _pathPoints) {
		// 将目的坐标转化成vec2
		Vec2 targetPos;
		targetPos.x = pathPoint->getX();
		targetPos.y = pathPoint->getY();
		// 创建一个 MoveTo 动作，使怪物移动到目标位置
		auto moveAction = MoveTo::create(0.5, targetPos);
		moveActions.pushBack(moveAction);
		
	}
	// 使用 Sequence 将所有 MoveTo 动作连接起来
    auto sequence = Sequence::create(moveActions);
    // 执行动作序列
    runAction(sequence);
}
