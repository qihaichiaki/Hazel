# 渲染架构

渲染系统最终存在两个方面:
| Render Api     | Renderer               |
| -------------- | ---------------------- |
| Render Context | 2D/3DRenderer          |
| Swap chain     | Scene Graph            |
| Frame Buffer   | Sorting                |
| Vertex Buffer  | Culling(剔除)          |
| index Buffer   | Materials(材质)        |
| Texture        | LOD                    |
| Shader         | Animation              |
| States         | Camera                 |
| Piplines       | VFX (视觉效果)         |
| Render Passes  | PostFX(后期效果)       |
|                | 随机, 反射, 环境光遮蔽 |

其中, RenderApi相当于是渲染原语, 通过其告诉GPU应该怎么做, 对此不同的技术存在不同的一份实现(OpenGL, Vulcan......)
材质和shader紧密相关, 材质实际上就是一个着色器机上一些统一的数据
渲染指令队列: 将每个渲染指令编码为一个大缓冲区的方法

1. 使用OpenGL 
   - 最简单的接口, 需要外部强大的设计才能使其发挥真正作用
2. 构建Render Api(材质系统......)
3. 构建Renderer
4. 实现基本的渲染器
5. 实现其他功能
   - 比如使用其他的底层库实现


## Render Context
* 对于在glfw初始化时调用的``glfwMakeContextCurrent(m_window);``, 会根据当前的上下文(opneGL...), 接收window句柄, 并且设置为我们实际使用的渲染接口的渲染目标或上下文
* 因为后续可能会支持其他glfw未支持的图形渲染接口(比如windows下的dX11), 所以``glfwMakeContextCurrent``api不能调用, 需要自己手动去抽象
* ``glfwSwapBuffers``同理
* window->renderContext 
   - ``GraphicsContext* context = new OpenGContext()`` 在glfw初始化窗口之后

* 文件目录
  * Hazel/Renderer/GraphicsContext.h
  * Platform/OpenGL/OpenGLContext.h/.cpp

* GraphicsContext 接口定义
  * 纯虚: Init(), SwapBuffers()

* OpenGLContext 具体实现
  * 构造函数: 需要传递glfwWindow句柄 - windowHandle
  * Init: glfwMakeContextCurrent + gladLoadGLLoader
  * SwapBuffers: glfwSwapBuffers


## 渲染首个三角形
* 先在application 使用原始的opengl相关接口, 将整个渲染流程编写下来, 后续根据此流程进行封装类, 符合从使用->封装的一个学习优化过程

* 创建一个三角形所需要的步骤:
> vertex array, vertex buffer, index buffer, shader(顶点/片段 - 可以使用GPU默认的着色器)

* 实现:
```c++
unsigned int vertexArray, vertexBuffer, indexBuffer;
 
glGenVertexArrays(1, &vertexArray);  // 生成一个顶点数组
glBindVertexArray(vertexArray);  // 绑定

glGenBuffers(1, vertexBuffer);  // 创建缓冲区
glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);  // 绑定为顶点缓冲区

// opengl默认处理的渲染空间: xyz= -1 ~ 1 
// 下面创建顶点数组的数据
// 按照逆时针旋转, 以屏幕中心点为中心, x为横轴, y为竖轴, z是从屏幕中冲出来
float vertices[3 * 3] = {
   -0.5f， -0.5f， 0.0f，
   0.5f， -0.5f， 0.0f，
   0.0f， 0.5f， 0.0f
}

// 上传顶点数据到GPU
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // GL_STATIC_DRAW 静态渲染, 不会每帧上传数据

// 为shader提供顶点属性, 其可以进行处理
// 启用数据的索引0
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float)， nullptr)  // stride->不同顶点之间的字节量,  属性偏移无, 只存在位置这个属性

// 创建索引缓冲区
glGenBuffers(1, &indexBuffer);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
// 作为一个顶点缓冲区的索引, 告诉如何绘制这些定顶点

unsigned int indices[3] = {0, 1, 2};
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


// 每帧渲染上做的:
glBindVertexArray(vertexArray);
glDrawElements(GL_TRIANGLES, 3, GL_UNSIGEND_INT, nullptr);
```

* 在OpenGLContext的初始化中打印显卡信息:
  * glGetString(GL_VENDOR)
  * GL_RENDERER
  * GL_VERSION


## OpenGL Shader
* 显卡上运行的程序, 告诉显卡如何处理我们发送给它的数据
* 理解图形管道
* 两种常用的shader: vertex shader(顶点着色器) and fragment shader(片段着色器->像素着色器)

* 顶点着色器决定渲染的每一个顶点(3d空间中的一个位置)
  * 顶点着色器会在每个顶点上运行一次, 所以实际上顶点着色器运行了顶点次的个数, 顶点着色器在**裁剪空间**中的定位
    * 通常是将它与模型视图投影矩阵相乘把它放到那个裁剪空间
    * 发送数据到片段着色器，并插值做其他的
  * 然后我们在管道上移动， 最终到达片段着色器， 片段着色器按像素运行 -> 填充图形(像素填充) -> 像素多少此shader就会运行多少次

* [shader示例wiki](https://www.khronos.org/opengl/wiki/Shader_Compilation)

* shader示例代码:
```c++
// Read our shaders into the appropriate buffers
std::string vertexSource = // Get source code for vertex shader.
std::string fragmentSource = // Get source code for fragment shader.

// Create an empty vertex shader handle
GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

// Send the vertex shader source code to GL
// Note that std::string's .c_str is NULL character terminated.
const GLchar *source = vertexSource.c_str();
glShaderSource(vertexShader, 1, &source, 0);

// Compile the vertex shader
glCompileShader(vertexShader);

// 检查编译是否成功
GLint isCompiled = 0;
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
if(isCompiled == GL_FALSE)
{
	GLint maxLength = 0;
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> infoLog(maxLength);
	glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
	
	// We don't need the shader anymore.
	glDeleteShader(vertexShader);

	// Use the infoLog as you see fit.
	
	// In this simple program, we'll just leave
	return;
}

// Create an empty fragment shader handle
GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

// Send the fragment shader source code to GL
// Note that std::string's .c_str is NULL character terminated.
source = (const GLchar *)fragmentSource.c_str();
glShaderSource(fragmentShader, 1, &source, 0);

// Compile the fragment shader
glCompileShader(fragmentShader);

glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
if (isCompiled == GL_FALSE)
{
	GLint maxLength = 0;
	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> infoLog(maxLength);
	glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);
	
	// We don't need the shader anymore.
	glDeleteShader(fragmentShader);
	// Either of them. Don't leak shaders.
	glDeleteShader(vertexShader);

	// Use the infoLog as you see fit.
	
	// In this simple program, we'll just leave
	return;
}

// Vertex and fragment shaders are successfully compiled.
// Now time to link them together into a program.
// Get a program object.
GLuint program = glCreateProgram();

// Attach our shaders to our program
glAttachShader(program, vertexShader);
glAttachShader(program, fragmentShader);

// Link our program
glLinkProgram(program);

// Note the different functions here: glGetProgram* instead of glGetShader*.
GLint isLinked = 0;
glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
if (isLinked == GL_FALSE)
{
	GLint maxLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

	// The maxLength includes the NULL character
	std::vector<GLchar> infoLog(maxLength);
	glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
	
	// We don't need the program anymore.
	glDeleteProgram(program);
	// Don't leak shaders either.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Use the infoLog as you see fit.
	
	// In this simple program, we'll just leave
	return;
}

// Always detach shaders after a successful link.
glDetachShader(program, vertexShader);
glDetachShader(program, fragmentShader);
```

> 上述代码是编译vertex shader和fragment shader的示例
> 先放在Hazel/Renderer目录下/ Shader.h


* Shader类
  * 构造函数: 通过字符串引入shader程序(vertexShader and fragment Shader)
    * 将示例代码复制粘贴
    * 在编译/链接失败时加上断言 + 日志输出即可
  * 析构函数:
    * glDeleteProgram(renderid);  // 此id就是构造编译的shader程序
  * Bind
    * glUseProgram(renderid)
  * Unbind(暂时保留, 方便用于调试)
    * glUseProgram(0)
  * 成员:
    * uint32_t renderid

* OpenGL中, genxxx表示只分配id, 而createxxx是做了很多工作的

* App
  * 将shader封装为unqiue_ptr 成员(可以使用reset初始化)
  * 使用string = R"(//....)" 这样就可以包含多行文本
  * 简单实现一个shader, 让三角形变成红色

* fragmentShader src()
```shader
#version 330 core
layout(location = 0) in vec3 a_Postion  // 这里的0就是gl设置索引属性的0 也就是将顶点中的数据取三个放入a_Postion变量中。 in表示输入

out vec3 v_Position;  // shader之间相互传输数据

void main
{
  v_Position = a_Postion
  gl_Position = vec4(a_Postion, 1.0);  // 输出位置和输入位置一致(1 为w分量)
}
```

* vertexShader src(设置颜色)
```shader
#version 330 core
layout(location = 0) out vec4 color;  // 输出颜色
in vec3 v_Position;

void main
{
  // color = vec4(1.0, 0.0, 0.0, 1.0);  // 纯红色
  color = vec4(v_Position * 0.5 + 0.5, 1.0)
}
```

* 在绘制前绑定即可

## [缓冲区渲染接口设计与抽象](./render_buffer.md)

## [顶点数组抽象](./render_vertex_array.md)

## [DrawCall](./render_draw_call.md)

## [材质系统](./render_materials.md)

## [Shader抽象](./render_shader.md)

## [纹理抽象](./render_texture.md)

## 如何构建2DRender
* 如何在代码里更好的区分2d和3d?
  * Render2D/3D

* 对于大量的图形, 比如瓦片地图, 像类似一个一个去提交是不行的, 性能不允许, 并且也差, 需要用到批量绘制, 也就是同时绘制这么多图形->**批量渲染器**
* 在2DRender里所有东西都可以抽象为一个四边形

* 纹理图集: 如何将多个几何批量处理为一个顶点缓冲区的事情->多个纹理整合为一个纹理

* 动画:
  * 同样是纹理图集, 每一帧存在一个纹理

* 压力测试:
  * 批量渲染器能够在100万张精灵中以每秒60帧的速度运行

* UI
  * 布局系统
  * 字体渲染

## [摄像机控制器](./render_camera.md)

## 2DRenderer准备
* 导出宏的准备, windows需要是被windw+64平台, 不需要外部传入宏HZ_PLATFORM_WINDOWS的定义
* 检查opengl版本: major > 4 || major == 4 && minor >= 5   (>=4.5)
* 修复正交相机控制器里移动:
  * 即在旋转的时候, 上下左右也能正常按照屏幕的方向移动而非旋转的方向左右(x cos(glm::radians(rotation))...) 上下(x -sin(...))
  * 控制旋转时, 超过180度就转换为-180度, 小于-180度的时候就转换为180度(-+360度)

* Sandbox新增sandbox2D.h/.cpp
  * 正交相机控制器
  * 使用一个shader -> flatcolor
  * 顶点数组（创建这些全部改为引用）
  * 专注于正方形渲染
  * update 渲染提交
  * 构造函数初始化Layer和相机控制器即可
  * 入口点从hazel中移除, 放在要控制main函数的地方

* 实现2DRenderer的第一步!

* 需要性能分析工具, 并且在引擎内可见

## [2DRenderer](./render_renderer2d.md)


## 引擎性能分析
* 一帧CPU实际使用需要多长时间?
* 摄像头更新需要多长时间?
* 清除帧需要多长时间?
* 渲染花费的时间是多少?

* 得到数据 -> 将数据进行可视化


* 设计一个timer类, 用于在其生命周期内进行一个计时单位是ms, 利用的库是C++标准库·中的chrono::high_resolution_clock::now() Timer(steady_clock) 计时器 
* 封装stop函数, 可以将对象用于主动计时或者生命周期结束自动计时

* 需要将获取间隔时间改为微秒的接口, 便于测试更加精确(microsecond), 再将其转换为毫秒接口这样会更加精确(小数保留等)

* 将上述的测试信息打印到控制台内会不好看, 所以需要imgui将其数据进行可视化出来
* 创建一个Struct ProFileResult, const char* Name and Time
* Vector 动态数组包含此类型, 当一个timer结束测量的时候将内容推送到此vector中去

* imgui部分:
  * 遍历结果列表
  * Text使用label标注, ProFileResult name(label 50缓冲区, 利用strcpy函数将结果中的内容拷贝到缓冲区中 随后将其利用strcat函数进行字符串拼接" %.3fms"(后面的值进行格式化), 提供给imgui展示)
  * 遍历完成后, profile的Vector进行清除

* 使用模板参数传入lambda表达式, 最后调用传参的时候直接构造struct对象{} 进行传递即可 (function对象存在动态开销)
* 对于Timer的使用可以定义一个宏PROFILE_SCOPE(name) Timer timer##__LINE__(name, lambda) 进行使用

## 可视化性能分析
* 上面的计时器不错, 但是只是显示一帧时的数据, 没有整体的去分析
* 想要基于整体, 又落实于各个部分的数据分析， 以图表的方式再imgui中进行呈现

* 文件: Hazel/Debug/Instrumentor.h(C++中的可视化基准测试里的内容)
* pch进行包含

* Instrumentor.h
  * 里面首先包含分析写出类
  * 利用宏配置, 确保发布版本不包含这些内容的使用
  * __FUNCTION__ 函数名称
  * __FUNCSIG__ 函数签名


* 数据分析处理:
  * 再app创建和run运行:
    * BEGIN_SESSION("Startup", "HazekProfile-Startup.json")
    * BEGIN_SESSION("Runtime", "HazekProfile-Runtime.json")
    * BEGIN_SESSION("Startup", "HazekProfile-Shutdown.json")

### 仪器测量
* 思考: 写入到文件是否是一个很好的做法?
* 游戏引擎力每秒大致60帧, 如果全写到文件内去, 则会产生大量空间并且不豪整体分析
* 可以只是分析一个片段, 比如只分析1s内的60帧