#pragma once
#include <string>
#include "framework.h"

class RenderContext;
class RenderTarget;
class RenderWindow;

class Texture
{
public:
    Texture(RenderTarget& renderTarget, std::string const& textureFile);
    ~Texture();

    VkImageView& getImageView();
    
    static const inline std::string TEXTURE_FOLDER = "res\\textures\\";

private:
    VkImage m_textureImage;
    VkImageView m_textureImageView;
    
    VkDeviceMemory m_textureImageMemory;

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void createTextureImageView(RenderTarget& renderWindow);
    
    void transitionImageLayout(RenderContext& renderWindow, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(RenderContext& renderWindow, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
