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

4. 其他:
	1. 原项目使用的visual studio, 本项目是基于cmake+vscode尝试构建

### 入口点
* 切入点：主函数的流程控制希望是从**引擎的角度**去提供的，而非应用程序。

* 项目文件: 
	1. core.h 定义一些宏的地方(比如导出导入宏)
	2. appliaction.h/.cpp 引擎层抽象应用程序类，让真正的应用程序继承自此类, 自定义东西, 但是应用程序的流程可以做到在引擎层的main中进行控制 
	3. 应用层sandbox定义sandboxApp.cpp 继承自Application

* 为了应用层使用引擎头文件更加的便利, 类似于只提供给客户端应用程序包含的头文件, 但是引擎Hazel永远不会包含这个内容
	- 问题: 会不会头文件膨胀呢?

* 由于是引擎提供入口点, 则需要在引擎的文件夹下提供entryPoint.h 头文件, 里面定义了main函数, 应用程序只需要引入即可(由此main函数控制application的生命周期, 并且利用唯一的createApplication相关外部声明函数得到唯一的application载入运行)
* 入口文件可能随着平台的不同而不同(利用宏控制)
* 返回应用程序指针, 可以将其函数在Hazel中声明，然后从其他.cpp文件里去实现一次即可.
  * 现在的实现似乎只是简单实现new - delete 这些需要用户心知肚明(先暂时简单的实现)

### 日志记录
* 记录事件的一种方式. 使用一种方式和使用它的人交流(引擎娘!?(警觉))
  * 重点是随着代码的复杂度增加, 需要日志系统来告诉使用者究竟发生了什么(编写复杂代码和自己交流的过程)
  * 来自哪里, 严重程度, 发生了什么

* 日志库使用现成的库即可(日志库重点集中于格式化打印)：spdlog

* 使用三方库的时候，如果其存在公开仓库内，那么使用git submodule 更好(随时控制更新代码....)

* 实际上可以直接使用spdlog。但是需要封装, 算是一种引擎日志，并且能够定义简单的宏进行使用，并且也是未来日后的方便更改日志系统。目标: 尽量减少破坏性。

* 项目文件:
  1. log.h/log.cpp: 封装spdlog, 目标能够简单的调用日志。
     * 两个logger->引擎+App
     * 自定义格式、日志等级等等

* 设置格式，消息如何打出: [spdlog日志格式](https://github.com/gabime/spdlog/wiki/Custom-formatting)
* 创建支持多线程logger(同步mt), 设置logger名字, 设置筛选级别

* 日志的初始化。暂时在入口点进行调用(后续应该会迁移到系统初始化函数当中去)
* 日志加入到暴露给客户端的头文件中去
* 封装宏，让调用日志更方便，并且能够**统一控制**诸如debug日志在发布版本中不写日志
* 使用宏的级别: trace, info, warn, error, fatal


### 使用构建系统(原为premake)
* 原本为premake, 这里用CMake替代.

* 构建系统的目的是为了适配不同平台编译出相同的可执行程序的


### 规划事件系统
* 因为要引入第一个系统->窗口系统. 那么此时事件处理就必不可少了

* Application: 包含一个循环, 保持整个系统运转, 并且持续更新我们的游戏
  *  作为事务的中心枢纽, 它需要能够接收事件，可以将他们最终派发到各个游戏层(layer)中去

* layer

* 以window事件为例: 放大、缩小、点击...... 这些在底层库中会存在事件发生(事件回调)。而整个事件我们需要传递给application.
* 但是，我们并不需要将application强行绑定到window类中，window通过事件调度派发事件。而事件的处理者的就是application相关层级, 所以application对于window来说是透明的.

* 需要一个事件系统。并且每个事件需要详细的信息.
* 应用程序创建窗口，应用程序此时就可以给窗口类型设置事件处理回调。这样每次窗口接收到对应的事件就会检查此回调是否存在并且执行相应的操作。
* 而执行的回调是application中设置的, 此时window就可以在不知道application的存在但是却调用了相关的处理方法, 随后再由application事件传播给layer使用.
* 需要注意的是事件均是**阻塞事件**. 因为会在栈上构建并且立即调用此函数, 处理此事件的时候会暂时暂停其他事件.

* 扩展方向: 事件队列, 延迟处理

* 对于事件传播layer的设想，layer相当于是一个堆栈, 每个layer都有机会处理到此事件，可以将事件向下传播到每一层, 一旦存在一个layer进行处理, 那么此事件就终止传播

* application -EventListener-> window  
* application <-OnEvent------- window  

* 事件处理传递的数据就是事件类型, 可以分为多种类型, 并且里面存储着当前事件发生的数据(比如鼠标的坐标......)
* 后续也可以作为应用程序事件, 执行一些业务功能(渲染事件......)

* 针对于OnEvent, 可能会存在事件分发器, 将不同的事件分发到比如OnMove... 类似的具体事件处理回调

### 事件系统设计
* 项目文件:
  * Hazel/Events:
    * event.h 
    * keyEvent.h
    * mouseEvent.h
    * applicationEvent.h

* 需要一些预编译头来优化整体编译性能

* 事件系统的当前设计是阻塞的, 事件并不是被延迟的, 会立即相应. 未来扩展可以放入某种事件总线中去.

* 存在一个enum class来描述事件类型(EventType). 当前暂时可以分为下面几类:
  * None = 0 保留字段
  * Window: Close, Resize, Focus, LostFocus, Moved
  * App: Tick, Update, Render
  * Key: Pressed, Released
  * Mouse: ButtonPressed, ButtonReleased, Moved, Scrolled 
* 针对上述的事件类型, 可以使用一个事件类别进行分门别类(enum EventCategory), 这里设计使用了位图, 应该是为了方便筛选事件的类别.
  * None = 0
  * Application
  * Input
  * Keyboard
  * Mouse
  * MouseButton

* 定义了EVENT_CLASS_TYPE宏，根据传入的type，方便每个事件实现类实现
  * GetStaticType() 静态函数，返回具体的事件类型 (不需要事件实例就可以获取事件类型)
  * GetEventType() 子类重写函数, 里面调用了此静态函数, 并且设计为了可继承 (手中持有的事件对象是一个基类)
  * GetName() 子类重写函数 返回const char* 事件名字, 写死了, 没有字符串开销

* 定义了EVENT_CLASS_CATEGORY宏, 根据传入的category(类别):
  * GetCategoryFlags 子类重写函数, 返回当前事件类别

* Event 声明事件基类:
  * 声明纯虚函数: GetEventType, GetName(可能并不是在所有的配置下均需要使用?), GetCategoryFlags
  * 实现虚函数: ToString, 返回的是string对象，里面调用GetName(). 可以重写此函数来表示更多的事件细节(也只是提供调试, 所以对性能方面没有关注)(后面对Event实现了ostream流重载, 调用了此函数)
  * 提供筛选事件类别接口: IsInCategory()
  * 存在保护字段: handled, 判断此事件是否处理

* 具体的相关事件:
  * 按键事件
    * KeyEvent, 按键事件(按下/释放时的按键代码 共同的数据)
      * 键事件基类, 成员为int类型的keycode, 构造函数是被保护的, 可以设置keycode(因为一个按键事件被确定，keycode一定时固定不可变的), 事件类别时keyboard和input
    * 释放事件不需要考虑其他的
    * 按下事件可能需要考虑是否重复按压(多维护了一个int repeatCount字段来表示当前key是否时重复按下(0为第一次按, 1为重复按下))
  * 鼠标事件(鼠标和输入类别)
    * MoveEvent, 存储数据字段mouseX, mouseY
    * ScrolledEvent, 存储数据字段xOffset, yoffset
    * 鼠标按键基类(和按键事件类似, 但是鼠标按下似乎没有检测重复)
    * 鼠标按键按下/释放
  * 应用程序事件(包含App和Window事件, 应用事件类别)
    * windowClose
    * windowResize: unsigned int width和height
    * 不确定的tick, update, render(它们可以被硬编码, 因为这是既定的事实，而不是可能的模块)

* EventDispatcher 声明事件调度器(不对外暴露)
  *  using EventFn, 使用模板参数来表示对应的事件类型(可以扩展对模板的检查), 使用function包装, 函数处理是bool(EventT&)
  *  构造函数传递的就是Event& event, 调度器内部存储Event&作为成员
  *  dispatch的时候, 就可以根据传入的EventFn进行调度. 
     *  细节: 通过模板EventT, 可以调用事件具体子类的静态函数, 来判断Dispatcher中存储的事件引用类型是否一致(多态函数type调用)
     *  事件类型一致, 函数回调(调用的时候需要将事件引用类型转换为对应的事件模板类型传递)的返回值作为此事件是否处理.并且返回true表示事件调度成功
     *  否则调度失败返回false
  
### 增加预编译头文件
* 项目文件:
  * hzpch.h
  * hzpch.cpp(vs里需要这样支持)

* 增加的预编译头文件:
  * iostream
  * memory
  * utility
  * algorithm
  * functional
  * 下面是容器
  * sstream
  * string
  * vector
  * unordered_map
  * unordered_set
  * Windows特有:Windows.h

* 随后用构建系统指定预编译头
  * CMAKE里: ``target_precompile_headers(MyApp PRIVATE "pch.h")``

* 需要注意pch文件是每个源文件内的头个头文件

### 窗口抽象和GLFW
* 从添加一个窗口开始, 可以提交渲染的任务, 可以可视化一切东西, 开始游戏引擎开发的一切......

* 希望窗口能够支持多平台, 那么就得对窗口类进行抽象

* 使用GLFW简单的跨平台库创建窗口
  * 前期为了简单适配渲染可以这样做. 比如渲染只有OpenGL. 但是如果后期支持DirectX, 则就需要在windows下调用win32的原始窗口API了
  * 虽然GLFW是跨平台的, 但是注意抽象可以做到此平台上的GLFW(直接修改等......), 不同平台是可以做出不同的东西的


* 抽象window类
  * struct WindowProps
    * string Title
    * unsigned int Width
    * unsigned int Height
    * Hazel Engine 1280 720
  * class HAZEL_API Window
    * using EventCallBackFn void(Event&)
    * virtual ~Window() {}
    * virtual onUpdate() = 0;
    * virtual Width/Height Get = 0
    * window -> attributes
      * virtual SetEventCallBack = 0
      * virtual SetVSync(bool) = 0
      * virtual bool IsVSync() const = 0
    * static Window* Create(....);  窗口唯一的创建接口实现(每个平台实现, 取决于当前编译的平台)

* src/PlatForm/Windows
  * WindowsWindow.h/.cpp 的平台具体实现
    * 接口重写
    * 私有方法: Init(WindowProps&), Shutdown
    * 私有属性: 
      * GLFWwindow*
      * struct WindowData(WindowProps的运行时数据 + VSync, EventCallBackFn 相关字段)
        * 抽象此字段的目的时可以将其传递给GLFW作为用户自定义数据, 方便修改
      * WindowData windowData
  * 相关实现：
    * Init
      * data字段赋值
      * 可能存在多个窗口创建的需求, 存在.cpp里静态字段, 防止glfw初始化过多 **glfwInit()**
        * 为了检查初始化是否成功, 引入日志断言: HZ_CORE_ASSERT(c) -> core.h
          * if (!c) { HZ_ERROR("断言失败: {0}", __VA_ARGS__); __debugbreak();}
          * 使用宏HZ_ENABLE_ASSERTS控制上述的行为
          * __debugbreak 适用于windows, 相当于调试的断点
        * glfwCreateWindow 创建窗口, 后面两个字段置空
        * glfwMakeContextCurrent(window) 将上下文设置为当前
        * glfwSetWindowUserPointer(window, &m_data); 设置用户自定义数据(方便后续窗口事件触发, 调用用户数据里的事件回调将事件广播出去)
        * SetVSync(true)
          * glfwSwapInterval(1) 垂直同步, 即窗口的缓冲区交换是否和显示器的刷新同步(需要激活当前上下文才能开启)
    * shutDown
      * glfwDestoryWindow(window) 释放窗口
    * onUpdate
      * glfwPollEvents();  glfw轮询事件
      * glfwSwapBuffers(window); glfw交换缓冲区 -> 双缓冲
  * 在application中添加此window的唯一指针
    * 在应用程序构造函数中, 调用Window::Create函数进行构建
    * 增加running控制循环, 持续更新window的更新
    * 可以在循环中添加glClearColor(1, 0, 1, 1) 设置清屏颜色, glClear(GL_COLOR_BUFFER_BIT)清除颜色缓冲区, 用设定的clear颜色 测试opengl上下文 

* pch中增加日志头文件

### 窗口事件
* 将window中产生的event抛出，给应用程序进行消费处理
* 完成 Key, Mouse, Window 事件处理
* 实际上，window时将事件扔给了上层给window塞入EventCallback的那个处理。

* 在Application中设置函数OnEvent，将其绑定传递给window，window发生事件会传递到此处, 然后又由此处进行派发调度。

* 使用**glfwGetWindowUserPointyer**取出对window存储的用户字段
* GLFW中设置glfw事件回调
  * glfwSetWindowSizeCallback(window, void(window, width, height))
  * glfwSetWindowCloseCallback(window, void(window))
  * glfwSetKeyCallback(window, void(window, key, scancode, action, mods))  // 需要存在自己的keycode, 不依赖于glfw的keycode
    * action: GLFW_PRESS,GLFW_RELEASE,GLFW_REPEAT 
  * glfwSetMouseButtonCallback(window, void(window, action, mods))
    * GLFW_PRESS, GLFW_RELEASE
  * glfwSetScrollCallback(window, void(window, double xOffset, yOffset))
  * glfwSetCursorPosCallback(window, void(window, double xPos, yPos))

* 另外的，可以在初始化时, 想glfw塞入一个错误处理回调: glfwSetErrorCallback void(error, const char description)

* 在application的OnEvent函数中实际利用事件调度器去处理事件
  * 根据传入的函数指针里的事件类型进行匹配