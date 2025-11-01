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
	1. base.h 定义一些宏的地方(比如导出导入宏)
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
        * 为了检查初始化是否成功, 引入日志断言: HZ_CORE_ASSERT(c) -> base.h
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


### layer层级支持
* 层决定了绘制顺序, 也适用于事件, 更新逻辑
* 层栈是存在顺序的, 可以据此决定渲染的先后。
* 需要注意的是层级接收事件的顺序和层级渲染的顺序是刚好相反的。层级越晚渲染，则其画面越在上面。而事件触发往往从上往底的去触发(以鼠标点击为例)
* 对于层栈的处理, 渲染的时候, 从前往后遍历(栈帧是从下往上), 越后的越被渲染, 其画面呈现优先度就越高; 事件处理的时候是从后往前遍历(栈帧是从上往下), 事件越应该被画面优先级越高的进行处理.

* 层是一个抽象的概念, 其是应用程序的一部分, 用来呈现某些类型的内容并接收事件。 
* 举例: GUI层, UI层, 调试层, 游戏层

* 项目文件:
  * Hazel/
    * Layer.h/.cpp
    * LayerStack.h/.cpp

* HAZEL_API layer抽象:
  * 构造函数赋值name(默认为Layer)
  * 保护字段: debugName 层级调试的name (在后续的发布版本中, 不应该以debugname去索引Layer)
  * 虚函数:
    * onAttach() 添加到LayerStash的时候被调用
    * onDetach() 被LayerStash移除的时候被调用
    * onUpdate() layer更新
    * onEvent(Event& event) layer响应事件
  * getName() const 返回debug调试的layer name 
  * 后续扩展, 层是否启用(控制层是否参与update和renderer)

* HAZEL_API LayerStack实现:
  * 首先Layer* 存在vector中
  * 里面保存其iterator 的 layer_insert 迭代器(== layers.begin(), 用来直接插入到栈顶, 常规的入栈操作)
  * 向外提供容器的begin, end迭代器
  * 实用功能:
    * PushLayer(Layer* ) 使用layer_insert插入到栈顶
    * PushOverlay(Layer* overlay) 插入到栈尾(画面最后渲染, 事件接收最前)
    * PopLayer(Layer*) -> 这里存在疑问! layer_insert何为--?
    * PopOverlay(Layer*) -> 同理
  * LayerStack是由Application所有, 所有权应该在Application中
  * layer的目的是在给定的时间点上贯穿application的整个生命周期
    * 当前的所有权语义: 存在vector当中的，只有析构的时候才会delete释放layer资源
    * 而pop的时候仅仅只是移除layer, 并不会对layer进行摧毁

* Application增加此LayerStack, 并且增加push layer
  * OnEvent: 事件传播从底向上进行遍历, 并且依次调用其layer的onEvent函数, 并且判断e的handled是否处理, 处理就break; 不会对上面的层级进行调用。
  * Onupdate: 对层级从上到下进行遍历(常规遍历即可), 调用里面的update方法即可

* 多线程调试版本
* Hazel.h 公共头文件暴露layer头文件
* SandboxApp中创建一个示例的layer, 插入到SandBoxApp中去
  * 下一步计划， 创建一个Imgui layer


### 现代OpenGL和Glad
* 需要一种将所有现代OpenGL函数从GPU加载到C++中的方式
* glad: 基于官方规范的多语言gl加载生成器(比glew好一些，更现代一点?)
* glad的网站配置下载: [glad](https://glad.dav1d.de/)
  * profile -> core
  * gl -> core 4.6
* 将其源码加入hazel/vendor
  *  将其变为C静态库进行链接即可

* glad库核心:
  * 调用gladLoadGLLoader(GLADloadproc), 使用拥有的所有的加载类函数
  * 在glfw初始化opengl当前上下文后就可以调用此api进行工作
  * 参数给予的是glfwGetProcAddress, 需要转换下函数类型

* 另外，因为存在多个可以引入gl的库, 针对glfw, 可以定义宏来控制glfw不引入gl头文件, 防止和glad之间存在冲突
  * GLFW_INCLUDE_NONE

* 加载完毕后，使用glGenVertexArrays简单测试下id是否有效

### 引入imgui
* 目标, imgui后续将使用的是Hazel提供的通用渲染接口, 来在窗口中绘制一些东西出来
* 同样的, imgui也作为submodule引入到hazel/vendor/imgui
  * 编译为静态库引入(没有选择后端)

* 项目文件：
  * hazel/ImGui: imgui_layer.h/.cpp

* 针对于imgui搭建layer对象
  * 针对于imgui的后端实现文件, impl_opengl3.cpp 放在PlatForm/OpenGL中实现
    * USE_GL_ES3 -> 定义删除
    * 命名风格变为: imgui_opengl_renderer
    * set default opengl loader to be gl3w -> 删除? 
    * 需要注意包含hzpch.h


* 编写imgui layer层:(可以尝试参考: glfw+opengl3的相关示例代码)
  * onAttach()
    * ImGui::CreateContext
    * ImGui::StyleColorDark
    * GuiIo GetIO()
      * io.BackedFlags |= ImGuiBackendFlags_HasMouseCursors;
      * io.BackedFlags |= ImGuiBackendFlags_HasSetMousePos;
      * 在glfw的后端实现中, 137-> keyboard mapping (临时实现-轮询输入)
    * ImGui_ImplOpenGL3_Init("#version 410");  // 初始化OpenGL
  * onUpdate()
    * io.DisplaySize = ImVec2{Get().GetWindow().GetWidth(), Get().GetWindow().GetHeight()};// 注意增加application初始化时添加application实例的静态指针(单例, 注意断言一下), 然后返回Application& 和 Window& 对象方法  创建新帧前需要设置窗口大小
    * glfwGetTime() 可以获取delta, 设置到io.DeltaTime(里面time- m_time(上次记录时间戳))
    * ImGui_ImplOpenGL3_NewFrame()
    * ImGui::NewFrame()
    * 临时键入static bool show, 里面增加ImGui::ShowDemoWindow()的演示窗口
    * ImGui::Render();
    * ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


* hazel.h 中增加imgui/imgui_layer头文件, 方便sandbox添加imgui层
  * 需要最后渲染, 事件第一个处理, 所以pushOverlay

* application pushlayer的时候需要调用onAttach()


* 简单总结
> 1. 首先引入的imgui是一个后端无关的lib, 只是提供UI层面的逻辑调用
> 2. 所谓的后端没实现, 则是指需要后端相关函数, 为imgui实现接收窗口事件, io相关设置
> 3. 最后想要完整的实现窗口+渲染, 也需要后端窗口+渲染的实现
>   具体表现为, 开始新帧阶段, glfw的实现里是设置窗口相关的信息, 以及按键映射等等, 而opengl则是准备渲染数据
>   当前项目主要是引用了imgui的opengl的渲染后端实现, 而窗口后端则是自己添加上去的.

* 注意: imgui新版本已经抛弃历史io.KeyMap和io.KeysDown那一套, 需要使用新的AddXXXEvent进行解决

### imgui事件
* 首先根据imgui的事件类型, 整理出全部的onXXXEvent函数, 方便在OnEvent函数中进行处理
* onXXXEvent整理如下:
  * 鼠标响应
  * OnMouseButtonPressedEvent()
    * io.MouseDown[button] = true
    * 当前版本: AddMouseButtonEvent(button, true);
  * OnMouseButtonReleasedEvent()
    * io.MouseDown[button] = false
    * 当前版本: AddMouseButtonEvent(button, false);
  * OnMouseMovedEvent()
    * io.MousePos = ImVec2
    * 当前版本: io.AddMousePosEvent
  * OnMouseScrolledEvent()
    * io.MouseWheel += x_offset
    * io.MouseWheelH += y_offset
    * 当前版本: io.AddMouseWheelEvent
  * 按键响应
  * OnKeyPressedEvent()
    * keysDown = true
    * 另外需要对修改器键单独处理
      * KeyCtrl
      * KeyShift
      * KeyAlt
      * KeySuper
    * 当前版本: io.AddKeyEvent, 似乎不需要特殊处理修改器按钮?
  * OnKeyReleasedEvent()
    * KeysDown = false
  * OnKeyTypedEvent()-> glfw_CharCallback
    * KeyTypedEvent
    * 用于在文本框输入类似的东西(io.AddInputCharacter)
    * 在之前的windowswindow中, 需要向glfw的glfwSetCharCallback注入事件处理回调
    * 当前版本: io.AddInputCharacter
  * 窗口响应
  * OnWindowResizedEvent()
    * DisplayerSize
    * DisplayerFramebufferScale = ImVec2{1.0f, 1.0f}
    * glViewport(0, 0, w, h) 调整渲染视口

* 一个事件进入了层的事件处理, 通过事件调度器, 方便的将事件派发到对应的事件处理函数上消费
* 可以将塞入函数回调使用绑定的整理为一个宏, 放入base.h的文件中去
  * HZ_BIND_EVENT_FIN(fn)

* 当前的事件消费处理都暂时设置为false, 即也要传递给下一个layer进行消费事件
* 后续计划:
  * Input可以轮询查询, 而不是通过Event的方式
  * keycode后续替换为hazel自身的

* 遇到问题:
  1. input字符移动了两次(按键按下事件被处理了两次(释放事件被误写成了按下事件))
  2. 控制键无法使用(当前解决方案使用IsKeyDown也可)
  3. 尝试支持中文字符输入(支持, 但是输入法暂时没有解决)

### 轮询输入
* 能够实现在程序的任何询问, 这个键是否按下了
* 项目文件:
  * Hazel/input.h

* input应该是全局的, 静态的

* input相关设计
  * Input设置为静态示例
  * 静态接口比如 IsKeyPressed(int keycode)， 调用静态示例中的虚函数方法(保护), IsKeyPressedImpl(int keycode) = 0; 即可
  * 同样的, 在适配平套的Windows下创建windows_input.h/.cpp实现windows下的具体input全局实例
  * 当前和GLFW集成即可。对于实现可集成/可移植/可扩展的系统, 比如WindowsWindow中的GLFW的window需要暴露出来, 需要设计一个比较强大的接口进行暴露
    * Window类中声明纯虚函数GetNativeWindow const = 0, 返回实际底层实现窗口句柄void*
    * 通过app->window->窗口句柄指针->glfw调用 glfwGetKey(glfwwindow, keycode);
    * return state == GLFW_PRESS || GLFW_REPEAT;
  *  同理: IsMouseButtonPressed(int button); GetMouseX/Y GetMousePosition(pair float-float)
  *  创建静态实例: 暂时直接在WindowInput下直接new即可
*  由于当前引擎没有设置按钮枚举, 所以只能暂时包含GLFW相关头文件使用key枚举才能使用


### 自定义按键代码
* 之前的输入/事件绑定均有点太依赖GLFW库, 这对未来可扩展(不使用GLFW库)不友好, 需要引擎层自己包装一层按键代码, 来屏蔽掉不同实现的差异
* hazel.h 增加Input.h

* 参考一种库的实现按钮映射方式, 在其他环境下做静态/动态时的转换即可
* 当前参考glfw，在Hazel/keyCodes.h 中将对应的按键宏拷贝过来， mouseButtonCode.h 类似  -> input.h
* 添加完毕后入客户端头文件中即可

* 转换:
  * 使用宏控制, 在不同平台是对应的那一套平台的按键宏定义(存在问题: 序列化怎么办?)
  * 使用一个函数->查找  表进行转换

### 渲染准备-数学库
* 处理一个潜在的2d/3d渲染器, 需要2维/3维/4向量, 变换矩阵......
* 数学库想要运行的更快, 实际上是要利用SSE/SIMD(单指令多数据)
  * 编译器内部函数 - 汇编程序.....
* 使用[glm](https://github.com/g-truc/glm), 是opengl的数学库. 使用submodule添加它

### IMGUI停靠和窗口
* 在原本的ImGui模块里, 添加一个ImGuiBuild.cpp 包含需要的build的.cpp后端(opengl3 + glfw)文件

* imgui_layer文件对的修改:
  * OnEvent全部取消, 由后端glfw直接支持
  * 将OnUpdate->OnImGuiRender
  * 原本OpenGL中的平台适配文件需要删除
  * onAttach 
    * configFlags: NavEnableKeyboard | DockingEnable | ViewportsEnable
    * get Style  configFlags & ViewportsEnable
      * windowRounding = 0.0f
      * Colors[ImGuiCol_Windowbg].w = 1.0f
    * init glfw
    * init opengl
  * onDetach
    * opengl/glfw shutdown
    * DestrtoryContext
  * Begin()  // 创建窗口
    * opengl/glfw new frame
    * ImGui::newFrame
  * End()
    * 更新io中的display size
    * imgui::Render
    * opengl 绘制数据
    * configFlags & ViewportsEnable
      * backup_current_context = glfwGEtCurrentContext()
      * imgui::updatePlatformwindows()
      * imgui::renderPlatFormwindowsdefault()
      * glfwMakeContextCurrent(backup_current_context)
  * OnImGuiRender() ovrride 此函数是给每一个layer都可以做任何的imgui渲染
    * 比如showDemo

* 这样写没问题, 因为最终是为每一个平台配置一份imguilayer
* Application:
  * 现在不希望imguilayer这一层被游戏/客户端所控制, 直接写在和window同地位的属性上，所有权再layerstack上
  * 然后在run的时候, 每个layer update后, 再根据imgui的begin、end中间增加每个layer的imguiRender即可

* LayerStackbug修复
  * 将迭代器换成unsig int , 来控制PushLayer的时候, 总是插到非overlay的layers的最后一个
  * push ++, pop -- layerInsertIndex
  * 目的是保证插到覆盖层之前

* 问题：
  * imgui作为静态库链接到了Hazel这个动态库, 符号都在里面，为什么现在直接暴露imgui的头文件给sandbox, sandbox无法直接用? 会link错误?(需要增加导出符号?)


### 渲染器主题
[开始一个渲染器](./render/render_head.md)


### TimeStep和增量时间
* 希望移动的时候是基于时间的, 而不会收到帧率的影响

* 如果更新的时候,能够具体知道当前帧间隔, 依据此帧间隔进行移动, 就不会出现上述的问题

* 文件:
 * Core/Timestep.h/.cpp

* class Timestep(对float包装为时间间隔的东西)
  * 属性:
    * float time
  * 方法
    * 构造函数: time 初始化0.0f
    * getSeconds const
    * getMilliseconds const
    * operator float() const

* app类包含float last_frame_time
  * 获取当前时间->可以使用标准库的函数获取时间
  * layer onupdate强制传入TimeStep


### Ref&Scope&智能指针
* 使用智能指针的目的:
  * 控制其生命周期
  * 引用结束自动释放资源

* render流程: 1. 延迟渲染 2. 多线程-渲染线程 3. 渲染器同时处理和渲染前一帧的数据
* 对于资产对象(shader......)，渲染线程实际上对这些对象存在强引用
* 封装共享指针:Ref和唯一指针:Scope
  * using 一下, 配合上template -> base.h

* 未来是资产的类型, 标记用上述类型替换(Buffer... shader.....)


### 文件整理
* 将除开相机控制器的其他文件转移到Core文件夹下
* 相机控制器暂时放在renderer下面


### 后续引擎规划
* 使用脚本来实现游戏的逻辑代码(lua, c#), 和引擎C++区分开
* 使用批量渲染, 让引擎快速正确的渲染2D图形(Renderer2D -> 批处理渲染器)
* 场景, ECS, 数据序列化, 用户UI, 脚本系统......


### 编辑器主题
* 设置停靠空间, 将所有显示组件挡住一块可以拖动的区域
* 需要将渲染内容渲染到一张纹理上, 然后将纹理绘制到一个窗口上

* [帧缓冲区](./render/render_frame_buffers.md)
* 新建HazelEditor项目
  * HazelEditorApp.cpp
  * EditorLayer.cpp/.h
  * HazelEditor 也存在于Hazel命名空间内

* 创建application时默认给予一个名字, 方便给主窗口进行命名?

#### Imgui面板-场景视口
* ImGui::PushStyleVar(IMGUIStyleVar_WindowPadding, InVec2{0, 0});  // 设置窗口无边框
* Imgui::Begin("Viewport")
* Imvec2 viewportPanelSize = GetContentRegionAvail();  // 获取当前视口的长和宽, 会动态的获取当前视口的信息
* m_viewport_size = viewportPanelSize;
* 如果和之前的大小不一样, 需要调整帧缓冲区的大小->帧缓冲区重新执行invalidate
* frameBuffer::resize(uint32_t width, uint32_t height)
  * m_specification.w / h xxx
  * invalidate();
    * 如果当前不是第一次，则删除之前的帧缓冲区和两个纹理，重新执行
* 相机控制器重新设置投影矩阵(因为此时视图的大小更新传播不了事件.)
  * CameraController::onResize(float width, float height);
  * .... resize
* //...
* ImGui::PopStyleVar();


* 另外，在帧缓冲区绑定的时候，gl设置一下视口大小: glViewport(0, 0, width, height);


* 现在存在问题: 不同视口上的事件会传播到别处, 存在影响

### 后续代码审查和方向
#### input
- 因为input是无法自己选择的, 是根据当前软件使用的平台抉择, 非运行时而编译期, 所以不需要设置虚函数进行实现兼容
- 将虚函数实现全部删除
- 直接在windowsinput后端实现所有函数, 利用宏控制是否包含当前实现
- 未来实现多平台使用宏包裹实现代码即可

#### 修正GUI检测事件输入问题
* 解决要点: 如果当前活动或者聚焦的是视口就继续传递事件
* 在当前的imgui窗口中, 检测是否被聚集和悬停: ImGui::IsWindowFocused(),ImGui::IsWindowHovered()
* 可以简单利用此bool来控制相机控制器是否更新
* 在ImGuiLayer中增加m_blockEvents变量控制是否阻止imGui事件的传播, application可以获取, 然后外部调用如果视口不处于焦点就阻止其将事件传播。
  * imguilayer::onEvent
  * if (m_blockEvents)
  * ImGui::GetIo();
  * e.Handled |= e.isInCategory(EventCategoryMouse) & io.WantCaptureMoouse;
  * e.Handled |= e.isInCategory(EventCategoryKeyBoard) & io.WantCaptureKeyboard;  


#### 帧缓冲区接收正确的大小调整
* Framebuffer::resize(w, h) 如果宽和高任意一个为0，都不应该进行调整大小
* 或者大于一个特定的值: s_MaxFramebufferSize = 8192;
* 在editor layer端, 需要检查下传入的size是否全都大于零，可以避免相机设置错误的投影值

#### 其他
* 对于平台相关的宏, 更希望能够在外界(编译器层)进行定义包含，而不是在base中, 防止一些一地方因为base不存在而丢失
* input_codes.h 需要将原本的宏转换为枚举变量enum class KeyCode: uint16_t
* 除了opengl，其他图形渲染api没有顶点数组对象这个概念。


### 实体组件系统
* 使用第三方ECS库: entt
* 实体组件系统对性能至关重要
* Scene容器 -> gameobject对象 entity -> 动作/行为 -> 分类system (缓存命中)

#### entt的引入
* 首先将<a href="https://github.com/skypjack/entt">entt</a>中的entt.hpp包含入vendor\entt\include
* 将其库中的MIT 许可证也一并下载到对应的仓库
* cmake中，hazel core/editor部分将其包含

* 创建Scene文件夹, 后续ECS相关的部分均会在此文件夹下输出

#### scene.h/.cpp
* class Scene
  * public:
  * Scene();
    * // 未来可以为添加到实体的相机组件设置事件回调
    * // m_registry.on_construct<CameraComponent>().connect<>
  * ~Scene();
  * Entity createEntity(const std::string& name = std::string{});
    * entity = {m_registry.create(), this};
    * entity.AddComponent<TransformComponent>();  // 每个创建出来的entity对象默认增加transform组件
    * tag = entity.AddComponent<TagComponent>();  // 每个创建出来的entity对象默认增加tag组件
      * tag = name.empty() ? "新实体" : name;
    * return entity;
  * DestroyEntity(Entity entity) {
    * m_registry.destroy(entity);
  * }
  * onUpdate(Timestep ts);  // 更新和渲染提交均在此处
    * 找到所有原生脚本组件, 进行执行操作
      * _registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc) {
      * 如果脚本对象没有创建执行创建，创建完毕赋予entity和当前的场景指针this -> entity对象 - > 延迟创建初始化, 等待的就是场景所有加载完毕再加载脚本对象(ToDo move to 场景play)
      * 执行update方法
      * });
    * 尝试通过m_registry获取当前场景的相机组件 { }
      * auto view = m_registry.view<TransformComponent, CameraComponent>();
      * for (entity : view)-> 
        * auto& [transform, camera] =  view.get<TransformComponent, CameraComponent>(entity);
        * 找到主相机和对应的transform
    * 如果主相机存在:
      * 开始2d渲染
      * group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
      * 遍历其得到entity, 使用group.get<TransformComponent, SpriteRendererComponent>(entity);得到对应entity的两个组件, 然后获取其值上传到renderer2d中去。
      * 关闭2d渲染场景
  * onViewportResize(uint32_t width, uint32_t height);  // 场景适配视口更新
    * 记录视口的宽高
    * view 相机组件
    * 遍历每个相机组件，判断是否非固定分辨率
      * 不是则更新其视口大小，使其更新投影矩阵
  * private:
  * onComponentAdded<T>(Entity, Component&)  // 实体添加特定组件时需要场景触发的方法
    * 将此实现实现到cpp中去，虽然是模板。。。
    * static_assert(false);  // 无此特化
  * onComponentAdded<CameraComponent>
    * camera.SetViewportSize(m_viewport_width, m_viewport_height);
  * entt::registry m_registry;  // 注册管理器 -> 组件和实体的容器
  * float m_viewport_width/height;

* 将此头文件加入到Hazel.h的renderer前面
* Renderer2D增加transform + color/texure2d 提交渲染的功能

#### Entity.h
* class Entity
  * public:
  * Entity() = default;
  * Entity(entt::entity, Scene* scene);
  * Entity(const Entity& other) = default;
  * bool hasComponent
    * m_registry.has<T>(entity);
  * T& addComponent
    * assert(!has(xxx));
    * T& component = m_registry.emplace<T>(entity, ...);
    * m_scene->onComponentAdded<T>(*this, component);  // 增加场景中添加特定组件会触发的一些行为
  * T& getComponent
    * assert(has(xxx));
    * m_registry.get<T>(entity);
  * void removeComponent
    * assert(has(xxx));
    * m_registry.remove<T>(entity);
  * operator bool() { return m_entity_handle != 0;}
  * operator entt::entity() const { return m_entity_handle;}
  * private:
  * entt::entity m_entity_handle{0};
  * Scene* m_scene = nullptr;  // TODO: 后续会改造为弱引用模式

#### ScriptableEntity.h
* 脚本实体对象提供继承, 方便实现脚本对象继承编写, 使用一些便捷函数
* class ScriptableEntity
  * public:
    * virtual ~ScriptableEntity();
    * getComponent
    * virtual void OnCreate;
    * virtual void OnDestroy;
    * virtual void OnUpdate(Timestep);
  * private:
  * Entity m_entity;
  * friend class Scene;

#### Components.h
* struct TagComponent
  * std::string& Tag;

* struct TransformComponent
  * // glm::mat4 Transform{1.0f}; 直接以矩阵存储，会让用户设置的一些信息丢失, 比如设置弧度值为1000，处理的时候显然要模上2pi，所以建议将位置, 旋转, 缩放分开存储, 最后处理时计算成一个矩阵即可 
  * glm::vec3 Translation{0.0f};
  * glm::vec3 Rotation{0.0f};
  * glm::vec3 Scale{1.0f};
  * TransformComponent() = default;
  * TransformComponent(const TransformComponent&) = default;
  * TransformComponent(const glm::vec3& translation) :Translation{translation} {}
  * glm::mat4 getTransform() const{
    * auto rotation = glm::rotate(mat4{1.0f}, Rotation.x, {1, 0, 0}) * glm::rotate(mat4{1.0f}, Rotation.y, {0, 1, 0}) * glm::rotate(mat4{1.0f}, Rotation.z, {0, 0, 1});
    * return glm::translate(...)  * rotation * glm::scale();
  * }

* struct SpriteRendererComponent
  * glm::vec4 Color{1.0f};
  * // TODO: 未来需要MaterialInstance来整合Texture和Shader，material(实际上就是shader+任意类型的统一变量)相关内容
  * SpriteRendererComponent() = default;
  * SpriteRendererComponent(const SpriteRendererComponent&) = default;
  * SpriteRendererComponent(const glm::vec4& color) :Color{color} {}

* 增加运行时相机 renderer

* struct CameraComponent
  * Hazel::SceneCamera Camera;
  * bool Primary = true;  // 是否是主相机
  * bool FixedAspectRatio = false;  // 是否固定分辨率

* 原生脚本组件支持
* struct NativeScriptComponent
  * ScriptableEntity* Instance{nullptr};  // 脚本实例对象
  * ScriptableEntity*(*InstantiateScript)() ;// 函数指针
  * ``void(*DestroyScript)(NativeScriptComponent* nsc);``
  * bind()
    * 赋值。T为ScriptableEntity实际继承类型

* 将此头文件加入到Hazel.h的scene后面

#### 相机系统
* renderer/camera.h/.cpp
  * class Camera
  * public:
  * Camera();
  * Camera(const glm::mat4& projection);  // 投影矩阵初始化
  * getProjection();
  * private:
  * glm::mat4 m_projection{1.0f};  // 投影矩阵

* 增加场景相机, 方便向其设置相机范围大小, 
* renderer/SceneCamera.h
  * class SceneCamera : Camera
  * SceneCamera()
    * public:
    * enum class ProjectionType {Perspective = 0, Orthographic = 1};
    * getProjectionType()
    * setProjectionType()
    * void setViewportSize(uint32_t width, uint32_t height);
      * aspectRatio = (float) width/ (float)height;
      * RecalculateProjection();
    * // 透视相机相关api
    * void set/getPerspectiveVerticalFOV(float verticalFOV);
    * float getOrthographicSize() const
    * set/getPerspectiveNearClip(), set/getPerspectiveFarClip();
    * // 正交相机相关api
    * void setOrthographicSize(float size);
      * RecalculateProjection();
    * float getOrthographicSize() const
    * set/getOrthographicNearClip(), set/getOrthographicFarClip();
      * set需要重新计算投影矩阵 RecalculateProjection
    * private:
    * RecalculateProjection();
      * if type == Perspective
        * m_projection = glm::perspective(perspective_FOV, m_aspectRatio, perspective_near, perspective_far);
      * if type == Orthographic
        * orthoLeft = -0.5 * orthographic_size * aspectRatio；
        * orthoRight = 0.5 * orthographic_size * aspectRatio；
        * orthoBottom = -0.5 * orthographic_size
        * orthoTop = 0.5 * orthographic_size；
        * glm::ortho(xxxx) -> 得到投影矩阵
    * ProjectionType m_projectionType = ProjectionType::Orthographic;
    * // 透视相机信息
    * float perspective_FOV = glm::radians(45.0f);  // 垂直视场
    * float perspective_near = 0.01f;  // 近点
    * float perspective_far = 1000.0f;  // 远点
    * // 正交相机信息
    * float orthographic_size = 10.0f;
    * float orthographic_near = -1.0f;  // z
    * float orthographic_far = 1.0f;  // 深度信息
    * m_aspectRatio = 0;  // 当前视口的纵横比

* 扩展renderer2d中的beginscene, 增加常规相机的添加 Camera
* renderer2d::BeginScene(const Camera& camera, const glm::mat4& transform);  // 相机 + transform , 通过相机获取投影矩阵, 再将transform逆置得到视图矩阵，最后利用pv矩阵(glm and opengl)上传到shader中去



#### 实体系统展示时的imgui相关
* ImGui::Separator();  // 插入一条分隔线（水平线），用于视觉上区分不同的区域或控件组。
* ImGui::Checkbox("xxx", &xxx);   // 复选框


### Editor GUI
#### 场景层次面板
* editor/Panels 面板文件夹
* 场景对象负责将场景内的信息取出渲染到场景面板上。类关联到Scene类，存在renderergui相关方法，将场景的树结构进行渲染，为后续的父子对象，删除，添加等做好准备
* class SceneHierarchyPanel
  * public:
  * SetContext() 设置scene对象
  * onImGuiRenderer() 将所有的entity取出, 渲染场景层次
    * drawEntityNode(entity);  // 递归渲染单个entity节点
    * 在场景面板中检查: ImGui::IsWindowHovered() && IsMouseDown(0);
      * 如果鼠标按下并且是在当前窗口悬停, 空白区域
      * m_selection_context = {};  // 置空
    * 添加/删除entity
      * if BeginPopupContextWindow(0, 1, false)  弹出上下文窗口 01默认参数, false表示如果选中一个当前串口的item则不会显示此窗口
        * 空白处右键窗口
        * if MenuItem("创建空实体")
          * CreateEntity("Empty Entity");
        * EndPopup();
    * drawEntityNode()  渲染节点
      * ImGuiTreeNodeFlags flags = ((m_selection_context == entity)? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
      * bool opened = TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
      * if (ImGui::IsItemClicked())
        * m_selection_context == entity;
      * bool entityDeleted = false;
      * BeginPopupContextItem()  // 如果右键当前的item对象
        * MenuItem("删除实体")
          * entityDeleted = true;  // 这里就存在不希望立即删除实体的需求了, 因为下面仍然在处理, 可能会访问删除的实体, 所以希望实体只是被标记删除, 下一帧开始之前将其删除即可
          * EndPopup();
      * if (opened) ...模拟实现递归结构 - 子元素
        * flags
        * opend ... 
        * ImGui::TreePop()
      * if (entityDeleted)
        * DestoryEntity(entity);  // 延迟删除
        * if (m_selection_context == entity)
          * m_selection_context = {};// 注意此处需要置空, 否则属性面板渲染的就是已经被摧毁的entity对象
  * private:
  * Entity m_selection_context
  * Ref<Scene> m_context;

#### 属性面板
* 仍然在类SceneHierarchyPanel中进行添加
* onImGuiRenderer()
  * ...
  * Begin("对象属性")
  * if (m_selection_context) {
  * drawComponents(Entity entity);
  * // 添加组件功能
  * if (Button("添加组件))
    * OpenPopup("AddComponent");  // 通过按钮打开上下文窗口
  * if(BeginPopup("AddComponent"))
    * MenuItem("Camera")
      * AddComponent<CameraComponent>();
      * CloseCurrentPopup();
    * MenuItem("Sprite Renderer")
    * EndPopup();
  * }
  * end()
* private: drawComponents(Entity entity);
* 根据if和hasComponent，依次进行绘制组件 // todo: 组件的先后顺序?，但是tag和transform是固定的 ->


* 增加便于写UI的函数, 未来可以将其移动到UI库中
* static void DrawVec3Contol(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f -> 标签的宽度);
  * 效果: 位置: [x]-- [y]-- [z]--
  * PushID(label.c_str())  // 用于整个控制下面控件的绑定id，防止复用代码时会将值全部绑定到一起
  * ImGui::Columns(2);   // 从这行开始下面的ui开始将其设置为2列
  * SetColumnWidth(0, columnWidth);  // 设置第一列的宽度
  * Text(label.c_str());
  * NextColumn();  // 开始添加下一行
  * PushMultiItemWidths(3， CalcItemWidth());  // 设置多个item的宽度 -> 添加imgui_internal.h头文件进行包含 
  * PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.0f})  // 项目间距
  * lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;  // 计算行高
  * ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight};
  * // 创建按钮 x、y、z
  * PushStyleColor(imGuiCOl_Button, imVec4{0.8f, 0.1f, 0.15f, 1.0f})  // 按钮颜色: 普通，悬停，激活
  * PushStyleColor(imGuiCOl_ButtonHovered, imVec4{0.9f, 0.2f, 0.2f, 1.0f})
  * PushStyleColor(imGuiCOl_ButtonActive, imVec4{0.8f, 0.1f, 0.15f, 1.0f});
  * // Y: {0.2, 0.7, 0.2} {0.3, 0.8, 0.3} {0.2, 0.7, 0.2}
  * // Z: {0.1, 0.25, 0.8} {0.2, 0.35, 0.9} {0.1, 0.25, 0.8}
  * if Button("X", buttonSize)
    * values.x = resetValue;  // 重置值
  * PopStyleColor(3);
  * SameLine();  下面开始的内容显示的内容在同一行上
  * DragFloat("##x", &values.x, 0.1f， 0.0f, 0.0f, "%.2f");  // ## 不会将标签显示出来
  * PopItemWidth();  // 弹出项的宽度, 当前设置了1个item
  * SameLine();  显示的内容在同一行上
  * // 上述内容同时设置xyz，从if开始
  * PopStyleVar();
  * ImGui::Columns(1);   // 设置回1列
  * PopID();


* drawComponents开始
* TagComponent -> tag可修改
  * char buffer[256];
  * memset(buffer, 0, sizeof(buffer));
  * strcpy(buffer, tag.c_str());  // strcpy_s
  * if InputText("Tag", buffer, sizeof(buffer)) {
  * tag = std::string{buffer};
  * }

* const ImGuiTreeNodeFlags treeNodeFlags =  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;  // 默认展开子内容和运行项目重叠（目的时在treenode这一行继续添加控件）
* 组件增加treeNode方便折叠展开
* ImGui::PushStyleVar(FramePadding, ImVec2{4, 4});
* bool open = TreeNodeEx((void*)typeid(xxxxclass).hash_code(), treeNodeFlags, "标题")
* SameLine(GetWindowWidth() - 25.0f);  // 下个控件对齐到设置的距离
* if Button("+", ImVec2{20, 20})  // 增加按钮+方便提示用户组件选项存在额外的内容
  * ImGui::OpenPopup("ComponentSettings");
* ImGui::PopStyleVar();
* bool removeComponent = false;
* if BeginPopup("ComponentSettings")
  * if MenuItem("删除组件")
    * removeComponent = true;  // 同样需要延迟删除
  * EndPopUp();
* if open
  * ..... 组件属性ImGui内容
  * ImGui::TreePop()
* if removeComponent
  * entity.removeComponent<xxx>();
* 上述删除组件的功能除开transform和tag组件不存在，其余组件均存在

* TransformComponent
  * // 以平移来将(平移矩阵的最后一列), DragFloat3 -> transform[3][0] //... 
  * DrawVec3Contol("Translation", ...);
  * DrawVec3Contol("Rotation", ...);  // 注意转换为度数给用户提供，degress(), 随后在转换回来 radians()
  * DrawVec3Contol("Scale", ...);


* CameraComponent
  * 运行时相机组件
  * ImGui::Checkbox("Primary”, )  // 设置是否是主相机 
  * const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};  // 透视和正交
  * const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
  * if (BeginCombo("Projection", currentProjectionTypeString))  // 下拉框选择
    * for i, 2
      * bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
      * if (ImGui::Selectable(projectionTypeStrings[i], isSelected))  // 下拉选择框
        * currentProjectionTypeString = projectionTypeStrings[i];
        * camera.setProjectionType((ProjectionType)i);
      * if (isSelected)
        * SetItemDefalutFacus();  // 当前选项位默认选中ui
    * ImGui::EndCombo();
  * if 判断类型
    * Perspective
      * FOV, Near, Far
      * FOV 使用glm::degrees()  将弧度值转换位角度值, 设置时使用glm::radians()再将角度值转换为弧度值
    * Orthographic
      * size，Near，Far
      * if (DragFloat("Size", &size))
        * setOrthographicSize(size);
      * ...
  * ImGui::Checkbox("Fixed Aspect Ratio”, )  // 是否固定纵横比

* SpriteRendererComponent
  * 精灵组件用户界面
  * ColorEdit4("Color", ...)

#### 变换控件
* 使用ImGUI三方库: ImGuizmo

* 控件使用思路:
  * 在更新中检测当前选择的实体是否存在
  * 存在的话进行控件渲染
  * 快捷键可以切换控件的变换模式(平移，旋转，缩放)
  * 应用回实体的transform组件(注意其中的旋转是计算变换量然后累加)
  * 使用hazel自身的分解transform函数进行拆分平移、旋转、缩放

#### 内容浏览面板
* 电脑上某个包含资源的文件夹
* ContentBrowserPanel
  * 使用filesystem::directory_iterator 遍历当前的目录


* 内容的拖拽和释放
* if BeginDragDropSource 可以拖拽的目标(在任何imgui view里面写这句表明任何imgui元素可拖拽)
* setDragDroppayload("name", void*, size, ImGuiCond_Once);  // 输入数据, 拷贝数据, ImGuiCond_Once防止多次拷贝
* EndDragDropSource
* 需要注意每次的id是否一致，否则每次就是拖拽的一个id, pushId()， popID();

* if BeginDragDropTarget() 可以释放的目标
* AcceptDragDropPayload("name");  // 获取对应name的拷贝数据
  * 注意检查返回值是否为空
* EndDragDropTarget();


### 序列化和反序列化
* 存在序列化和反序列化是能协同开发, 长期开发的基础。由于存在git管理和人类阅读的习惯，通常将保存的文件使用文本文件存储，里面好用且清晰的，使用yaml-cpp, 将其编译为C++静态库链接到项目中来, 提供文件序列化和反序列化的功能.
* 从scene出发, scene -> entity -> component. 使用独立于这些对象之外的类进行序列化即可

* Scene/scene_serializer.h/.cpp

* class SceneSerializer
* public:
* SceneSerializer(const Ref<Scene>& scene);
* serialize(const std::string& filepath);
* serializeRuntime(const std::string& filepath);  // TODO
* deserialize(const std::string& filepath);
* deserializeRuntime(const std::string& filepath);  // TODO
* private:
* Ref<Scene> m_scene;

* yaml语法:
  * 从Emitter开始执行序列化
  * BeginMap;  //使用一个map, 后面全是使用Key和Value的
  * 其中Value可以不用指定输入, 后面可以直接着BeginMap
  * EndMap结束map
  * BeginSeq是序列，类似于数组
  * EndSeq结束序列
  * 对于自定义类型序列化, 重载<<即可
  * YAML::Node data; 接收节点
  * YAML::Load(str);  // 进行反序列化
  * data[xxx].as<xxx>();  // 进行返回
  * data[xxx] = value(map里键值对里的值或者子node)
  * 自定义反序列化.as<xxx>()
    * 在YAML namespace里操作
    * template<>
    * 在struct convert<xxx>  //特化类中实现
    * static Node encode(const xxx& rhs)
      * Node node;
      * node.push_back...
      * return node;
    * static bool decode(const Node& node, xxx& rhs)
      * if !node.IsSequence() || node.size() != 3 return false
      * rhs.x = node[0].as<float>();  ...
      * return true;

#### 添加打开/保存文件
* Hazel/Utils/PlatformUtils.h
* 同input，只会在对应编译的平台对应实现生效。

* class FileDialogs;
  * static string OpenFile(const char*)
  * static string SaveFile(const char*)

* Platform/Windows/WindowsUtils.cpp
* commdlg.h -> 32位通用对话API
* GLFW/glfw.h -> 依附于现在创建窗口
* #define GLFW_EXPOSE_NATIVE_WIN32
* GLFW/glfw3native.h -> 返回关心的WIN32相关接口
- OpenFile
    - ...


* 注意bug: 如果此时选中一个entity，那么此时信息中存储的m_selection_context是对应场景的entity，但是如果此时切换别的场景，那么此时调用entity相关方法就会出现失效。
* 解决此需要SceneHierarchyPanel同步到当前切换的场景，也就是说setContext时候将m_selection_context置空

* 增加New，Open，SaveSceneAs 场景到EditorLayer中去
* 增加快捷键


### 编辑器相机
* 构建场景和运行游戏是多个不同的相机对象进行渲染
* 构建场景时(非运行时)需要用到编辑器相机，此相机并非运行时相机组件

* 编辑器更新渲染的逻辑:
  * 不会更新场景内的脚本组件
  * 不会去找运行时相机组件然后开始渲染(场景运行时需要做的工作)

* 之前的Scene->onUpdate 更新为 onUpdateRuntime(ts)
* 新增onUpdateEditor(ts, EditorCamera&)
* 后续优化的点在于讲这两个抽象为两个类实现，因为真正运行的时候，是不需要编辑器场景的各种UI元素的，打包成游戏我们并不想把这些和游戏运行时无相关的东西加进来。


* onUpdateEditor(ts, EditorCamera&)
  * beginScene 开始批量渲染即可


* 添加EditorCamera (Hazel/Renderer/EditorCamera.h/.cpp)
  * 主要是通过界面UI操作相机的视图矩阵

* 注意讲之前的ImGuizmo更新为编辑器相机

* 编辑器相机操作: alt + 左键 = 旋转 alt + 中键 = 平移 (alt + 右键) | 滚轮 = 缩放


### 鼠标拾取
* 思路: 鼠标坐标映射到选中的纹理坐标上，此纹理坐标是附加的纹理坐标，用来显示实体ID的, 获取后即可知道选择的entity是谁
* 附加数据?额外的渲染通道，IDBuffer?

#### 帧缓冲区重构
* 扩展创建帧缓冲区所使用的纹理样式
* enum class FramebufferTexureFormat
  * None, RGBA8 - 颜色(8)，DEPTH24STENCIL8 -  深度模板(深度24，模板8), Depth = DEPTH24TENCIL8(默认) 

* 帧缓冲区的纹理实际规范
* struct FramebufferTexureSpecification
  * FramebufferTexureSpecification() = default;
  * FramebufferTexureSpecification(TexureFormat)
  * FramebufferTexureFormat TexureFormat;
  * TODO: filtering/wrap

* 帧缓冲区附件规范
* struct FramebufferAttachmentSpecification
  * std::vector<FramebufferTexureSpecification> Attachments;

* FramebufferSpecification
  * FramebufferAttachmentSpecification Attachments;
  * uint32_t Samples = 1;  // 是否多重纹理采样? >1


* TextureTarget -> multisampled? GL_TEXTURE_2D_MULTISAMPLE: GL_TEXTURE_2D
* createTextures(bool multisampled, uint32_t * out_id, uint32_t count) {
  * glCreateTextures(TextureTarget(multisampled), count, out_id);
* }
* bindTexture(bool multisampled, uint32_t id) -> glBindTexture(TextureTarget(multisampled), id);
* AttachColorTexture(id, Samples, GLenum format, width, height, index) {
  * bool is_multisampled = Samples > 1;
  * if (is_multisampled) {  //多重采样处理
    * glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, format, width, height, GL_FALSE);
  * }
  * else{
    * glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    * glTexParameteri(GL_TEXTURE_2D, MIN_FILTER/MAG, LINEAR)
    * glTexParameteri(GL_TEXTURE_2D, WRAP_R, GL_CLAMP_TO_EDGE)
    * glTexParameteri(GL_TEXTURE_2D, WRAP_S, GL_CLAMP_TO_EDGE)
    * glTexParameteri(GL_TEXTURE_2D, WRAP_T, GL_CLAMP_TO_EDGE)
  * }
  * glFramebufferTexturee2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(is_multisampled) id, 0);  // 附加到帧缓冲区上
  * 
* }
* AttachDepthTexture(..., GLenum format, GLenum attachment_type, ...);\
  * if ....
  * glTexStorage2D(GL_TEXTURE_2D, 1, format, witdh, height);
  * ...
  * glFramebufferTexturee2D(GL_FRAMEBUFFER, attachment_type, ...)
* IsDepthFormat(FramebufferTexureSpecification)  // 判断传入的规范是否是深度模板

* OpenGLFramebuffer
  * 构造函数
    * 将附件分类
    * 遍历 FramebufferAttachmentSpecification中的每个附件规范
    * 如果是depth就赋值m_depth_attachment_spec,否则m_color_attachment_specs就进行加入
    * invalidate();
  * invalidate() 实际添加附件纹理
    * 清除时:
      * 对于m_color_attachments, m_depth_attachment进行清空
    * 提前对m_color_attachments进行扩容
    * createTextures(is_multisample, m_color_attachments.data(), m_color_attachments.size());  // 一次性全部创建完毕
    * 遍历每个color附件()
      * 指定使用样式，分配内存
      * bindTexture
      * switch TextureFormat
        * RGBA8: AttachColorTexture(id, Samples, GL_RGBA8, 宽度, 高度, index) 
    * if m_depth_attachment_spec.TexureFormat != None
      * createTextures(is_multisample, &m_depth_attachment, 1);
      * bindTexture
      * switch TextureFormat
        * DEPTH24TENCIL8:  AttachDepthTexture(id, Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, 宽度, 高度) 
    * 如果存在颜色附件大于1的情况，则assert只能支持4个
      * Glenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
      * glDrawBuffers(m_ColorAttachments.size, buffers);
    * else if (颜色附件为空)
      * glDrawBuffers(GL_NONE);
  * private:
    * vector<FramebufferTexureSpecification> m_color_attachment_specs;
    * FramebufferTexureSpecification m_depth_attachment_spec;
    * vector<uint32_t> m_color_attachments;  // 颜色缓冲区每个附件的id
    * uint32_t m_depth_attachment = 0;  // 深度缓冲区附件的id

* 测试效果, 帧缓冲区创建RGBA8, RGBA8, Depth 的效果
* 然后在片段着色器中使用layout(location = 1) out vec4 color
  * color = xxx

然后在使用帧缓冲区时使用第二个颜色附加区即可, 查看是否渲染为设置的第二个颜色?


#### 准备第二个输出int的附加
* 基本思路: 给帧缓冲区增加int类型的附加纹理输出, 利用每个entity渲染时，将其entity的id设置到顶点缓冲区中，映射到片段着色器中layout 2 out进行输出，随后计算鼠标的相对坐标，读取其坐标所在的第二个像素槽的值，方可知道当前entity的id

* 为帧缓冲区每一部分的颜色附加增加颜色清除, 这样方便在RGB_INTER类型的附加中设置-1, 方便铺满背景, 点击时显示此处不存在实体对象



#### 鼠标拾取
* 需要逐渐意识到, 此时做的是编辑器鼠标拾取, shader也是编辑器渲染的专属shdaer，所以在设计API的时候需要考虑是否影响其运行时的效率问题
* 基本思路就是给每个实体渲染的顶点数据中增加一个int类型的数据, 使其传入顶点shader后传递给片段shader，从而使其输出给颜色附件
* 需要注意使用guizmo的时候不要让选择实体触发, 通过guizmo的一些判断条件避免设置即可: !IsOver() 另外还存在按下alt控制相机时点击也会消失，检查不按下alt即可



### 材质系统


#### shader重构
* texture.glsl
  * 针对于相机的投影矩阵, 因为在一次绘制调用中, 渲染的每个对象的投影矩阵并不会发生改变, 又或者是说存在不同的材质(不同的shader), 没有必要每次都给这些shader上传更新投影矩阵的值，可以使用一种叫uniform buffer的东西, 与GPU交互的可被shader访问的内存块，内存块可以被任意的shader进行访问
```glsl
layout(std140, binding = 0) uniform Camera
{
  mat4 u_ViewProjection;
}
```
  * 以前的``uniform mat4 u_ViewProjection`` 需要给每一个shader都上传一遍, 而uniform buffer则不需要
  * 注意shader的版本变为了"450 core"
  * 后续凡是不单绑定于shader的数据均可通过此方式进行上传
  * 对于sampler2D, 以前单个的话``uniform sampler2D u_Texture`` 可以通过uniformI设置绑定的纹理槽, 但是现在可以在前面shader文件指定纹理槽``layout (binding = 0) uniform sampler2D u_Texture`` 不需要手动在gpu进行指定
  * 另外一些同一对象的数据可以使用struct整合起来, 输出的时候一样按顺序输出即可(location会从0 -> x 个数就是你的struct里的数据个数)
  * flat作用就是禁止插值, 防止一些固定值因为插值而发生变化, 不是预期的效果
  * 无论in还是out，前面均可使用``layout(location=xxx)`` 指明位置, 另外in和out是分开的
  * 使用spurv进行编译着色器, 这样会生成缓存, 不更新shader的话每次就读取缓存就不会重新的进行编译着色器了
  * 使用vulkan glsl shader的流程:
    * vulkan glsl -SPIR-V编译-> 1二进制文件 -SPIR-V交叉编译器-> opengl glsl ->SPIR-V编译-> 2二进制文件
    * 缓存这两个二进制文件, 下次无需编译直接使用



### 2d物理引擎
* 使用三方2d物理引擎box2d: https://github.com/erincatto/box2d
* 场景中运行的思路:
  * 启动运行时更新时, 检查当前场景中所有存在2d刚体组件的实体, 检查后将其信息以及存在的碰撞组件添加入物理更新世界
  * 更新时, 脚本更新完毕后, 执行物理引擎更新, 更新完毕将实时应用的数据反馈回刚体组件的对象

* box2d使用细节:
  * Scene中包含物理更新世界: b2World* m_physic_world;
  * b2World:
    * 创建世界时赋予持续的水平力和持续的竖直力:new b2World{0.0f, -9.8f};
    * 创建一个对象, 刚体:
      * b2BodyDef bodyDef; b2Body* body = m_physic_world->CreateBody(&bodyDef);
      * bodyDef.type, position.Set(x, y), angle = Rotation.z
    * 更新:
      * // 控制物理模拟的细粒度, 取决于制作的游戏类型
      * const int32_t velocityIterations = 6;
      * const int32_t positionIterations = 2;
      * m_physic_world->step(ts, velocityIterations, positionIterations);
  * b2Body 
    * SetFixedRotation();  // 设置是否固定旋转
    * 创建碰撞器:
      * b2PolygonShape polygonShape; // 设置多边形形状
      * polygonShape.SetAsBox(hx, hy);  // 设置为box, 里面值均为半宽和半高， 这里为了和物体设置的scale保持一致, 便可以何其相互乘
      * b2FixtureDef fixtureDef;  //设置固定装置
      * fixtureDef.shape  = polygonShape;
      * fixtureDef.物理特性
      * CreateFixture(fixtureDef); 
  * Rigidbody2DComponent
    * BodyType Static, Dynamic, Kinematic type
    * bool FixedRotation 是否固定z轴上的旋转
    * void* RuntimeBody = nullptr;  // 作为2d物理世界创建实体的句柄保存
  * BoxCollider2DComponent 2d碰撞盒子组件
    * vec2 offset (和transform之间的偏移)
    * vec2 size(0.5, 0.5) (碰撞盒子大小)
    * 物理特性:
    * float Density= 1.0f;  // 密度
    * float Friction = 0.5f;  // 摩擦力
    * float Restitution = 0.0f  // 反弹系数
    * float RestitutionThreshold =  0.5f// 反弹恢复阈值
    * 后续这些物理特性需要移转到物理材质中去

### UUID
* 游戏引擎里的需求基本利用C++随机库生成一个uint64_t 位的uuid即可
* 使用random_device s_random_device、mt19937_64 s_engine{s_random_device}, uniform_int_distribution<uint64_t> s_uniform_distribution;
* s_uniform_distribution(s_engine); 生成分配即可
* 注意利用下hash
* namespace std
*
* template<>
* struct hash<UUID>
* operator()(const UUID& uuid) const
  * return std::hash<uint64_t>()((uint64_t)uuid);

* 使用场景:
  * 新创建实体时
    * 使用IDComponent, UUID作为成员
    * 创建实体时, add一个IDComponent即可
    * 另外创建实体可以给予一个UUID进行创建实体

### 复制场景和实体
* Scene::Copy 
  * 利用IDComponent, 找到所有uuid和对应的entt, 建立一个映射关系
  * 利用id和tag在新场景创建每一个entity
  * 利用映射找到新老entity, 将老entity中的component 插入和替换(替换的存在是因为存在默认的transform组件)
* DuplicateEntity ctrl + d
  * 利用老entity创建一个相同tag和相同组件的实体

* 保存当前场景 ctrl s