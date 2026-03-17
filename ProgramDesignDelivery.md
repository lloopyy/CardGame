# CardGame 程序设计交付文档

## 1. 当前架构
- view 负责展示和收点击。
- controller 负责串业务。
- model 负责存数据。
- service 放无状态规则。
- manager 放有状态能力（比如 undo）。

### 1.1 目录
- configs：放静态配置和加载逻辑。
- models：放运行时数据结构，比如 CardModel、GameModel、UndoModel。
- views：只做显示和输入回调，比如 CardView、GameView。
- controllers：业务调度中心，比如 GameController、CardController。
- managers：有状态管理能力，这里核心是 UndoManager。
- services：无状态规则和转换，比如 CardRuleService、GameModelFromLevelGenerator。
- utils：通用小工具，比如 CardUtils。

### 1.2 一次点击大概怎么走
1. 玩家点了一张牌，先到 view。
2. view 通过回调把事件交给 controller。
3. controller 调 service 和 manager，改 model。
4. controller 通知 view 刷新和播动画。
5. 这次操作的回退记录会进入 UndoManager，后面点撤销就能回放。

## 2. 怎么新增一个卡牌

### 2.1 只加普通牌（最常见）
这种情况最简单：只改关卡，不改规则。

1. 去关卡 JSON 加一条配置
- 文件：Resources/res/levels/level_x.json
- 加在 Playfield 或 Stack 都可以。
- 字段按现有格式填：
  - CardFace：0 到 12（A 到 K）
  - CardSuit：0 到 3（梅方红黑）
  - Position：x/y 坐标

2. 确认加载链路没问题
- LevelConfigLoader 负责把 JSON 读进来。
- GameModelFromLevelGenerator 负责生成运行时 CardModel 并分配 cardId。

3. 最后做一遍可玩性自测
- 当前匹配规则是点数差 1（CardRuleService::canMatch）。
- 当前可点击判定在 CardController::canSelectPlayFieldCard（含遮挡判断）。
- 建议保证至少有一条可消除路径，不然容易开局死局。

### 2.2 如果是“特殊牌”
比如要加万能牌、锁牌、炸弹牌，就不只是改 JSON 了。

1. 先改数据模型
- 在 CardModel 增字段（比如 specialType、flags）。
- 如果支持存档，记得同步 toValueMap/fromValueMap。

2. 再改配置和加载
- LevelConfig 增对应字段。
- LevelConfigLoader 解析新字段。
- GameModelFromLevelGenerator 把字段映射到 CardModel。

3. 再改规则
- 在 CardRuleService 加新规则入口。
- 建议保留原 canMatch，再加 canMatchWithContext 之类的新接口。
- 尽量不要在 controller 里堆很多 if-else 规则分支。

4. 最后改表现
- CardResConfig 加资源映射。
- CardView::refreshVisual 根据新字段切换外观。

## 3. 怎么新增一种回退功能（Undo 类型）

### 3.1 先看现在的 Undo 是怎么做的
- UndoModel 存记录。
- UndoManager 负责 push/pop。
- GameController 在执行动作前写 UndoRecord，撤销时按记录反向恢复。

### 3.2 推荐扩展步骤（以“撤销洗牌”举例）
1. 新增回退类型
- 在 UndoActionType 里加一个枚举，比如 ShuffleStack。

2. 给 UndoRecord 加够用的数据
- 目标是“撤销时信息够还原”。
- 常见字段：
  - oldStackOrder（洗牌前顺序）
  - newStackOrder（可选，便于调试）
  - affectedCardIds（可选）

3. 在动作触发前写回退记录
- 先构造 UndoRecord。
- 写入 previousTopCardId、oldStackOrder 等关键状态。
- push 给 UndoManager。

4. 在 onUndoClicked 里加分支
- switch(record.actionType) 分开处理。
- ReplaceTopCard 走原逻辑。
- ShuffleStack 负责恢复 stackCardIds、topCardId 和相关位置层级。
- 恢复后统一做 layoutStackCards() + refreshViewFromModel()。

5. 动画要不要回放，按体验决定
- 如果希望视觉更完整，就在 GameView 补一个回退动画接口。

6. 如果支持中途存档
- 同步补 UndoModel/UndoRecord 的序列化字段。

### 3.3 给后续扩展留个模板
建议按“动作命令”的思路统一下来：
- 执行函数：ApplyXxxAction
- 记录函数：BuildXxxUndoRecord
- 回退函数：UndoXxxAction

这样 onUndoClicked 就不会越来越臃肿，后面也更容易抽成独立 UndoService。

