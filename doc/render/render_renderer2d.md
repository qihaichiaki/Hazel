# 2DRenderer

* Shader::SetMat4 SetFloat4
  * 这样不需要强制转换类型, 暂时绕开材质系统去上传统一变量


* 新增文件: Renderer/Renderer2D
* 实现的renderer不存在任何数据, 只有存储是一个类1byete, 其余全是静态方法
    - why? 管理数据, 执行opengl指令，是一个全局的概念

* Renderer 3drender init里可以支持Renderer2d的init
* Shader 
  * SetMat4 抽象
  * SetF0loat4 抽象

1. Init();
    - 初始化顶点数组相关
    - 容器写在cpp的Renderer2DStorage中, 并且在当前翻译单元中定义为static, 对外不可见s_Data;  // 一个静态指针类型, init new, shutdown delete
      - Ref<VertexArray> QuadVertexArray; 
      - Ref<Shader> FlatColorShader;
      - Ref<Shader> TextureShader;
    - 平面图形的四边形初始化流程
      1. 创建四边形的顶点数组对象
      2. 创建四边形的顶点缓冲区对象
      3. 根据顶点缓冲区创建其布局(坐标布局)
      4. vertex buffer 加载入顶点数组(顶点数组绑定, buffer绑定, layout上传)
      5. 创建索引缓冲区
      6. index buffer 加载如顶点数组
      7. 创建平面颜色shader(读取文件/硬编码) (当前选择读取文件)

2. Shutdown();
   - delete s_Data

3. BeginScene(const OrthoGraphicCamera& camera);
   - 将摄像机的投影视图矩阵(PV)上传到gpu->shader
     1. FlatColorShader->bind();                    // 使用shader程序
     2. FlatColorShader->SetMat4(camera->getPV());  // 上传统一变量
     3. TextureShader同理

4. EndScene();

5. DrawQuad(const vec2& position, const vec2& size, const vec4& color);
   - 6的z坐标为0, 其中z标识图层关系
   - 注意当前正交摄像机看向的位置是z的负方向 -> opengl使用的是右手坐标系, 当前屏幕沿着中心, 被投影矩阵设置为了平面笛卡尔坐标系(右边正x, 上边正y)

6. DrawQuad(const vec3& position, const vec2& size, const vec4& color);
   - FlatColorShader->bind();                   // 使用shader程序
   - FlatColorShader->SetFloat4(color)          // 上传统一变量颜色
   - set transform : 根据size,position 计算其缩放和位移
   - FlatColorShader->SetMat4(transform)        // 上传transom矩阵 即对象的模型矩阵
   - 绑定顶点数组对象
   - DrawIndexed() 根据其索引顺序进行绘制 -> 实时渲染程序

7. DrawQuad(const vec2& position, const vec2& size, const Ref<Texture2D> texture);
8. DrawQuad(const vec2& position, const vec2& size, const Ref<Texture2D> texture);
  - 纹理需要使用纹理shader
  - 在上传常规的矩阵同时, 也要让纹理进行绑定上传即可
  - 随后shader上传uniform int指定在片段shader中的纹理过滤器(也就是纹理绑定的那个槽的index) - > 这一步可以直接在初始化时shader就绑定好
  - 未来可以支持纹理和颜色同时存在(混合颜色给予纹理)
  - 顶点数组对象需要扩展下(纹理坐标映射在顶点buffer中体现+layout)

* 想要让图片渲染之间存在深度层级的关系, 需要在opengl init的时候开启:
  * glEnable(GL_DEPTH_TEST);  // 深度测试

* 修改sandbox上的代码
* 首先替换为Render2D, 其次DrawQuad()即可, 不存在shader和submit相关的内容
* 初始化的内容转移到Render2D的Init内容中去
* 增加先前的纹理渲染(棋盘 + 任意其上的图片)

=> 注意事项
* 在图形编程中, 如果要进行切换shader, 这是一个性能消耗很大的事情!

* 优化, 不使用两个shader一个绘制平面color, 一个特地的绘制纹理
- 保留textures shader, 只要能保证在片段shader中, 没有纹理的情况下, 纹理过滤器返回的值是1, 1, 1, 1即可, 这样可以更好的和颜色进行相乘
  - 可以没有任何纹理的时候, 加载一张纯白纹理进行贴图即可, 但是这样太麻烦了, 不如直接扩展shader去创建纹理
- beginscene的时候绑定一次shader即可
- OpenGLRendererAPI DrawIndexed后, 需要glBindTexture(GL_TEXTURE_2D, 0);  不绑定任何纹理程序->清空纹理槽
- 扩展Texture, 可以创建纯白色的纹理
- Renderer2DStorage中添加Texture WhiteTexture
  - 设置为1x1
  - data数据就是 => uint32_t whiteTextureData = 0xffffffff;


## 优化2d渲染API
* textureshader中, 新增新的uniform float u_TilingFactor, 它将与在纹理取值中的纹理坐标进行相乘
  * 默认给1.0f
* shader 新增SetFloat -> 上传共享变量float
* 在渲染纹理的相关api中, 新增平铺因子整个默认形参 -> 默认值为1.0f

* 此时取决于纹理采样超出纹理坐标值之后应该怎么做，当前设置的是平铺
  * glTextureParameteri(m_renderer_id, GL_TEXTURE_WRAP_S, GL_REPEAT);

* 添加旋转 -> z轴方向 float rotation(弧度值)
* 旋转需要额外处理选准矩阵，最好和其他的api分开 DrawRotatedQuad

* 为绘制纹理的api增加colcor设置
  * vec4 tintColor = 1.0f


## renderer2d 批绘制处理
* 创建一个动态的顶点缓冲对象

* struct QuadVertex
  * glm::vec3 Position;  // 位置
  * glm::vec4 Color;      // 颜色
  * glm::vec2 TexCoord;  // 纹理坐标
  * //TODO: texid

* Renderer2DStorage -> Renderer2DData(将其之前由堆空间分配直接改为静态数据对象)
  * const uint32_t MaxQuads = 10000;  // 最多1w个四边形的同时绘制
  * const uint32_t MaxVerteices = MaxQuads * 4;  // 最多4w个顶点的同时绘制 
  * const uint32_t MaxIndices = MaxQuads * 3 * 2;  // 最多6w个索引的同时绘制(一个四边形存在两个三角形, 每个三角形存在3个索引个数)
  * Ref<VertexBuffer> QuadVertexBuffer;
  * uint32_t QuadIndexCount = 0; 
  * QuadVertex* QuadVertexBufferBase = nullptr;
  * QuadVertex* QuadVertexBufferPtr = nullptr;


### VertexBuffer
* Create(uint32_t size); ->不传入具体的顶点缓冲数据创建
  - OpenGLVertexBuffer: 创建buffer, 绑定ARRAY_BUFFER, 传入数据(区分在于数据并不直接传入, 而是传入一个空指针, 并且不再是静态绘制数据, 而是动态绘制数据GL_DYNAMIC_DRAW)
* SetData(const void*, uint32_t size)
  - OpenGLVertexBuffer:
    - 绑定当前顶点缓冲区
    - glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);


### IndexBuffer
* 注意当前hazel只支持32-bit的索引缓冲区


### Renderer2D::init()
* 创建一个顶点数组对象VertexArray
* 创建一个固定大小的顶点缓冲区, 动态的改变buffer内容: 大小为MaxVertices * sizeof(QuadVertex)
* 其中将squareVB->quadVB
* 布局如QuadVertex, 并且将其添加到VertexArray中
* 初始化数据内的四边形对象数组QuadVertexBufferBase, new QuadVertex[MaxVertices];
* 创建索引数组, new uint32_t[MaxIndices], 大概240kb大小
* 创建顶点buffer, 个数为MaxIndices
* 初始化索引数组: 遍历所有的索引数组值, 以6个为一组, 012, 230, 增加offset(offset + 1...), 标识不同的索引值, 最后offset+=4.
* 设置到顶点数组对象中, 然后释放掉索引数组的内容(由于当前是单线程调用, 所以没有管很多细节)
* 创建单位白色像纹理, 读取纹理shader，设置其使用纹理采用器绑定到0号槽位

### Renderer2D::BeginScene
* 绑定纹理shader, 上传相机的pv矩阵(投影矩阵 * 视图矩阵)
* QuadIndexCount = 0;
* 将当前renderer数据的四边形buffer指针指向base地址

### Renderer2D::DrawQuad
* 将当前的四边形顶点对象(QuadVertexBufferPtr)中的位置, 颜色，纹理坐标(0,0)
  * 重复四遍, 以传入的点为左下角开始计算, 逆时针旋转
* QuadIndexCount++;
* 批处理开始, 此处并不会去绘制索引, 也就是说这里就只是单纯的数据准备


### RendererAPI::DrawIndexed(VertexArray, uint32_t indexCount = 0)
* 扩展此接口, 如果 indexCount = 0就使用完整的index缓冲区内的所有索引, 否则使用指定的
- OpenGLRendererAPI:
  - count = ....
  - glDrawElements...

### Renderer2D::Flush
* RenderCommand::DrawIndexed(QuadVertexArray, QuadIndexCount * 6);

### Renderer2D::EndScene
* 实际上在此处应该进行渲染，处于性能分析的缘故, 会增加一个函数flush进行渲染
* 将所有使用到的数据进行上传
* QuadVertexBuffer->SetData(QuadVertexBufferBase, (uint8_t*)QuadVertexBufferPtr - (uint8_t*)QuadVertexBufferBase);

* 修改shader, 和布局一致即可