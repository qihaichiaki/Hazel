# 材质系统

* shader需要灵活的编写, 而不是硬编码的输出某一种或者某一类
* shader可以接收外界的输入, 从而输出不同的颜色

* 输入时重点, 可以是由外界渲染物体的材质造成的, 纹理等等...... 
* 材质实际上就是一个shader, 代码定义了要混合的算法, 决定最终需要输出的颜色, 并且接收统一变量的输入(因为要在每帧绘制前发生改变)

* shader新增unloadUniformFloat4, 接收vec4
  * glUniform4f....

* sandbox
  * 尝试使用两种颜色进行渲染网格
    * 使用颜色shader上传统一变量即可


* 材质系统: 实际上时将所有shader的统一变量进行缓存或者统一进行设置, 因为它不需要每一帧的进行设置, 纹理也是如此......
