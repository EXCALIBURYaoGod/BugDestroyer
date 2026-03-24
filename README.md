<h1 align="center">BugDestroyer (多人团队射击游戏)</h1>

<p align="center">
  <img src="https://img.shields.io/badge/Unreal%20Engine-5.x-blue.svg" alt="UE5">
  <img src="https://img.shields.io/badge/Language-C++-orange.svg" alt="C++">
  <img src="https://img.shields.io/badge/Architecture-Client%2FServer-green.svg" alt="C/S">
</p>

## 📖 项目简介 | Introduction

**BugDestroyer** 是一个基于 Unreal Engine 5 (UE5) 和 C++ 开发的多人网络第一人称射击游戏（FPS）。
本项目采用了严谨的客户端/服务端（C/S）架构，核心玩法为红蓝阵营的团队死斗（TDM）。项目重点实现了服务端权威的战斗结算、基于历史帧的延迟补偿（Lag Compensation）、完整的比赛状态机流转，以及一套基于 CommonUI 的数据驱动前端框架。

---

## ✨ 核心技术特性 | Core Features

### ⚔️ 网络与战斗系统 (Networking & Combat)
- **服务端权威：** 所有核心逻辑（如伤害结算、射击判定、弹药扣除、武器切换）均在服务端执行或验证。
- **延迟补偿 (Server-Side Rewind)：** 自定义 `ULagCompensationComponent` 组件，服务端会记录玩家 HitBox 的历史变换帧 (FrameHistory)。开火时通过回溯对应延迟时间戳的包围盒位置，对 HitScan（射线类）和 Projectile（抛物线类）武器进行高精度的命中确认。
- **动态射击反馈：** - 实现了枪械的垂直/水平后坐力 (Recoil) 以及停止开火后的自动准心回复 (Recovery)。
  - UI 准星会根据玩家的移动速度 (Velocity) 和瞄准状态 (Aiming) 动态计算扩散系数。
- **状态增益系统：** 独立封装的 `UBuffComponent`，处理通过场景拾取物获取的生命值回复、护盾补充、移动速度强化及跳跃高度提升，并同步至相关运动组件。

### ⏱️ 比赛生命周期管理 (Match Flow & Team System)
- **状态机流转：** `ACommonGameMode` 管控了整个房间的生命周期，包含 `WaitingToStart` (热身倒计时)、`InProgress` (比赛进行中) 和 `Cooldown` (结算冷却并准备无缝漫游 ServerTravel)。
- **阵营与复活机制：** 玩家登入 (`PostLogin`) 时分配红蓝队伍，客户端依据队伍动态生成对应颜色的材质实例 (MID)；死亡后通过自定义 `ChoosePlayerStart` 清理缓存并安全复活。
- **玩家数据解耦：** 使用 `ACommonGamePlayerState` 持久化记录玩家的击杀数 (Kills)、死亡数 (Defeats) 和队伍标签，保证角色被销毁后数据不丢失，并在回合结束时计算 MVP。

### 📊 数据驱动 UI 与前端框架 (Data-Driven UI & Input)
- **异步 UI 栈：** 基于 `UBugUISubsystem` 和 CommonUI，实现界面的堆栈式管理，支持 UI 资产和贴图的异步加载 (`LoadSynchronous` 与软指针结合应用)。
- **设置数据注册表：** 编写了 `UOptionsDataRegistry` 集中管理游戏设置（音量、画质、视距、腰射/开镜灵敏度）。配置数据直接绑定至 `UGameUserSettings` 的子类并使用 `Config` 标签，实现自动持久化 (.ini 文件写入)。
- **动态按键绑定：** 结合 UE5 的 Enhanced Input 系统，提供运行时的键盘/鼠标与手柄按键侦听及重映射功能。

---

## 🏗️ 核心类与模块 | Architecture Overview

- **`ACommonGameMode` / `ATeamsGameMode`**
  - 服务端独占。控制游戏核心时序，处理伤害转换 (`CalculateActualDamage`)，拦截作弊，分发出生点。
- **`ACommonGameState`**
  - 全网同步。维护房间当前阶段时间 (Warmup/Match/Cooldown)，红蓝队伍比分，以及多播击杀广播 (KillFeed)。
- **`ABugCharacter`**
  - 处理物理移动、基于视角的射线检测目标 (`GetHitTarget`)、以及动画的融合（IK、AimOffsets）。死亡时包含网格体溶解材质的 Timeline 动画过渡。
- **`UCombatComponent`**
  - 角色战斗核心组件。处理主副武器的装备、切换、装弹、抛掷手雷以及弹药本地预表现。

---

## 🚀 快速开始 | Getting Started

### 开发环境
* **引擎版本：** Unreal Engine 5.x (建议具有 C++ 开发环境配置)
* **IDE：** Visual Studio 2022 (或 Rider)

### 编译与运行
1. 克隆项目仓库到本地计算机：
   ```bash
   git clone [https://github.com/EXCALIBURYaoGod/BugDestroyer.git](https://github.com/EXCALIBURYaoGod/BugDestroyer.git)
