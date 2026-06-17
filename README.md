# UE5 First Person Multiplayer Demo
UE5 开发的第一人称多人合作生存射击模板项目，具备完整网络同步、智能敌人AI、波次生存玩法，可直接作为多人FPS游戏开发的基础框架。

---

## ✨ 功能特性
| 模块 | 功能说明 |
|------|----------|
| 🎮 操控系统 | 完整第一人称操作：移动、跳跃、蹲伏、射击、装弹 |
| 🌐 网络同步 | 全状态跨客户端同步：角色属性、战斗、AI行为、游戏进度 |
| 🤖 智能AI | 敌人三状态机：巡逻→追逐→攻击，支持自动寻路与目标检测 |
| ⚔️ 战斗系统 | 射线检测射击、弹药管理、伤害判定、特效音效框架 |
| 🎯 生存玩法 | 波次递进机制：随波次增加敌人数量与难度 |
| 💀 生命系统 | 玩家死亡判定、自动重生、血量恢复框架 |
| 📊 流程管理 | 完整游戏生命周期：等待玩家→游戏中→游戏结束 |
| 🎨 UI框架 | 可扩展HUD：血量、弹药、分数实时显示 |

---

## 🚀 快速开始
### 环境要求
- Unreal Engine 5.0+（推荐 5.1 / 5.2 稳定版）
- Visual Studio 2022（C++ 开发环境，需安装游戏开发组件）
- Windows 10 / 11 64位系统

### 安装与编译
1. **克隆仓库**
```bash
git clone https://github.com/guozhiyang87-cyber/UE5-FirstPerson-Multiplayer-Demo.git
```

2. **生成项目文件**
   - 右键 `FirstPersonDemo.uproject`
   - 选择 `Generate Visual Studio project files`

3. **编译项目**
   - 打开生成的 `FirstPersonDemo.sln`
   - 选择 `Development Editor` 配置，编译解决方案

4. **打开项目**
   - 双击 `FirstPersonDemo.uproject` 启动UE编辑器

### 运行测试
#### 编辑器内多人测试
1. 打开关卡 `MainMap`（需自行创建基础关卡并放置玩家出生点）
2. 点击播放按钮旁的下拉箭头，选择 `多人游戏` 模式
3. 设置玩家数量（2人及以上），点击播放即可测试联机玩法

#### 打包独立运行
1. 编辑器菜单 → 编辑 → 项目设置 → 打包
2. 选择目标平台，点击 `打包项目`
3. 运行打包后的exe，先启动服务器，再启动客户端连接即可

---

## 📁 项目结构
```
Source/FirstPersonDemo/
├── Public/                          # 头文件目录
│   ├── Character/                   # 角色模块
│   │   ├── FirstPersonCharacter.h    # 玩家角色（移动、战斗、网络）
│   │   ├── EnemyCharacter.h          # 敌人角色（血量、状态、攻击）
│   │   └── EnemyAIController.h       # 敌人AI控制器（行为状态机）
│   ├── Game/                        # 游戏流程模块
│   │   ├── MultiplayerGameMode.h     # 游戏模式（波次、敌人生成）
│   │   ├── MultiplayerGameState.h    # 全局游戏状态
│   │   └── MultiplayerPlayerController.h # 玩家控制器
│   ├── Weapon/                      # 武器模块
│   │   └── Weapon.h                  # 基础武器类（射击、弹药）
│   └── Utilities/                   # 工具类目录
├── Private/                         # 实现文件目录
│   ├── Character/
│   ├── Game/
│   └── Weapon/
├── FirstPersonDemo.Build.cs         # 模块依赖配置
└── FirstPersonDemo.cpp              # 模块入口（日志定义）
```

---

## 🎯 核心系统详解
### 1. 输入系统（Enhanced Input）
基于UE5原生Enhanced Input框架实现，支持按键自定义映射：
| 默认按键 | 对应功能 |
|----------|----------|
| WASD | 前后左右移动 |
| 鼠标 | 视角旋转 |
| 鼠标左键 | 射击 |
| 空格 | 跳跃 |
| 左Ctrl | 蹲伏 |
| R | 装弹 |

### 2. 网络同步架构
采用UE标准Server-Client架构：
- 所有核心属性使用 `Replicated` 标记自动同步
- 关键操作（射击、受伤、死亡）使用 `Server RPC` 权威判定
- 视觉效果使用 `NetMulticast RPC` 全客户端同步
- 同步内容：角色位置、血量、弹药、分数、敌人状态、游戏波次

### 3. 敌人AI行为树
三状态自动切换状态机：
1. **巡逻状态**：随机生成巡逻点，自动寻路移动
2. **追逐状态**：检测到玩家进入范围，向玩家移动
3. **攻击状态**：进入攻击范围，停止移动并周期性攻击

### 4. 波次生存机制
- 初始波次生成3个敌人，每波递增2个敌人
- 波次间隔30秒，敌人逐个间隔生成
- 支持自定义出生点，敌人随机位置生成
- 所有玩家死亡触发游戏结束

---

## ⚙️ 参数配置
### 游戏全局配置（MultiplayerGameMode.h）
```cpp
int32 MaxPlayers = 8;                  // 最大支持玩家数
int32 InitialEnemyCount = 3;           // 第一波敌人数量
float EnemySpawnInterval = 3.0f;       // 单个敌人生成间隔
float WaveInterval = 30.0f;            // 波次间隔时间
float EnemyDetectionRadius = 2000.0f;  // 敌人玩家检测范围
float EnemyAttackRadius = 200.0f;      // 敌人攻击触发范围
```

### 武器参数配置（Weapon.h）
```cpp
float Damage = 25.0f;                  // 每发子弹伤害
float FireRate = 0.1f;                 // 射击间隔（秒）
float Range = 10000.0f;                // 射击最大距离
int32 AmmoPerMagazine = 30;            // 单弹匣弹药量
```

### 角色参数配置（FirstPersonCharacter.h）
```cpp
float MaxHealth = 100.0f;              // 玩家最大血量
float MaxWalkSpeed = 800.0f;           // 移动速度
float MaxWalkSpeedCrouched = 400.0f;   // 蹲伏移动速度
float RespawnDelay = 5.0f;             // 死亡后重生延迟
```

---

## 🛠️ 开发扩展指南
### 添加新武器类型
1. 创建C++类，继承 `AWeapon` 基类
2. 重写 `Fire` 函数实现自定义射击逻辑（如散射、蓄力等）
3. 在玩家角色蓝图中配置新武器类即可使用

### 添加新敌人类型
1. 创建C++类，继承 `AEnemyCharacter` 基类
2. 自定义血量、伤害、移动速度等属性
3. 在 `MultiplayerGameMode` 中添加新敌人的生成逻辑

### 添加自定义UI
1. 创建Widget蓝图，继承 `UUserWidget`
2. 在 `FirstPersonHUD` 中添加对应更新函数
3. 绑定角色属性变化事件，实现数据实时更新

---

## ❓ 常见问题
### 1. 编译报错 `LogFirstPersonDemo is undefined`
已在项目中修复，确保 `FirstPersonDemo.cpp` 中包含：
```cpp
DEFINE_LOG_CATEGORY(LogFirstPersonDemo);
```

### 2. 敌人不移动/原地打转
- 确保关卡中已构建导航网格：编辑器菜单 → Build → Build Navigation Mesh
- 确保出生点不在导航网格外

### 3. 多人模式按键无响应
- 确保 `BeginPlay` 中仅本地玩家注册Input Mapping Context
- 检查Input Action与Input Mapping Context资源是否正确赋值

### 4. 打包后无法联机
- 项目设置中开启 `Allow Peer to Peer Networking`
- 确保防火墙开放对应端口，或使用局域网连接

---

## 📄 许可证
MIT License - 可自由用于学习、个人项目与商业开发。
