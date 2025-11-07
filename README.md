# FastMiner

一个基于 Levilamina 的快速连锁采集 Mod(Plugin)。

## 🚀 性能测试

> 测试环境均为: Windows 10 22H2 + 同时运行客户端和服务端  
> 配置: i5-6300HQ @2.9Ghz + D4-8G-2133

| 数量 | 耗时    | 平均         |
| ---- | ------- | ------------ |
| 5    | 0ms     | /            |
| 33   | 3ms     | /            |
| 67   | 8ms     | /            |
| 1k   | 117ms   | 0.117ms/个   |
| 1w   | 1261ms  | 0.1261ms/个  |
| 10w  | 13592ms | 0.13592ms/个 |

> 🧩 2025.11.7 更新：重构后的调度系统性能测试 **100096** 个方块, 总耗时 **10606ms**, 平均 **0.10595ms/个**, 理论提升 22%🚀。

## 安装

```bash
lip install github.com/engsr6982/fastminer
```

> Tip：初次使用，需输入命令 /fm 打开 GUI，开启需要连锁的方块。

## 命令

- /fm 打开设置 GUI
- /fm off 关闭连锁采集
- /fm on 开启连锁采集
- /fm manager 方块管理 GUI（OP）

## 配置文件

```json
{
  "version": 6,
  "dispatcher": {
    "globalBlockLimitPerTick": 256, // 每次调度最多采集多少个方块
    "maxResumeTasksPerTick": 16 // 每次调度最多恢复多少个任务
  },
  "moneys": {
    "Enable": false, // 是否启用经济系统
    "MoneyType": "LLMoney", // 经济类型 LLMoney 或 ScoreBoard
    "MoneyName": "money", // 经济名称
    "ScoreName": "" // 计分板名称
  },
  "blocks": {
    // 方块配置，键(Key) 填写方块命名空间
    "minecraft:acacia_log": {
      "name": "金合欢木原木", // 方块名称
      "cost": 0, // 每个方块消耗的经济
      "limit": 256, // 最大连锁采集数量
      "destroyMod": "Cube", // 破坏模式，支持: Default 和 Cube。 Default搜索相邻的6个面，Cube 3x3x3搜索
      "silkTouschMod": "Unlimited", // 精准采集附魔，Unlimited 无限制、Forbid 禁止精准附魔、Need 需要精准附魔
      "tools": [
        "minecraft:wooden_axe", // 工具限制，如果留空数组，则代表不限制工具类型
        "minecraft:stone_axe",
        "minecraft:iron_axe",
        "minecraft:diamond_axe",
        "minecraft:golden_axe",
        "minecraft:netherite_axe"
      ],
      "similarBlock": [] // 相似方块，填写方块命名空间，连锁时会一起采集
    }
  }
}
```

![image](./readmeimg.png)
