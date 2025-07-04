# 渲染缓冲区抽象和实现

## 顶点/索引缓冲区的简单抽象
* 对于渲染接口的抽象, 取决于是编译期决定还是运行期决定
* 本质上是将渲染接口分解为每一个图元对象(各种缓冲区, 纹理类, shader类......)
  * 对于编译期决定的缺点: 每一个都是一个独立的构建 
  * 运行期切换： 存在运行期决策

* 文件:
  * Renderer/Buffer.h/.cpp

* 需要设计为纯虚接口
* VertexBuffer
  * ~VertexBuffer() {}
  * virtual void Bind () const = 0;
  * virtual void UnBind () const = 0;
  * static VertexBuffer* create(float* vertices, uint32_t size); (cpp中实现，用来运行中决定创建什么类型的)

* IndexBuffer
  * ~IndexBuffer() {}
  * virtual void Bind () const = 0;
  * virtual void UnBind () const = 0;
  * static IndexBuffer* create(uint32_t* indices, uint32_t count); (cpp中实现，用来运行中决定创建什么类型的)
  * virtual uint32_t GetCount() const = 0

* Platform/OpenGL/OpenGLBuffer.h/.cpp
  * OpenGLxxxBuffer
  * VertexBuffer
    * 构造函数: loat* vertices, uint32_t size
      * // gen and bind gen->Create , 让gl接口保持一致
      * glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);  // 上传数据
    * 析构函数
      * glDeleteBuffers(1, &m_renderID);
    * Bind
      * glBindBuffer GL_ARRAY_BUFFER
    * UnBind
      * glBindBuffer GL_ARRAY_BUFFER 0
    * uin32_t renderid;

* index类似

* 在app中添加上唯一指针进行调试运行即可

* 为了让渲染器能够集中管理这些功能, 创建Renderer类
* enum class RendererAPI
  * None = 0, OpenGL = 1
* Renderer class  
  * static RendererAPI 类型
  * getAPI -> static 类型的方法返回

* create 据此创建对应的buffer


## buffer layouts
* API设计需要简单, 优雅, 美观.

* 顶点缓冲布局
  * 这个字节数组的数据, 里面有什么, 如何分解, 有多少元素
  * 解析API: glEnableVertexAttributeArray / glVertexAttribPointer
  * 顶点属性实际上存储在顶点数组(父)当中, 而不是顶点缓冲区中
  * 一个顶点数组可以包含多个缓冲区
  * glVertexAttribPointer中设定的size是指整个顶点所占的大小
  * 布局和shader精密联系, shader中通过layout(location = xxx) xxx; 获取


* BufferLayer 来描述布局
  * ShaderDataType: 布局产生变量的类型
  * name, 变量的名字(opengl可能不需要, 但是DX中可能需要, 总之给布局留一个名字总是不错的选择)

* BufferLayer设计
  * 属性: vector<BufferElements> m_elements;
    * getter
  * 构造函数, 不能直接传入vector<BufferElement>, 而是使用const std::initializer_list<BufferElement>& elements初始化列表进行初始化
    * 直接将elements传递给vector进行初始化即可
    * 随后就需要对这里的每一个布局元素计算其offset和总大小之类的, 使用函数CalculateOffsetsAndStride(私有)
  * 属性: uint32_t m_stride;  当前布局所有元素的总偏移量 getter
  * CalculateOffsetsAndStride
    * 遍历vector, 计算每个布局元素的offset和汇集总的stride
  * 提供begin/end方便迭代

* struct BufferElement
  * string Name
  * ShaderDataType Type
  * uint32_t Offset
  * uint32_t Size
  * bool Normalized
  * 构造函数
    * type, Name(size, offset需要后面计算)
  * GetComponetCount 
    * 根据枚举, 算每个类型的最小类型的个数是什么, 比如Float3就是3
  * 可以在app里临时写一个ShaderDataTypeToOpenGLBaseType



* enum class ShaderDataType: uint8_t
  * None, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool

* static uint32_t ShaderDataTypeSize(ShaderDataType type)
  * 返回每个类型对应的大小即可(不能sizeof, 因为不同平台不一样)
    * float 4byte, int 4byte, bool 1byte, Mat3: 4 * 3 * 3, Mat4: 4 * 4 * 4;


* VectorBuffer
  * virtual SetLayout (const BufferLayout& layout) = 0 设置布局
  * virtual const xxxx const getter = 0 获取布局

