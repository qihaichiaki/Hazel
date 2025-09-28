# Hazel
This is the learning game engine project Hazel Engine based on TheCherno's game engine project Hazel. YouTube channel: https://www.youtube.com/@TheCherno

## learn
这里是每一步的学习文档, 跟随视频一步一步敲出来并且结合自己思路的文档:<a href="./doc/learnMain.md">learn</a>

### 项目整体思路梳理
* Hazel整体以dll的形式提供给外部, 外部只需要写好应用程序和相关的layer，实现创建application即可使用引擎。
* 引擎的运行: 包含文件``Hazel/Core/entry_point.h``, 内部存在main函数，可固定运行流程. sandbox中只需要提供createApplication即可运行

#### Application 
* 游戏引擎的应用程序基类, 其包含了引擎内各个组件的初始化以及相关的环境准备。
* 构造初始化:
   - Window::create()
   - 设置window事件回调

#### Window
* 游戏引擎的窗口基类, 负责创建窗口应用程序，并且监听窗口事件传回应用程序
* 存在平台适配, 下面只是具体实现了windows平台下的窗口示例
* 构造初始化(仅仅初始化一次)：
  - glfwInit()
  - 设置glfw的错误回调
  - 创建窗口句柄glfwCreateWindow(可优化到单独创建window里面)
  - 将窗口句柄传入OpenGlContext,OpenGlContext进行init(同上) 
  - 设置垂直同步setVSync(glfwSwapInterval(1)) (必须要设置吗?)
  - 往glfw注册用户数据, 后续设置窗口事件回调, 方便使用数据填充
  - 设置窗口事件回调, 具体为:窗口大小调整, 窗口关闭, 键盘按键/文本输入, 鼠标按键/滚轮/坐标  
* 更新函数