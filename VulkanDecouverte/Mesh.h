#pragma once

#include "framework.h"

class RenderWindow;

struct Vertex
{
    vec3 position;
    vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        // layout(location(#location) = 0) in vec2(#format) inPosition;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        
        return attributeDescriptions;
    }
};

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    static MeshData& Cube()
    {
        static MeshData cubeData {};

        cubeData.vertices = {
            // FRONT
            {{ -.5f, -.5f, -.5f }, {  0, 0, 0, 1 }},	// 0
            {{ -.5f,  .5f, -.5f }, {  0, 1, 0, 1 }},	// 1
            {{  .5f,  .5f, -.5f }, {  1, 1, 0, 1 }},	// 2
            {{  .5f, -.5f, -.5f }, {  1, 0, 0, 1 }},	// 3

            // BACK
            {{ -.5f, -.5f,  .5f }, {  0, 0, 1, 1 }},	// 4
            {{  .5f, -.5f,  .5f }, {  1, 0, 1, 1 }},	// 5
            {{  .5f,  .5f,  .5f }, {  1, 1, 1, 1 }},	// 6
            {{ -.5f,  .5f,  .5f }, {  0, 1, 1, 1 }},	// 7 

            // LEFT
            {{ -.5f, -.5f,  .5f }, { 0, 0, 1, 1 }},	// 8
            {{ -.5f,  .5f,  .5f }, { 0, 1, 1, 1 }},	// 9
            {{ -.5f,  .5f, -.5f }, { 0, 1, 0, 1 }},	// 10
            {{ -.5f, -.5f, -.5f }, { 0, 0, 0, 1 }},	// 11

            // RIGHT
            {{  .5f, -.5f, -.5f }, {  1, 0, 0, 1 }},	// 12
            {{  .5f,  .5f, -.5f }, {  1, 1, 0, 1 }},	// 13
            {{  .5f,  .5f,  .5f }, {  1, 1, 1, 1 }},	// 14
            {{  .5f, -.5f,  .5f }, {  1, 0, 1, 1 }},	// 15

            // TOP
            {{ -.5f,  .5f, -.5f }, {  0, 1, 0, 1 }},	// 16
            {{ -.5f,  .5f,  .5f }, {  0, 1, 1, 1 }},	// 17
            {{  .5f,  .5f,  .5f }, {  1, 1, 1, 1 }},	// 18
            {{  .5f,  .5f, -.5f }, {  1, 1, 0, 1 }},	// 19

            // BOTTOM
            {{ -.5f, -.5f,  .5f }, {  0, 0, 1, 1 }},	// 20
            {{ -.5f, -.5f, -.5f }, {  0, 0, 0, 1 }},	// 21
            {{  .5f, -.5f, -.5f }, {  1, 0, 0, 1 }},	// 22
            {{  .5f, -.5f,  .5f }, {  1, 0, 1, 1 }},	// 23
        };

        cubeData.indices = {
            // FRONT
            0, 1, 2,
            0, 2, 3,
		
            // BACK
            4, 5, 6,
            4, 6, 7,
		
            // LEFT
            8, 9, 10,
            8, 10, 11,

            // RIGHT
            12, 13, 14,
            12, 14, 15,

            // TOP 
            16, 17, 18,
            16, 18, 19,

            // BOTTOM
            20, 21, 22,
            20, 22, 23
        };

        return cubeData;
    }
};

class Mesh
{

    RenderWindow* m_window;
    MeshData m_meshData;
    
    VkBuffer m_vertexBuffer;
    VkDeviceMemory m_vertexUploader;

    VkBuffer m_indexBuffer;
    VkDeviceMemory m_indexBufferUploader;

public:
    Mesh(RenderWindow& window, MeshData const& data);
    ~Mesh();

    VkBuffer const& getVertexBuffer() const;
    VkBuffer const& getIndexBuffer() const;
    std::vector<Vertex> const& getVertices() const;
    uint32_t getIndexCount() const;
    
};
