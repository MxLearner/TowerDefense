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
	
    // 在动作序列完成后执行回调，移除怪物
	// 回调函数执行到达终点逻辑：carrot 掉血，判断游戏是否结束
    auto callback = CallFunc::create([this]() {
        removeFromParent();
        // 获取场景
        auto Scene = Director::getInstance()->getRunningScene();

        if (Scene) {
            // 获取 Carrot 对象
            auto layer = dynamic_cast<GameScene*>(Scene->getChildByName("layer"));
#ifdef DEBUG
            if (layer == nullptr) {
                CCLOG("not found layer");
            }
#endif // DEBUG
			auto _tileMap = layer->getChildByName("_tileMap");
#ifdef DEBUG
            if (_tileMap == nullptr) {
                CCLOG("not found _tileMap");
            }
#endif // DEBUG
            auto carrot=dynamic_cast<Sprite*>(_tileMap->getChildByName("carrot"));
#ifdef DEBUG
            if (carrot == nullptr) {
                CCLOG("not found carrot");
            }
#endif // DEBUG           

            if (carrot) {
                // 执行到达终点的逻辑，例如减少 Carrot 的生命值
                layer->HurtCarrot();
                

                // 判断游戏是否结束
                if (layer->getCarrotHealth()<=0) {// 先<=0 这样写
                    // 游戏结束逻辑，例如显示游戏结束画面、重置游戏等
                    // 这里只是一个示例，你需要根据实际情况实现
                    CCLOG("Game Over!");
                }
                else {
                    carrot->setSpriteFrame(StringUtils::format("Carrot_%d.png", layer->getCarrotHealth()));
                }

            }
        }
    });

    // 使用 s 同时执行动作序列和回调
    auto s = Sequence::create(sequence, callback, nullptr);

    // 运行 Spawn 动作
    runAction(s);
}
