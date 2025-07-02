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