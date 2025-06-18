#pragma once

#include "framework.h"
#include "RenderContext.h"

class RenderObject;
class RenderPipeline;
class RenderWindow;

class RenderTarget
{
public:
    
    RenderTarget(VkFormat format, int width, int height);
    ~RenderTarget();
    
    VkRenderPass& getRenderPass();
    
    void setRenderContext(RenderContext* renderContext);
    RenderContext& getRenderContext();

    VkImageView createImageView(VkImage image, VkFormat format);

private:

    RenderContext* m_renderContext;  
    
    VkExtent2D extent{};
    
    VkRenderPass m_renderPass;
    
    std::vector<VkImage>        m_images;
    std::vector<VkImageView>    m_imageViews;
    std::vector<VkFramebuffer>  m_framebuffers;

    void createRenderPass(VkFormat format);

};
