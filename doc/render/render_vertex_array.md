# 顶点数组
* OpenGL相当于entity -> id标识, 其中可以包含顶点缓冲区, 索引缓冲区的引用
* 只不过它只能作为整体的标识, 只是引用,不可控制

* 流程: 绑定VertexArray, VertexBuffer, ->BufferLayout .... 
* 抽象差异: DirectX中, VertexArray通常并不存在


## 抽象VertexArray
* 文件: Renderer/VertexArray.h/.cpp
* 资源在Hazel内部的工作方式都是引用， buffer, 纹理, shader这些都是资源

* AddVertexBuffer(shader_ptr); = 0
* SetIndexBuffer(shader_ptr);
* getXXX const = 0
* Bind const = 0
* UnBind const = 0

* OpenGL实现: OpenGLVertexArray
  * 属性: 
    * vector<shader_ptr VectorBuffer> ... Vectorbuffers
    * shader_ptr IndexBuffer(这个应该只存在一个)
    * m_vertex_array 标识id
  * 构造函数
    * glCreateVertexArrays(1, ...);
  * 析构函数
    * glDeteteVErtexArrays(1, ...);
  * Bind/UnBind
    * glBindVertexArray() ...
  * AddVertexBuffer
    * glBind
    * vertexBuffer->Bind
    * layout set(app中的glEnableXXX/glVertexAttribPointer xxx设置)
    * buffer push_back
  * SetIndexBuffer
    * glBind
    * indexBuffer->Bind

* application中
  * 增加shader_ptr 的 VertexArray(将其他的render资源也都改成shader_ptr)
  * 创建
  * 添加vertexBuufer
  * 添加indexBuffer
  * 注意到由于opengl是基于状态的, 所以当前设计的api实际上存在很强的顺序问题, 如果顺序写错, 可能造成和预期的效果并不一致
    * 比如AddVertexBuffer, 如果你在这个之后才给VertexBuffer设置layout, 那么会失效(没有执行属性索引和属性获取设置), 可以适当增加日志提示属性是否绑定
    * 另外对于vertexBuffer and indexBuffer创建的时候, 需要前面是否创建VertexArray? 这两者之间是否存在依赖?（创建的时候, vertexArray是否需要bind好呢?） 


* application创建一个正方形
  * 创建属性 squareVA
  * 加载顶点缓冲区
  * 设置布局 - 设置3个float的点位置即可
  * 创建indexBuffer(需要注意, 正方形需要安装两个三角形进行创建)
  * 创建一个blue shader, 快速渲染一个蓝色的正方形(0.2 0.3 0.8)

* 清理下之前创建三角形渲染的代码

* 后面讲逐步把这些render组件集成到renderer上, 并且创建真正的渲染命令

