#pragma once

#include "framework.h"

class RenderWindow;

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;

    Vertex();
    Vertex(float x, float y, float z, float xN, float yN, float zN, float xT, float yT);

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        // layout(location(#location) = 0) in vec2(#format) inPosition;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoords);
        
        return attributeDescriptions;
    }
};

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
};

class Mesh
{

    RenderWindow* m_window;
    MeshData* m_meshData;
    
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexUploader;

    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferUploader;

public:
    Mesh(RenderWindow& window, MeshData* data);
    ~Mesh();

    VkBuffer const& getVertexBuffer() const;
    VkBuffer const& getIndexBuffer() const;
    std::vector<Vertex> const& getVertices() const;
    uint32_t getIndexCount() const;
    
};
