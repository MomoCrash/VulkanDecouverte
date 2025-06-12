#include "Mesh.h"

#include "RenderWindow.h"

Vertex::Vertex(): position(0, 0, 0), normal(0, 0, 0), texCoords(0, 0) {}

Vertex::Vertex(float x, float y, float z, float xN, float yN, float zN, float xT, float yT)
    : position(x, y, z), normal(xN, yN, zN), texCoords(xT, yT) {}

Mesh::Mesh(RenderWindow& window, MeshData const& dMesh) : m_vertexBuffer(nullptr)
{
    
    m_window = &window;
    m_meshData = dMesh;

    uint64_t vSize = sizeof(dMesh.Vertices[0]) * dMesh.Vertices.size();

    VkBuffer stagingBuffer = nullptr;
    VkDeviceMemory stagingBufferMemory = nullptr;
    window.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory, vSize);

    void* data;
    vkMapMemory(Application::getInstance()->getDevice(), stagingBufferMemory, 0, vSize, 0, &data);
    memcpy(data, dMesh.Vertices.data(), vSize);
    vkUnmapMemory(Application::getInstance()->getDevice(), stagingBufferMemory);

    window.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_vertexBuffer, m_vertexUploader, vSize);

    window.copyBuffer(stagingBuffer, m_vertexBuffer, vSize);

    VkDeviceSize iSize = sizeof(dMesh.Indices[0]) * dMesh.Indices.size();
    
    vkMapMemory(Application::getInstance()->getDevice(), stagingBufferMemory, 0, iSize, 0, &data);
    memcpy(data, dMesh.Indices.data(), iSize);
    vkUnmapMemory(Application::getInstance()->getDevice(), stagingBufferMemory);

    window.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_indexBuffer, m_indexBufferUploader, iSize);

    window.copyBuffer(stagingBuffer, m_indexBuffer, iSize);

    vkDestroyBuffer(Application::getInstance()->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(Application::getInstance()->getDevice(), stagingBufferMemory, nullptr);
    
}

Mesh::~Mesh()
{

    vkDestroyBuffer(Application::getInstance()->getDevice(), m_indexBuffer, nullptr);
    vkFreeMemory(Application::getInstance()->getDevice(), m_indexBufferUploader, nullptr);
    
    vkDestroyBuffer(Application::getInstance()->getDevice(), m_vertexBuffer, nullptr);
    vkFreeMemory(Application::getInstance()->getDevice(), m_vertexUploader, nullptr);
    
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
    return m_meshData.Vertices;
}

uint32_t Mesh::getIndexCount() const
{
    return m_meshData.Indices.size();
}
