#include "Mesh.h"

#include "RenderWindow.h"

Mesh::Mesh(RenderWindow& window, MeshData const& dMesh) : m_vertexBuffer(nullptr)
{
    
    m_window = &window;
    m_meshData = dMesh;

    uint64_t vSize = sizeof(dMesh.vertices[0]) * dMesh.vertices.size();

    VkBuffer stagingBuffer = nullptr;
    VkDeviceMemory stagingBufferMemory = nullptr;
    window.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory, vSize);

    void* data;
    vkMapMemory(window.getDevice(), stagingBufferMemory, 0, vSize, 0, &data);
    memcpy(data, dMesh.vertices.data(), vSize);
    vkUnmapMemory(window.getDevice(), stagingBufferMemory);

    window.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_vertexBuffer, m_vertexUploader, vSize);

    window.copyBuffer(stagingBuffer, m_vertexBuffer, vSize);

    VkDeviceSize iSize = sizeof(dMesh.indices[0]) * dMesh.indices.size();
    
    vkMapMemory(window.getDevice(), stagingBufferMemory, 0, iSize, 0, &data);
    memcpy(data, dMesh.indices.data(), iSize);
    vkUnmapMemory(window.getDevice(), stagingBufferMemory);

    window.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_indexBuffer, m_indexBufferUploader, iSize);

    window.copyBuffer(stagingBuffer, m_indexBuffer, iSize);

    vkDestroyBuffer(window.getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(window.getDevice(), stagingBufferMemory, nullptr);
    
}

Mesh::~Mesh()
{

    vkDestroyBuffer(m_window->getDevice(), m_indexBuffer, nullptr);
    vkFreeMemory(m_window->getDevice(), m_indexBufferUploader, nullptr);
    
    vkDestroyBuffer(m_window->getDevice(), m_vertexBuffer, nullptr);
    vkFreeMemory(m_window->getDevice(), m_vertexUploader, nullptr);
    
}

VkBuffer const& Mesh::getVertexBuffer() const
{
    return m_vertexBuffer;
}

VkBuffer const& Mesh::getIndexBuffer() const
{
    return m_indexBuffer;
}

std::vector<Vertex> const& Mesh::getVertices() const
{
    return m_meshData.vertices;
}

uint32_t Mesh::getIndexCount() const
{
    return m_meshData.indices.size();
}
