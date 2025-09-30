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
   - Window::create(), 创建窗口对象
   - 设置window事件回调，方便将窗口捕获的事件发送到application, 随后由application从栈顶往下传播
   - Renderer::init() 渲染初始化
   - 创建第一个覆盖层: imguilayer 将其pushOverlay入, 并且调用器attach相关方法

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

#### Renderer
* 游戏的渲染器类. 由于当前引擎适配的是2D渲染, 所以实际内部调用是渲染命令初始化和2d渲染器初始化.
* 当前引擎的思路是，底层渲染接口 -> 渲染命令 -> 2d/3d渲染器封装。除开底层外, 其余均以static的方法向外提供, 只是渲染命令内部存储的RendererAPI为对象, 作为底层实现的对象。
* init()
  * RendererCommand::init()
  * Renderer2D::init()

#### Renderer2D
* 2d渲染类
* init()
  * 准备四边形的基础像素块渲染对象。
  * 首先创建顶点数组对象
  * 然后创建顶点缓冲区, 存在基础坐标(限制在-1, 1)和纹理uv坐标(0, 0 -> 1, 1), 据此设置顶点缓冲区的布局
  * 创建索引缓冲区, 四边形由两个三角形组成, 并且依据右手定则确定好方向
  * 上传到顶点数组对象中
  * 从文件加载texture的shader, 并且创建shader程序对象
  * 创建单位白色纹理
  * 使用shader程序, 并且指定纹理槽为0

##### RendererCommand
* 渲染命令, 方便给上层提供适配好各个api的渲染封装命令可以直接调用, 并且无需依赖实例
* init()
  - renderer_api->init()

##### RendererAPI
* 底层适配RendererAPI对象，下面以OpenGL为例
* init()
  * ``glEnable(GL_BLEND)`` 启用纹理混合, 即当前片段颜色(src)和帧缓冲中存在的颜色(dst)按照某个公式混合在一起, 而不是帧缓冲直接覆盖
  * ``glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);``混合函数设置, 告诉渲染接口如何将src和dst颜色进行混合. 此时启用经典的混合函数, 方便实现透明的效果
  * ``glEnable(GL_DEPTH_TEST);`` 启用深度测试, 让片段最终的z坐标可以决定渲染前后而不依赖绘制顺序