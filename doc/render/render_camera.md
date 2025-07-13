# Camera

## 相机原理
* 相机提供了一种观察世界的手段
* 可视范围 -> 视野。一般视野的角度在45°~65°之间合适, 如果大于, 则会在边缘周围相当多的扭曲 
* 分析一件事情的时候可以尝试从数据入手

* 相机的transform
  * 实际上是被观察物体们做的类似transform变换
  * 并且是做相机的相对运动

* 相机是虚拟想象的, 并且是基于场景进行变换的

* 定义相机变换位置的数据是一个矩阵, 是一个变换矩阵.
* 相机看到的画面是视图矩阵。
* 对于每一个渲染物体, 拿到相机的view matrix, 进行求逆, 然后参与变换, 得到的数据其实就是相机渲染时用到的数据

* 相机放在空间中的某个位置, 给他个方向. 这两个数据组成了一个变换矩阵(位置和旋转)
* 相机存在纵横比, 这也是相机所能看到的视野, -> 投影矩阵（实现透视效果?）

* 实际的API和数据准备
  * 立方体: 
    1. model matrix(transform 定义了它在世界上的位置) 
    2. vertex positions(顶点数据- 属于一种网格)
  * 相机
    1. view matrix(视图矩阵定义了它在世界上的位置)
    2. projection matrix(投影矩阵定义了视野) 
    - PV VP

* 最后的运算:
  * **``projection matrix * view matrix * model martrix * vertex positions``** -> 得到每个顶点的实际渲染坐标
  * 出现在屏幕外就会被剪切
  * 区别: DirectX是主行, 所以是上述矩阵反过来

* 对于相机矩阵数据的准备, 比如VP矩阵相乘, 可以在C++代码中实现。
* 但是对于每个渲染对象乘以此, 可以放在最后的shader中完成(比如vertex shader? 或者计算完毕后上传-> CPU里做批处理?)
    - 比如cpu准备mvp矩阵, 随后在vertex shader中乘以最后的顶点坐标, 完成渲染位置的计算

* 相机渲染流程总结:
  1. beginScene(Camera)
     - 提交此视图的projection matrix, view matrix
     - 取到`所有`shader, 将vp矩阵作为统一的着色器上传到我们的着色器当中(统一变量)(并不是实时的传递, 可以先缓存, 因为此时可能没有任何一个对象上传数据 -> 多线程渲染任务, 将相机此时的矩阵数据拷贝即可, 引用是非常危险的)
  2. 绑定不同的shader, 渲染所有对象
  3. endScene(什么都不做......也许可以做一个begin保护?)

* 针对于统一变量, 可以说是不受位置所影响的
  * 对于相机: 环境(光照...)
  * 对于渲染对象: 材质(反射效果, color)


## 创建正交相机
* 首先这是一个2d相机, 因为z轴没有变换, 始终为0
* 其次是一个正交相机, 投影矩阵是正交的(将观察范围正交的映射到一个矩形上(没有近大远小的效果))
* Renderer/OrthoGraphicCamera.h/.cpp

* class OrthoGraphicCamera
  * 构造函数(float left, right, bottom, top)
    * projection_matrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f));
      * glm/gtc/matrix_transform
    * view_matrix(1.0f) 初始化为单位矩阵
    * view_projection_matrix = projection_matrix * view_matrix;(需要注意glm是主列的, 所以这里需要反过来)
  * 数据
    * mat4 projection_matrix
    * mat4 view_matrix(角度和位置)
      * 实际transform的逆矩阵
    * mat4 view_projection_matrix;  // vp 缓存
    * vec3 position(注意初始化为0)
    * float rotation(传入的是角度, 应用的时候注意转换为弧度)
  * set/get Position
  * set/get Rotation
    * 绕着z轴旋转
  * private: RecalculateViewMatrix 重新计算view矩阵
    * 基本是在设置自身属性时改变的东西(位置和旋转)
    * transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.f, 0.f, 1.0f));
    * view_matrix = glm::inverse(transform);  // 取逆
    * view_projection_matrix = projection_matrix * view_matrix;(需要注意glm是主列的, 所以这里需要反过来)
  * get projectionMatrix const
  * get viewMatrix const
  * get viewProjectionMatrix const


* Shader
  * UploadUniformMat4(name, mat4& matrix) 使用此上传统一变量前, 需要进行绑定(也就是使用shader程序), name和shader中统一变量名字一致
    * GLint location = glGenUniformLocation(render_id, name.c_str());  // 检索统一变量在当前shader程序中的位置, 方便下面传输过去
    * glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix))  // 主列不需要转置 value_ptr需要包含glm/gtc/type_ptr.hpp


* Renderer
  * BeginScene(OrthoGraphicCamera&)
    * scene_data 设置相机的viewProjectionMatrix缓存数据
  * submit(const shader_ptr<shader>& shader, vertexArray)
    * shader进行绑定
    * shader先硬编码上传对应的pv矩阵
  * render 存在缓存数据, 暂时叫struct SceneData
    * viewProjectionMatrix;
  *  static SceneData* scene_data



* app内使用
  * 初始化相机 -1.0f, 1.0f, -1.0f, 1.0f
  * vertex shader
    * uniform mat4 u_ViewProjection;
    * gl_Position = u_ViewProjection * vec4(a_Position, 1.0)
  * 在shader submit前, 给shader上传相机的pv统一变量
  * 测试:
    * 如果投影矩阵的边界变大, 那么里面的对象应该变小
      * 这里可以发现问题, 因为坐标数据本来渲染正方形的, 但是却渲染成了矩形
      * 投影矩阵需要设置和屏幕正确的纵横比(如果投影矩阵设置和屏幕一样的纵横比, 你会发现顶点数据的比例被正确渲染出来了)
    * 设置相机位置, 查看是否偏移
    * 设置相机旋转, 查看是否旋转  
    * 组合测试


* 渲染对象流程总结:
  * 首先对象基于自身变换, 得到世界坐标(当前就是position)
  * 让对象需要根据相机的视图矩阵进行变换(相机的transform矩阵的逆矩阵, 因为是相机移动/缩放/渲染相反的方向)
  * 然后将视图矩阵和投影矩阵进行变换, 从而映射到屏幕坐标点上进行渲染


## 摄像机控制器
* 封装一个对正交相机的控制器, 方便提供给客户端一个基本的摄像机控制, 而非需要客户端对相机进行脚本处理

* 正交相机类应该完全只是对投影矩阵和视图矩阵的简单表达

* 暂时放在: hazel/OrthoGraphicCameraController.h/.cpp

* OrthoGraphicCamera
  * setProjection(float left, float right, float bottom, float top);

* class OrthoGraphicCameraController
  * 属性: 
    * float m_aspect_ratio;
    * float zoom_level = 1.0f;  // 缩放级别
    * OrthoGraphicCamera m_camera;
    * vec3 m_camera_position;   // 相机位置
    * float m_camera_rotation;  // 相机旋转角度
    * float m_camera_translation_speed = 5.0f
    * float m_camera_rotation_speed = 180.0f;
  * 构造函数
    * 垂直方向上: 默认下是存在两个单位的空间
    * 水平方向上: 宽高比乘以2个单位 -> 正确得到宽高比
    * OrthoGraphicCameraController(float aspect_ratio);
      * aspect_ratio = width:height(纵横比)
      * 初始化正交相机: -aspect_ratio * zoom_level, aspect_ratio * zoom_level, -zoom_level, zoom_level

  * onUpdate(Timestep ts);
    * 摄像机的上下左右以及旋转均可以采用之前sandbox中的
    * 键位换成wasd, qe左右旋转
  * onEvent(Event& e);
    * 使用调度器调度选择onMouseScrolled和onWindowResized两个自注册事件处理
  * const getCamera() const {}
  * private: onMouseScrolled()
    * zoom_level -= getYOffset * 0.25f  // 往上是在放大显示图像, 摄像机的范围减少, 同理可得往下是缩小显示图像, 摄像机范围增大
      * max限制在最小为0.25f
      * 让m_camera_translation_speed速度和zoom_level保持一致, 这样能做到范围变大时, 速度变快, 范围变小时, 速度变小
    * setProjection, 和构造函数初始化时做一样的操作
  * private: onWindowResized()
    * m_aspect_ratio = 1.0f * e.GetWidth() / e.GetHeight();
    * setProjection


* 调整窗口大小
  * OpenGL会利用渲染缓冲区(像素数据)中的数据通过shader会将其渲染到一个目标上, 缓冲区的大小必须和我们渲染的像素尺寸相匹配
  * 使用GL viewport 设置正在渲染的视口
  * 不会直接渲染到屏幕上(不会再opengl环境进行基本渲染), 应该是设置自己的帧缓冲区， 即渲染在一个纹理上, 然后将这个纹理渲染到屏幕上
  * 另外一个窗口大小需要考虑的是你是在修改范围还是真正的缩放大小
  * 正交相机应该考虑的是裁剪范围?

* OpenGLRendererAPI: SetViewport
  * glViewport(x, y, w, h);  ....  // 临时, 后面需要渲染到自定义的缓冲区上

* RedererCommand::SetViewport
  * render_api -> xxx

* Renderer
  * static void OnWindowResize(uin32_t width, height);
    * RedererCommand::SetViewport(0, 0, width, height);  // 设置渲染视口

* Application
  * 在onEvent中调度新增处理OnWindowResize
  * 最小化的时候, 窗口的高度和宽度都是被设置为0
    * 最小化时, 应该让所有layer的更新停止(其中一个任意为0, 都应该停止？)
      * minimized = true
      * update控制
  * Renderer::OnWindowResize(w, h);

* OrthoGraphicCameraController
  * set/getZoomlevel设置缩放级别

* 上面实现的是场景元素的放大和缩小, 但是真正想实现的是范围的缩放， 所以对相机的投影矩阵的缩放进行变化
* 上述在宽度的变化中是范围的缩放, 但是在纵向上是元素的放大和缩小