# DrawCall

## 渲染流和提交
* OpenGL -> glDrawElements

* 现在请想象在3D世界里面渲染一个立方体, 实际上需要什么?
  * VertexArray(VertexBuffer and IndexBuffer), shader
  * 每帧清除屏幕
  * 我们如何看? 相机系统(一个相机用来投影矩阵和视图矩阵)
  * 立方体在世界空间中的实际位置, 需要立方体的整个变换矩阵
  * 立方体的表面材料, 纹理
  * 环境的信息(光照)
* 上述描述的这些都是属于一个**场景**里的信息
* 信息采集完毕, 进入渲染队列, 不会被立即渲染(需要渲染的太多了?),单独的在渲染线程上进行计算......


* 设想接口
  * RenderCommand::SetClearColor();
  * RenderCommand::Clear(): 清屏
  * Renderer::BeginScene(): 在每一个tick中, render需要开始场景, 然后包含关于场景的所有信息. 
    * 要处理的就是相机, 灯光, 环境.... (先隐藏, 目前没有)
  * shader->Bind()
  * Renderer::Submit(): 提交渲染信息(vertexVA...)
  * Renderer::EndScene(): 结束场景, 渲染信息准备完毕
  * Renderer::Flush(): (暂时隐藏, 没有渲染队列)


* 文件:
  * Renderer/RendererAPI.h/.cpp(物理渲染接口, platform每一份有不同的实现)
  * Renderer/RendererCommand.h/.cpp(实际原始渲染指令, 调用物理渲染接口, 不需要关心底层实现)
    * 包含头文件RendererAPI
  * Platfrom/OpenGL/OpenGLRendererAPI.h/.cpp 物理渲染接口的OpenGL实现版本


* class RendererAPI (基础渲染命令, 包含数学库->glm/glm.hpp)
  * enum class API(将之前的rendererAPI迁移过来, None and OpenGL)
  * SetClearColor(const glm::vec4& color);
  * Clear() = 0;
  * DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;
  * static API GetAPI
  * 属性
    * static API s_api;(.cpp里默认定义: OpenGL(当前阶段))

* class OpenGLRendererAPI opengl实现
  * SetClearColor
    * glClearColor(color.a, ...) // glm的vec4中提供了多种别名
  * Clear
    * glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)  // 颜色和深度缓冲区
  * DrawIndexed
    * ``glDrawElements(GL_TRIANGLES, m_square_va->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);``
    * GL_TRIANGLES 是指最小单位为三角形

* class RendererCommand(静态包裹类)
  * SetClearColor
  * Clear
  * DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
    * 调用s_renderAPi的内容
  * 属性:
    * static RendererAPI* s_renderAPi(暂时: cpp里直接new OpenGLRendererAPI类型的)

* class Renderer
  * static void BeginScene()
  * static void EndScene()
  * static void Submit(const std::shared_ptr<VertexArray>&);
    * VertexArray->Bind
    * 应该是提交到渲染任务队列中, 当前直接调用DrawIndexed命令  
  * static GetAPI, 实际调用RenderAPI::GetAPI


## [相机系统](./render_camera.md)


## 整理渲染代码待sandbox中
- 需要暴露给客户端的相关头文件:
  - Renderer/Renderer.h
  - Renderer/RendererCommand.h
  - Renderer/Buffer.h
  - Renderer/Shader.h
  - Renderer/VertexArray.h
  - Renderer/OrthoGraphicCamera.h

- 将之前app内的渲染业务代码整理到sanbox的layer中去
- 添加事件处理让相机移动起来
  - keyPressedEvent
    - 增加相机的坐标和速度设置
    - 当前没有delta time, 所以相机的速度会随着帧率的影响而影响
  - 实际上使用事件系统, 如果只是在触发事件的时候移动, 会让其移动不流畅。可以基于状态或者轮询来移动相机


## [模型矩阵变换](./render_model_transform.md)