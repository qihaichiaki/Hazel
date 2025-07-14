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

4. EndScene();

5. DrawQuad(const vec2& position, const vec2& size, const vec4& color);
   - 6的z坐标为0, 其中z标识图层关系

6. DrawQuad(const vec3& position, const vec2& size, const vec4& color);
   - FlatColorShader->bind();                   // 使用shader程序
   - FlatColorShader->SetFloat4(color)          // 上传统一变量颜色
   - set transform : 根据size,position 计算其缩放和位移
   - FlatColorShader->SetMat4(transform)        // 上传transom矩阵 即对象的模型矩阵
   - 绑定顶点数组对象
   - DrawIndexed() 根据其索引顺序进行绘制 -> 实时渲染程序


* 修改sandbox上的代码
* 首先替换为Render2D, 其次DrawQuad()即可, 不存在shader和submit相关的内容
* 初始化的内容转移到Render2D的Init内容中去