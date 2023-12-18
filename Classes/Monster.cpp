#include"Monster.h"
#include"GameScene.h"

#define DEBUG
bool Monster::init()
{
    return true;
}
/*
	int _lifeValue;        // 生命值
	LoadingBar* _HP;       // 血条
	float _HPInterval;     //  血条更新量
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点
	int _gold;         // 消灭怪物获得的金币
	float _speed;        // 移动速度
*/
void Monster::startMoving()
{   
    Vector<FiniteTimeAction*> moveActions;
    int mark = 1;
	for (auto pathPoint : _pathPoints) {
		// 将目的坐标转化成vec2
		Vec2 targetPos;
		targetPos.x = pathPoint->getX();
		targetPos.y = pathPoint->getY();
        // 在第一个路线位置，先设置初始位置
        if (mark == 1) {
            this->setPosition(targetPos);
            mark--;
            continue;
        }
		// 创建一个 MoveTo 动作，使怪物移动到目标位置
        // 增加可改变的速度
		auto moveAction = MoveTo::create(0.5/_speed, targetPos);
		moveActions.pushBack(moveAction);
		
	}
	// 使用 Sequence 将所有 MoveTo 动作连接起来
    auto sequence = Sequence::create(moveActions);
    // 回调函数，标记怪物到达终点
    auto callback = CallFunc::create([this]() {
        isLastPoint = true;
        });
    // 在动作序列完成后执行回调，移除怪物，此逻辑移到gamescene，
    // 使用 s 同时执行动作序列和回调
    auto s = Sequence::create(sequence, callback, nullptr);

    runAction(s);
}
