## Shader抽象

* 稍微重构shader类, 让其解耦OpenGL
* 实际上就是单纯的虚拟化, opengl给出个实现类即可
* 和顶点数组抽象类似, 暴露静态创建接口, 在.cpp中根据render api进行选择


* 对于上传统一变量相关接口删除, 它们应该是和类似于材质这类对象绑定到一起上传......
* 当前渲染,后端实现的openglshader可以进行暴露, 用它临时上传统一变量的数据

* 暂时增加一些变量上传?
* 增加简单的imgui内容, 能够动态的改变网格的格子颜色
  * ColorEdit4()..


### 着色器资源文件
* shader通常编译一次留下的字节数据是可以保留的(每个平台初次使用时进行编译), 这样每次使用就不需要频繁编译。
  * 先不考虑这个, 尚未达到阶段， 并且opengl似乎不支持读取二进制的shader数据
* 并且当前的shader需要提供一种能直接从本地文件读取编译的功能

* api流程:
  1. 创建着色器文件并且编写着色器代码
  2. 磁盘中读取文件, 拆分为两个部分字符串(顶点和片段), 参与shader程序的编译和链接 
  3. 和之前的shader操作一致......

* 资产文件: assets/shaders/texture.glsl
  * glsl(opengl的shader文件)

* 后续可以将glsl变成我们自己的着色语言->能够增加更多标识符, 从而有助于Hazel理解某些区域的预期用法
* 文件中使用``#type``区分shader类型:
  * vertex
  * fragment


* Shader
  * static Shader* Create(const std::string& path);

* OpenGLShader
  * OpenGLShader(filepath)

* 创建compile来代替构造函数里做的编译链接相关操作
* 创建一个文件输入流(暂时使用, 未来需要引进虚拟文件系统) ifsteam std::ios::in, std::ios::binary(二进制格式读取) in
* 创建string result
* 如果没打开进行断言
  * in.seekg(0, std::ios::end);  // 到达文件的尽头
  * result.reset(in.tellg());  // 实际指针在哪里, 因为在末尾, 所以就是文件的大小
  * in.seekg(0, std::ios::beg);  // 回到开头
  * in.read(&result[0], result.size());  // 读取整个文件
  * in.close();

* 新建函数: string readFile(const std::string& filepath) 就是上述的内容
* 构造函数中获取到加载的字符后, 进行处理
* 新建函数: std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
  * 将不同的shader类型的shader程序片段分别放到对应的key下面
  * 字符串处理:
    * typeToken是#type
    * 先fin到此位置pos
    * 如果pos != std::string::npos
      * size_t eol = source.find_first_of("\r\n", pos);  // 找到新行
      * 断言 eol != std::string::npos(表示必须存在) syntax error
      * size_t begin = pos +  typeTokenLenghth(strlen(char*)计算) + 1;
      * string type = source.substr(begin, eol - begin);
      * 断言是否等于vertex || fragment || pixel 不是则非法type
      * size_t nextLinePos = source.find_first_of("\r\n", eol);
      * pos = source.find(typeToken, nextLinePos);
      * shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos? source.size() - 1 : nextLinePos));

* 静态函数 ShaderTypeFromString 根据type返回不同的opengl shader类型: GL_VERTEX_SHADER/ GL_FRAGMEBT_SHADER;  否则请断言
* 新建函数 compile(const  std::unordered_map<GLenum, std::string>&) 进行处理