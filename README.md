# Hazel
This is the learning game engine project Hazel Engine based on TheCherno's game engine project Hazel. YouTube channel: https://www.youtube.com/@TheCherno

## learn
### 什么是游戏引擎?
实际让人们创建**交互式场景**的工具.
交互式场景? 实时显示图形的应用, 并且可以交互, 游戏是其中的一种类型.
目标: **将一组数据转换成另一组数据**, 然后将其从屏幕中渲染出来.(纹理/模型->游戏引擎(量身定制)=> 提供交互)
读取文件: 也就是资产, 可以是外部输入的, 也可以是游戏引擎创建的文件.
为了这些转换会抽象成诸多功能:: 平台抽象层、渲染、音频处理、文件序列化......

### 初步设计游戏引擎
随着时间逐步优化迭代。从基础的框架开始。

1. 我们需要一个入口.
2. 应用层（生命周期和事件）
3. 窗口层（绘制和事件都源于此）
	- 输入
	- 事件(事件管理器, 广播系统->)
4. render渲染器（复杂/庞大）
5. render api 渲染抽象层（OpenGL、DirectX、Vulkan...）
6. debug - 日志系统/配置文件系统(随时查找各个系统的实际效率, 方便优化)
7. 脚本系统 - 使用更高级的语言编写脚本
8. 内存系统 - > 内存对性能至关重要
9. ECS 世界中创建对象的方法
10. 物理系统
11. 文件输入输出,VFS(虚拟文件系统)
12. 构建系统(外部文件格式->引擎自定义格式, 运行时可以不用关心格式转换)
	- 热重载机制

### 项目设置
Hazel引擎

1. 目标: Hazel引擎实际上是一个动态库，创建一个外部应用程序, 是exe.然后将此dll链接到应用程序中即可.（其他依赖的模块为静态库，附着于引擎dll。对外提供此dll即可）
2. 项目设置：
	- 配置:debug/release
	- 平台: x64
	- 生成文件夹: build/bin/config-platform/project_name/
	- 中间文件生成目录: build/bin/int/config-platform/project_name/
3. SandBox 利用游戏引擎的应用程序 -> exe

其他:
	1. 原项目使用的visual studio, 本项目是基于cmake+vscode尝试构建
