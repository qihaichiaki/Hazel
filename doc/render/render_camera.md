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