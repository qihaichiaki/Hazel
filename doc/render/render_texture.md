## Texture

* 纹理实际上是我们已接收并上传到GPU中的内存缓冲, 在片段着色器中确定像素的颜色
* 法线贴图, .......
* 使用这些纹理, 是因为需要在渲染着色器中对某个像素进行反射率取值
 
* 基于物理的渲染(PBR). 一些难以计算的值可以事先存储到纹理缓冲中, 运行时直接读取即可
* 本质上修改顶点缓冲区, 在正方形的每个角上包含纹理坐标, 修改shader, 接收2D纹理槽.

* sandbox中具体操作:
  1. 首先显示一个1.5倍大的正方形
  2. 修改顶点缓冲区, 增加纹理坐标(沿着x轴正向)
     - 左下角0, 0
     - 右下角1, 0
     - ...
  3. 修改顶点坐标布局
     - 增加float2, a_TexCoord
  4. 新建textureShader(复制之前的shader代码)
     - vertex shader获取实际的纹理坐标, 发送到片段着色器(out vec2 v_TexCoord)
     - fragment shader中将接收到的纹理坐标先映射到颜色上, 验证是否正确（用颜色验证数据是比较常见的, 因为GPU只能输出验证颜色）

* Renderer/Texture类
  * virtual uint32_t GetWidth() const = 0;
  * virtual uint32_t GetHeight() const = 0;
  * virtual void Bind(uint32_t slot = 0) const = 0;

* class Texture2d
  * static Ref<Texture2D> Create(const std::string& path);  // 类似之前的renderer类实现

* Platform/OpenGL/OpenGLTexture2d
  * 属性:
    * 保留路径(用于调试和引擎的开发版很有用, 可能希望引擎热重新加载纹理-> 实际上应该是资产管理器该干的活)
    * 宽度, 高度
    * renderID
  * 加载图像(构造函数)
    * 在引擎里的资产不会依赖这些png, jpg这些图像格式, 而是会制作自己的纹理格式, 构建时间构造运行时加载的格式
    * 最简单的是使用std_img(单一的头文件) ->  vendor/std_img/stb_image.h (添加.cpp文件, 包含头文件和相关宏)
    * stbi_uc* stbi_load(路径, 出参 (x,y, 通道数), 图像转换为需要的格式(通常情况下是0)) -> 默认情况下是从顶部到底部读取, 而OPENGL需要从底部到顶部进行渲染(前面可以设置stbi_set_flip_vertically_on_load(1) 让其翻转加载)
      * stbi_uc 无符号的char数组
      * 失败了nullptr, 可以先断言验证是否正确
    * 实际获取数据缓冲区, 并将其上传到OpenGL->GPU
    * glCreateTextures(GL_TEXTURE_2D, 1, &id);
    * glTextureStorage2D(id, 1(弄清有多少mipmaps? 1级别), GL_RGB8, width, height);  // GPU申请内存存储
    * glTextureParameteri(id, )  // 设置一些纹理参数
      - GL_TEXTURE_MIN_FILTER 使用什么样的过滤进行缩小
      - GL_LINEAR(不处理mipmaps不需要), 线性插值计算我们想要什么颜色
    * glTextureParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR)  // 使用这样的过滤进行放大
      - GL_NEAREST, 让图像放大不模糊(使用线性会模糊)
    * glTextureSubImage2D(id, 0级别, xoffset = 0, yoffset=0, width, height, GL_RGB(也可以由前面的通道获得), GL_UNSIGNED_BYTE, data);  // 上传纹理数据
    * stbi_image_free(data);  // cpu 上的内存释放
  * 析构函数
    * deleteTexctures, 删除id
  * Bind(uint32_t slot)
    - glBindTextureUint(slot, id);  // 在槽slot绑定

* sanbox中创建assets/textures/.png
  * 首先hazel包含texture的头文件
  * Texture Ref成员
  * 通过相对路径加载资源
  * TextureShader, 上传uniformInt, "u_Texture", 0 -> 0是绑定的纹理槽, 所以采样器实际上就是在texure中上传的绑定的对应的纹理槽上
  * 在正方形渲染前, bind texture
  * 修改TextureShader中进行采样, 
    * 片段着色器: uniform sampler2D u_Texture;  // 对2D纹理的采样
    * color = texture(u_Texture, v_TexCoord);  // 采样器 + 纹理坐标


* 但是Texture如果读取了带有透明通道的图片就会出现问题:  
  * OpenGLTexture2D支持RGB和RGBA
    * GLenum internalFormat, dataFormat = 0; if ==4, GL_RGBA8, GL_RGBA, else GL_RGB8, GL_RGB ->断言不要让其他通道进来(表示不支持这种通道处理), 通关两者==0判断
  
* 混合是指OpenGL处理Alpha通带和带Alpha的对象的方式
  * 没有完全透明的情况下, 如何将颜色组合在一起 
* 创建完渲染器, 进行一系列的初始化, 这个时候就需要进行启动混合
* 在application, 创建完window后，调用Renderer::Init();
  * RendererCommand::Init();

* RenderAPI->Init()
  * glEnable(GL_BLEND);  // 启用混合
  * glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // 经典混合函数 