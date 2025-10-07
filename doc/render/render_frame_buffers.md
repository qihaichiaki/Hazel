### 帧缓冲区
简单来讲就是讲本来渲染到窗口上的图形渲染到一张纹理上


* Renderer/framebuffer.h/.cpp
* Platform/OpenGL/OpenGLFramebuffer.h/.cpp


* struct FramebufferSpecification  // 帧缓冲区规范
  * uint32_t Width, Height;
  * uint32_t Samples = 1; 
  * bool SwapChainTarget = false;  // 交换链目标, 将帧缓冲区渲染到其上 类似于glBindFrameBuffer(0)


* class Framebuffer
  * getSpecification() const = 0;
  * static Ref<xxx> create(const FramebufferSpecification& spec);
  * bind();  // 需要成为渲染管道的一部分, 后续会移除掉
  * unBind();
  * uint32_t getColorAttachmentRendererID() const = 0;

* class OpenGLFramebuffer
  * public:
    * OpenGLFramebuffer(const FramebufferSpecification& spec)
      * Invalidate();
    * ~OpenGLFramebuffer()
      * glDeleteFrameBuffers(1, &m_rendererID);
    * Invalidate()  // resize
      * glCreateFramebuffers(1, &m_rendererID);
      * glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
      * // 1 颜色缓冲区
      * glCreateTexures(GL_TEXTURE_2D, 1, &m_colorAttachment);  // 颜色缓冲区 
      * glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
      * glTexImage2D(GL_TEXURE_2D, 0, GL_RGBA8, m_specification.w, m_specification.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);  // 这样创建方便采样？
      * glTextureParameteri(GL_TEXURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 线性插值 
      * glTextureParameteri(GL_TEXURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      * glFrameBufferTexure2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXURE_2D, m_colorAttachment, 0)  // 支持最多32种不同的贴图附件
      * // 2 深度缓冲区
      * glCreateTexures(GL_TEXTURE_2D, 1, &m_depthAttachment);  // 颜色缓冲区
      * glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
      * glTextureStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_specification.w, m_specification.h);
      * // glTexImage2D(GL_TEXURE_2D, 0, GL_DEPTH24_STENCIL8, m_specification.w, m_specification.h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
      * glFrameBufferTexure2D(GL_FRAMEBUFFER, GL_DEPTH_STENCI_ATTACHMENT, GL_TEXURE_2D, m_depthAttachment, 0);
      * // 3 检查一下帧缓冲区的状态是否完整
      * HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
      * glBindFramebuffer(GL_FRAMEBUFFER, 0);
    * bind()
      * glBindFramebuffer(GL_FRAMEBUFFER, m_rendererID);
    * unbind()
      * glBindFramebuffer(GL_FRAMEBUFFER, 0);
  * private:
    * uint32_t m_rendererID;
    * uint32_t m_colorAttachment;  // 颜色缓冲区
    * uint32_t m_depthAttachment;  // 深度缓冲区
    * FramebufferSpecification m_specification; 