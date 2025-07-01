# 渲染架构

渲染系统最终存在两个方面:
| Render Api     | Renderer               |
| -------------- | ---------------------- |
| Render Context | 2D/3DRenderer          |
| Swap chain     | Scene Graph            |
| Frame Buffer   | Sorting                |
| Vertex Buffer  | Culling(剔除)          |
| index Buffer   | Materials(材质)        |
| Texture        | LOD                    |
| Shader         | Animation              |
| States         | Camera                 |
| Piplines       | VFX (视觉效果)         |
| Render Passes  | PostFX(后期效果)       |
|                | 随机, 反射, 环境光遮蔽 |

其中, RenderApi相当于是渲染原语, 通过其告诉GPU应该怎么做, 对此不同的技术存在不同的一份实现(OpenGL, Vulcan......)
材质和shader紧密相关, 材质实际上就是一个着色器机上一些统一的数据
渲染指令队列: 将每个渲染指令编码为一个大缓冲区的方法

1. 使用OpenGL 
   - 最简单的接口, 需要外部强大的设计才能使其发挥真正作用
2. 构建Render Api(材质系统......)
3. 构建Renderer
4. 实现基本的渲染器
5. 实现其他功能
   - 比如使用其他的底层库实现