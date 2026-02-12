# BugDestroyer 项目说明

## 项目概览
- 引擎版本：Unreal Engine 5.7（从构建与日志推断）
- 项目类型：C++ + UMG/CommonUI 的 UI 驱动第三人称项目
- 插件：含 MultiplayerSessions（当前仅资源目录可见）
- 入口工程文件：[BugDestroyer.uproject](file:///e:/UE/BugDestroyer/BugDestroyer.uproject)

## 运行与构建
- 推荐直接用 UE5 打开 `BugDestroyer.uproject` 进行开发与运行
- C++ 开发可使用 VS/Rider，通过生成的解决方案 [BugDestroyer.sln](file:///e:/UE/BugDestroyer/BugDestroyer.sln)
- 目标与构建脚本：
  - [BugDestroyer.Target.cs](file:///e:/UE/BugDestroyer/Source/BugDestroyer.Target.cs)
  - [BugDestroyerEditor.Target.cs](file:///e:/UE/BugDestroyer/Source/BugDestroyerEditor.Target.cs)
  - [BugDestroyer.Build.cs](file:///e:/UE/BugDestroyer/Source/BugDestroyer/BugDestroyer.Build.cs)

## 目录结构
- Config：项目默认配置（Engine/Game/Input 等）
- Content：美术与蓝图资源（UI、Maps、Input、Materials、Textures 等）
- Source：C++ 源码
- Plugins：工程插件（当前可见 MultiplayerSessions/Resources）
- Binaries/Build/Intermediate/Saved：构建产物与临时文件（无需纳入版本控制）
- 顶层工程文件：`BugDestroyer.uproject`、`BugDestroyer.sln`

## 源码模块概览
- 核心入口
  - [BugDestroyer.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/BugDestroyer.cpp)
  - [BugDestroyer.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/BugDestroyer.h)
- 游戏标签
  - [BugGameplayTags.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/BugGameplayTags.h)
  - [BugGameplayTags.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Private/BugGameplayTags.cpp)
- 控制器
  - [BugPlayerController.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Controllers/BugPlayerController.h)
  - [BugPlayerController.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Private/BugPlayerController.cpp)
- 子系统
  - [BugUISubsystem.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Subsystems/BugUISubsystem.h)
  - [BugUISubsystem.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Private/Subsystems/BugUISubsystem.cpp)
  - [BugLoadingScreenSubsystem.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Subsystems/BugLoadingScreenSubsystem.h)
  - [BugLoadingScreenSubsystem.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Private/Subsystems/BugLoadingScreenSubsystem.cpp)
- 设置与开发者设置
  - [BugGameUserSettings.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Settings/BugGameUserSettings.h)
  - [BugGameUserSettings.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Private/Settings/BugGameUserSettings.cpp)
  - [BugDestoryerDeveloperSettings.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Settings/BugDestoryerDeveloperSettings.h)
  - [BugDestoryerDeveloperSettings.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Private/DeveloperSettings/BugDestoryerDeveloperSettings.cpp)
- UI 与组件
  - 核心激活/布局：[Widget_ActivatableBase.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Widget_ActivatableBase.h)、[Widget_PrimaryLayout.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Widget_PrimaryLayout.h)
  - 选项菜单与条目：
    - [Widget_OptionsScreen.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Widget_OptionsScreen.h)
    - [OptionsDataRegistry.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Options/OptionsDataRegistry.h)
    - [Widget_ListEntry_Base.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Options/ListEntries/Widget_ListEntry_Base.h)
  - 通用组件：
    - [BugCommonListView.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Components/BugCommonListView.h)
    - [BugTabListWidgetBase.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Components/BugTabListWidgetBase.h)
- 异步动作（UI 推送）
  - [AsyncAction_PushConfirmScreen.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/AsyncActions/AsyncAction_PushConfirmScreen.h)
  - [AsyncAction_PushSoftWidget.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/AsyncActions/AsyncAction_PushSoftWidget.h)
- 工具与类型
  - [BugUIFunctionLibrary.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/BugUIFunctionLibrary.h)
  - [BugEnumTypes.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/BugTypes/BugEnumTypes.h)
  - [BugStructTypes.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/BugTypes/BugStructTypes.h)

## 资源与关卡
- UI：`Content/UI/Widgets` 下包含主菜单、加载、确认、多人等界面
- 输入：`Content/UI/Input` 与 `Content/Input` 下有 IMC/IA 资产
- 地图：`Content/Maps`（如 `Lobby.umap`、`FrontendTestMap.umap`）
- 样例第三人称：`Content/ThirdPerson/Lvl_ThirdPerson.umap`

## 配置要点
- 全局默认配置位于 [Config](file:///e:/UE/BugDestroyer/Config)
- Saved/Temp 中可见打包配置示例（仅供参考），实际以工程设置为准

## 项目结构建议与命名规范
- 遵循 UE 约定：
  - `Source/<Module>/Public` 与 `Private` 分层清晰，当前已良好遵循
  - `Content` 下按功能域分文件夹（UI/Maps/Input/Assets 等）
- 命名一致性建议（后续逐步优化）：
  - `BugDestoryerDeveloperSettings` → 建议修正为 `BugDestroyerDeveloperSettings`
  - `BugLoadingScrennInterface` → 建议修正为 `BugLoadingScreenInterface`
  - `ListDataObjcet_Collection` → 建议修正为 `ListDataObject_Collection`
  - 以上为代码与文件名层面改进，涉及引用与 UCLASS/UINTERFACE 元数据，请在重命名前评估影响
- 插件与第三方：
  - `Plugins/MultiplayerSessions` 目前仅资源文件，建议补齐源码或移除无用目录
- 版本控制建议：
  - 忽略 `Binaries/Build/Intermediate/Saved/Cooked` 等构建输出与临时文件

## 打包与发布
- 通过 UE 编辑器的 Packaging 设置进行打包，当前使用 IOStore 与 Oodle 压缩（Saved/Temp 中可见）
- 测试包与产物位于 `Build/` 与 `Saved/StagedBuilds/`

## 快速链接
- 项目配置：[Config](file:///e:/UE/BugDestroyer/Config)
- 源码入口：[BugDestroyer.cpp](file:///e:/UE/BugDestroyer/Source/BugDestroyer/BugDestroyer.cpp)
- UI 子系统：[BugUISubsystem.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Subsystems/BugUISubsystem.h)
- 选项界面：[Widget_OptionsScreen.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Widget/Widget_OptionsScreen.h)
- 玩家控制器：[BugPlayerController.h](file:///e:/UE/BugDestroyer/Source/BugDestroyer/Public/Controllers/BugPlayerController.h)

