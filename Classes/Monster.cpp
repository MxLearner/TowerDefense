#include"Monster.h"
#include"GameScene.h"

#define DEBUG
/*
	int _lifeValue;        // 生命值
	LoadingBar* _HP;       // 血条
	float _HPInterval;     //  血条更新量
	Vector<PointDelegate*> _pathPoints;   // 记录有效路径点
	int _gold=10;         // 消灭怪物获得的金币
	float _speed=100;        // 移动速度
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
		auto moveAction = MoveTo::create(0.5, targetPos);
		moveActions.pushBack(moveAction);
		
	}
	// 使用 Sequence 将所有 MoveTo 动作连接起来
    auto sequence = Sequence::create(moveActions);
	
    // 在动作序列完成后执行回调，移除怪物
	// 回调函数执行到达终点逻辑：carrot 掉血，判断游戏是否结束
    auto callback = CallFunc::create([this]() {
        // 获取当前场景
        auto Scene = Director::getInstance()->getRunningScene();

        if (Scene) {
            // 获取 layer 对象
            auto layer = dynamic_cast<GameScene*>(Scene->getChildByName("layer"));
#ifdef DEBUG
            if (layer == nullptr) {
                CCLOG("not found layer");
            }
#endif // DEBUG
            // 获取tilemap
			auto _tileMap = dynamic_cast<TMXTiledMap*>(layer->getChildByName("_tileMap"));
#ifdef DEBUG
            if (_tileMap == nullptr) {
                CCLOG("not found _tileMap");
            }
#endif // DEBUG
            // 获取carrot
            auto carrot=dynamic_cast<Sprite*>(_tileMap->getChildByName("carrot"));
#ifdef DEBUG
            if (carrot == nullptr) {
                CCLOG("not found carrot");
            }
#endif // DEBUG           

            if (carrot) {
                // 执行到达终点的逻辑，例如减少 Carrot 的生命值
                layer->HurtCarrot();
            }
        // *******移除怪物
        // 从场景移除
        removeFromParent();
        // 从现存怪物数组中移除
        layer->removeMonster(this);
        
        }
		
    });

    // 使用 s 同时执行动作序列和回调
    auto s = Sequence::create(sequence, callback, nullptr);

    // 运行 Spawn 动作
    runAction(s);
}
